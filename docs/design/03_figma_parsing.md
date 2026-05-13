# figma2lvgl — Figma Parsing Rules

All parsing logic lives in `core/figma_parser.py` and `core/utils/figma_helpers.py`. The parser reads Figma XML exported by the **FigML** plugin and produces `ParsedScreen` / `ParsedChild` / `ParsedStyle` objects. It has no knowledge of C code or templates.

---

## XML Structure (Real FigML Format)

FigML exports a `<page>` root, not `<root>`. Each `<Frame>` child becomes one screen. Real nodes include noise attributes (`id`, `maskType`, `type`, `blendMode`) that the parser reads through safely.

```xml
<page id="0:1" name="Page_1" type="PAGE">
  <children>
    <Frame id="6:3" name="ili9486_home" maskType="ALPHA"
           x="-170" y="-287" width="320" height="480"
           clipsContent="true" type="FRAME">
      <children>
        <Text id="6:4" name="Time" maskType="ALPHA" strokeAlign="OUTSIDE"
              x="94" y="79" width="133" height="34" fontSize="12"
              fontWeight="400" characters="Time is 15:00" type="TEXT"
              textAlignVertical="TOP">
          <fills>
            <fill blendMode="NORMAL" color="#000000" />
          </fills>
          <fontName family="Inter" style="Regular" />
        </Text>
        <Rectangle id="21:4" name="bar" maskType="ALPHA"
                   x="33" y="311" width="241" height="35" type="RECTANGLE">
          <fills>
            <fill blendMode="NORMAL" color="#e56060" />
          </fills>
        </Rectangle>
        <icon_wifi id="12:9" name="icon_wifi" maskType="ALPHA"
                   x="129" y="143" width="48" height="48" type="INSTANCE">
          <fills>
            <fill visible="false" blendMode="NORMAL" color="#ffffff" />
          </fills>
        </icon_wifi>
      </children>
    </Frame>
  </children>
</page>
```

Key observations from the real format:
- Root tag is `page`, not `root`. `root.find("children")` still works.
- Text content is in the `characters` **attribute** — not `node.text` (which is whitespace) and not a `<characters>` child element.
- `textAlignHorizontal` is **absent** — FigML omits it when alignment is default (left).
- Component instances (type `INSTANCE`) use their component name as the XML tag (e.g. `<icon_wifi>`, `<Wifi_off>`).
- Nested children are **not** traversed — only direct children of the `<Frame>` are parsed.

---

## Screen Parsing

`parse_screen(frame_node)` is called once per `<Frame>` node.

1. Reads `name` attribute → `ParsedScreen.name`
2. Derives `ParsedScreen.snake` via `to_snake_case(name)`
3. Iterates direct children of `<children>`
4. For each child: type detection → geometry → ID normalization → style extraction → text content
5. Emits warning and skips unrecognized nodes (rather than silently converting them to labels)
6. Detects duplicate normalized IDs — raises `ValueError`

---

## Widget Type Detection

`map_tag_to_child_type(node)` in `core/utils/figma_helpers.py` returns a `WidgetType` enum value or `None`.

| Priority | Condition | Result |
|----------|-----------|--------|
| 1 | XML tag is `Text` | `WidgetType.LABEL` |
| 2 | Node name contains `bar` (case-insensitive) | `WidgetType.BAR` |
| 3 | Node name contains `icon` or `image` (case-insensitive) | `WidgetType.IMAGE` |
| 4 | (no match) | `None` → skip with warning |

**On `None`:** `parse_screen()` emits a `WARNING` log with the parent screen name and the node name/tag/type, then skips the node. The warning message tells the user exactly how to rename the node in Figma to get it recognized.

Example: `<Wifi_off name="Wifi_off" type="INSTANCE">` matches none of the rules. The warning says:
```
In screen 'ili9486_home': skipping node 'Wifi_off' (tag='Wifi_off', type='INSTANCE').
To generate a widget from this node, rename it in Figma to include one of:
'icon' or 'image' (-> lv_image), 'bar' (-> lv_bar).
Example: rename 'Wifi_off' -> 'icon_wifi_off'.
```

---

## ID Normalization

`normalize_id(name)` converts a Figma node name to a valid C snake_case identifier:

1. Split camelCase/PascalCase boundaries (e.g. `Battery` | `Bar`)
2. Split before consecutive uppercase sequences (e.g. `HTTP` | `Status`)
3. Lowercase
4. Replace spaces and hyphens with `_`
5. Replace remaining non-alnum with `_`
6. Collapse multiple underscores
7. Strip leading/trailing underscores

| Figma name | Normalized ID |
|-----------|--------------|
| `time_label` | `time_label` |
| `Progress Bar` | `progress_bar` |
| `icon-wifi` | `icon_wifi` |
| `BatteryBar` | `battery_bar` |
| `HTTPStatus` | `http_status` |
| `Time (Label)` | `time_label` |

The normalized ID is used as:
- The `id[]` string in `ui_child_t`
- The suffix in generated setter/callback function names

---

## Geometry Extraction

`int_attr(node, key)` reads an XML attribute as `int`. Returns `0` if absent or malformed.

Fields read: `x`, `y`, `width`, `height`.

---

## Text Content Extraction

For `Text` nodes, text content is read from the `characters` attribute:

```python
raw_text = child.attrib.get("characters", "")
text_content = sanitize_c_string(raw_text, UI_MAX_STRING_LENGTH)
```

`sanitize_c_string()` escapes characters that would produce invalid C string literals:

| Character | Escaped as |
|-----------|-----------|
| `\` | `\\` |
| `"` | `\"` |
| `\n` (0x0A) | `\n` |
| `\r` (0x0D) | `\r` |
| `\t` (0x09) | `\t` |
| U+2028 (LINE SEPARATOR) | `\n` — FigML sometimes uses this instead of `\n` |
| U+2029 (PARAGRAPH SEPARATOR) | `\n` |

The result is truncated to `UI_MAX_STRING_LENGTH - 1` characters (leaving room for the null terminator). Silent truncation — no warning is emitted.

---

## Style Extraction

`parse_style(node, child_type)` is called for every child node. `child_type` is a `WidgetType` enum value.

### Fill Color and Opacity

1. Finds the first `<fill>` under `<fills>` where `visible != "false"`
2. Reads `color` attribute — hex string → integer
3. **Routing:** if `node.attrib["type"] == "TEXT"` or `child_type == WidgetType.LABEL`, fill color routes to `ParsedStyleText.color` (text color). Otherwise it routes to `ParsedStyleBox.bg_color`. This comparison uses the `WidgetType` enum — comparing against the string `"UI_CHILD_LABEL"` would always be `False` and silently route every label's fill to bg_color.
4. `opacity` attribute on the fill element → `ParsedStyleBox.bg_opa` (non-text only)

### Border / Stroke

1. First `<stroke>` under `<strokes>` → `ParsedStyleBox.border_color`
2. `strokeWeight` attribute on the node → `ParsedStyleBox.border_width`
3. If color set but no width: defaults to `1`

### Corner Radius

`cornerRadius` attribute → `ParsedStyleBox.radius`

### Text Properties

For text nodes (`node_type == "TEXT"` or `child_type == WidgetType.LABEL`):

- `fontSize` attribute → `ParsedStyleText.size`
- Horizontal text alignment: **not extracted** — the `textAlignHorizontal` attribute does not exist in FigML exports. LVGL's default (left) matches FigML's default. `ParsedStyleText.align` is always `None`.

### Whole-Widget Opacity

`opacity` attribute on the node → `ParsedStyleEffects.opacity`

---

## Empty Style Handling

`ParsedStyle.is_empty()` is `True` when all fields across all three sub-structs are `None`. The generator emits `{ .box = { 0 }, .text = { 0 }, .effects = { 0 } }` rather than an explicit initialiser.

---

## Naming Conventions Summary

| Figma element | Requirement | Maps to |
|--------------|-------------|---------|
| Frame | Any name | Screen (`to_snake_case` → C identifier base) |
| `Text` node | XML tag must be `Text` | `WidgetType.LABEL` |
| Any node | Name must contain `bar` | `WidgetType.BAR` |
| Any node | Name must contain `icon` or `image` | `WidgetType.IMAGE` |
| Everything else | — | Skipped with warning |
