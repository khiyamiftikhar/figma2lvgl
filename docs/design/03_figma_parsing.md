# figma2lvgl — Figma Parsing

All parsing logic lives in `core/figma_parser.py` and `core/utils/figma_helpers.py`. The parser reads Figma XML exported by the FigML plugin and produces a tree of `ParsedNode` objects. It has no knowledge of C code or templates.

---

## XML Structure

The FigML plugin exports:

```xml
<page name="Page_1" type="PAGE">
  <children>
    <Frame name="home" width="320" height="480" type="FRAME">
      <children>
        <!-- widgets here -->
      </children>
    </Frame>
  </children>
</page>
```

Each top-level `<Frame>` inside `<children>` becomes one `ParsedScreen`. The parser entry point is `parse_screen(frame_node)`.

---

## Tree Walker

The parser recurses into the full Figma node hierarchy — not just direct children of the Frame. The recursive function is `_parse_children(parent_xml, screen_name, depth, seen_ids)`.

**At each level, for each child node:**

1. Call `detect_widget_type(node)` to determine the WidgetType
2. If `STRUCTURAL` → drop the frame, recurse into its children at the current level (promotion)
3. If `None` → skip with warning (includes screen name, node name, tag)
4. If depth > `MAX_DEPTH_HARD` (7) → skip subtree, log error
5. If depth > `MAX_DEPTH` (5) → continue but log warning
6. Build a `ParsedNode`, then:
   - `BUTTON` → one-level peek inside for label text, do NOT recurse further
   - `DYNAMIC` → create node, stop recursion
   - `PANEL` → create node, recurse into children
   - All others → leaf node, no recursion

---

## Widget Type Detection

`detect_widget_type(node)` in `core/utils/figma_helpers.py`. Returns a `WidgetType` enum value or `None`. Rules applied in priority order:

| Priority | Condition | Result |
|----------|-----------|--------|
| 1 | `node.tag == "Text"` | `LABEL` |
| 2 | `name.lower().startswith(("btn_", "button_"))` | `BUTTON` |
| 3 | `name.lower().startswith("slider_")` or `name.lower().endswith("_slider")` | `SLIDER` |
| 4 | `name.lower().startswith(("list_", "grid_"))` | `DYNAMIC` |
| 5 | `"bar" in name.lower()` | `BAR` |
| 6 | `"icon" in name.lower()` or `"image" in name.lower()` | `IMAGE` |
| 7 | `node.tag == "Vector"` | `None` (SVG path data — skip) |
| 8 | `type == "INSTANCE"` and contains Vector children | `IMAGE` (icon component) |
| 8 | `type == "INSTANCE"` and no Vector children | `None` (skip with warning) |
| 9 | Has children + has fill/border/radius OR has meaningful name | `PANEL` |
| 10 | Has children + no visual properties + auto-named ("Frame 12") | `STRUCTURAL` (drop+promote) |
| — | Leaf with no match | `None` (skip with warning) |

### Rule 8 — Icon component content detection

Figma icon components are always internally structured as a Frame or Instance wrapping one or more `Vector` paths. Example: a component named `Wifi_off` containing a `<Vector name="Icon">` child.

The heuristic: if a component `INSTANCE` contains `Vector` children (checked one-to-two levels deep), it is an icon component. The tool treats the whole instance as `IMAGE` and expects `<normalized_id>.png`.

```
Wifi_off (INSTANCE)          → detect: has Vector child "Icon" → IMAGE
  └── Vector "Icon"          → never reached as separate node
```

This means the designer does not need to rename design system icon components.

### Auto-named frame detection

`_is_auto_named(name)` returns `True` for Figma's auto-generated frame names (regex: `^(Frame|Group|Rectangle|Ellipse|...) \d+$`). These are structural grouping frames with no semantic meaning → `STRUCTURAL` → promoted children, frame itself discarded.

---

## Text Content Extraction

**Confirmed from real FigML exports:** Text content is in the `characters` **attribute** of `<Text>` nodes, not in `node.text` or a child element.

```xml
<Text name="time" characters="16:30" fontSize="14" ...>
```

Extraction:
```python
raw_text     = child_xml.attrib.get("characters", "")
text_content = sanitize_c_string(raw_text, UI_MAX_STRING_LENGTH)
```

`sanitize_c_string()` escapes: `\` → `\\`, `"` → `\"`, `\n` → `\n`, `\r` → `\r`, `\t` → `\t`, U+2028 → `\n`, U+2029 → `\n`. FigML sometimes embeds U+2028 (Unicode LINE SEPARATOR) in `characters` attributes instead of a standard newline.

**Text alignment note:** FigML does not export `textAlignHorizontal`. The attribute is absent from all Text nodes. LVGL default (left) is used; `has_align` is never set to `true`.

---

## Widget Name Parsing — Base Name vs Modifiers

Figma widget names encode two distinct things that the parser separates immediately, before any ID is formed:

1. **Identity** — becomes the C struct field name and all API function names
2. **Behavioral modifiers** — consumed by the generator to produce code variations (event registrations, value ranges)

The rule: **modifiers are stripped before `normalize_id()` is called**. They never appear in C struct names or API function names.

`parse_widget_name(raw_name)` in `figma_helpers.py` handles this split:

```python
# btn_ok_lp  → base "btn_ok",          modifiers ["lp"]
# btn_ok     → base "btn_ok",          modifiers []
# btn_ok_lpr → base "btn_ok",          modifiers ["lpr"]
```

The `event_modifiers` list is stored on `ParsedNode` and read by the init and setter emitters to generate the correct `lv_obj_add_event_cb` registrations and weak callback functions.

**Slider range numbers are also stripped from the struct ID** — separately from event modifiers, in `_parse_children()`:

```python
# brightness_slider_0_255 → struct field "brightness_slider"
#                           range (0, 255) stored in node.slider_min/max
```

---

## Button Label Style Extraction

`_find_button_label(button_xml)` reads the label text from the first `<Text>` child inside a button frame. A companion operation (in `_parse_children()`) also reads the Text child's fill color and font size and merges them into the button's `ParsedStyle.text` fields:

```python
# Text child fill color  → style.text.color
# Text child fontSize    → style.text.size
```

The same `ui_style_t` struct field on the button carries both the button body appearance (`style.box`) and the label text appearance (`style.text`). They are applied to different LVGL objects during init — see `05_code_generation.md`.

---

## Style Extraction

`parse_style(node, widget_type)` is called for every node. Returns a `ParsedStyle` with only the fields that were found in the XML populated.

**Fill color routing:**
- `node.tag == "Text"` OR `widget_type == WidgetType.LABEL` → fill color goes to `ParsedStyleText.color` (text color)
- All other widgets → fill color goes to `ParsedStyleBox.bg_color`

This is the critical distinction: a Text node's fill is its foreground (text) color in Figma, not a background color.

**Fill visibility:** Fills with `visible="false"` are skipped.

**Border default width:** If a border color is found but no `strokeWeight` attribute is present, `border_width` defaults to `1`.

---

## Slider Range

`_parse_slider_range(name)` extracts (min, max) from the node name:
- `brightness_slider_0_255` → (0, 255)
- `temp_slider_n20_50` → (-20, 50) — prefix `n` = negative
- No range in name → default (0, 100)

---

## ID Normalization

`normalize_id(name)` converts Figma node names to valid C identifiers:
1. Insert underscores at camelCase/PascalCase boundaries (`BatteryBar` → `Battery_Bar`)
2. Same for acronyms (`HTTPStatus` → `HTTP_Status`)
3. Lowercase
4. Replace spaces and hyphens with underscores
5. Replace remaining non-alphanumeric with underscores
6. Collapse multiple underscores
7. Strip leading/trailing underscores

Examples: `BatteryBar` → `battery_bar`, `Progress Bar` → `progress_bar`, `icon-wifi` → `icon_wifi`.

Duplicate IDs within the same level raise a warning and are disambiguated by appending the depth.

---

## Nesting Limits

| Threshold | Action |
|-----------|--------|
| depth > 5 | `[UI GEN WARN]` log warning with screen name and node name |
| depth > 7 | Log error, skip subtree entirely |
