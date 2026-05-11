# figma2lvgl — Figma Parsing Rules

All parsing logic lives in `core/figma_parser.py` and `core/utils/figma_helpers.py`. The parser reads Figma XML exported by the **FigML** plugin and produces `ParsedScreen` / `ParsedChild` / `ParsedStyle` objects. It has no knowledge of C code or templates.

---

## XML Structure Expected

The FigML plugin exports a structure like:

```xml
<root>
  <children>
    <Frame name="HomeScreen" width="320" height="480" ...>
      <children>
        <Text name="time_label" x="10" y="20" width="100" height="30"
              fontSize="16" textAlignHorizontal="CENTER"
              ...>
          <fills>
            <fill color="#ffffff" opacity="1.0" />
          </fills>
        </Text>
        <Rectangle name="progress_bar" x="10" y="100" width="200" height="20"
                   cornerRadius="4" strokeWeight="1" ...>
          <fills>
            <fill color="#4caf50" visible="true" />
          </fills>
          <strokes>
            <stroke color="#333333" />
          </strokes>
        </Rectangle>
        ...
      </children>
    </Frame>
    <Frame name="StatusScreen" ...>
      ...
    </Frame>
  </children>
</root>
```

- Each top-level `<Frame>` inside `<children>` becomes one `ParsedScreen`.
- Each child node inside a frame's `<children>` is parsed as a `ParsedChild`.
- Nested children (children of children) are **not** traversed — only direct children of the frame are parsed.

---

## Screen Parsing

`parse_screen(frame_node)` is called once per `<Frame>` node.

1. Reads `name` attribute from the frame → stored as `ParsedScreen.name`
2. Derives `ParsedScreen.snake` via `to_snake_case(name)` — this becomes the C identifier base (e.g. `"Home Screen"` → `"home_screen"`)
3. Iterates direct children of `<children>`
4. For each child: runs type detection, reads geometry, normalizes ID, extracts style
5. Detects duplicate IDs — raises `ValueError` if two children in the same frame produce the same normalized ID

---

## Widget Type Detection

Handled by `map_tag_to_child_type(node)` in `core/utils/figma_helpers.py`.

Rules applied **in order**:

| Priority | Condition | Result |
|----------|-----------|--------|
| 1 | XML tag is `Text` | `UI_CHILD_LABEL` |
| 2 | Node name contains `bar` (case-insensitive) | `UI_CHILD_BAR` |
| 3 | Node name contains `icon` or `image` (case-insensitive) | `UI_CHILD_IMAGE` |
| 4 | (fallback) | `UI_CHILD_LABEL` |

> **Important:** The fallback maps unknown nodes to `UI_CHILD_LABEL`. Nodes that don't match any explicit rule are silently treated as labels rather than skipped. This can produce unexpected output if a frame contains non-UI structural nodes (e.g. grouping frames).

A return value of `None` from `map_tag_to_child_type()` causes the child to be skipped. Currently no code path returns `None` — the fallback always returns `UI_CHILD_LABEL`.

---

## ID Normalization

The `name` attribute of the Figma node becomes the widget's `id` in generated C code.

`normalize_id(name)` applies:
1. Lowercase the string
2. Replace `-` with `_`
3. Replace spaces with `_`

Examples:

| Figma name | Normalized ID |
|-----------|--------------|
| `time_label` | `time_label` |
| `Progress Bar` | `progress_bar` |
| `icon-wifi` | `icon_wifi` |
| `BatteryBar` | `batterybar` |

The normalized ID is used as:
- The `id[]` string in `ui_child_t`
- The suffix in generated setter function names (e.g. `ui_home_screen_set_time_label`)

---

## Geometry Extraction

`int_attr(node, key)` reads a named XML attribute and converts it to `int`. Returns `0` if the attribute is absent or malformed (no error raised).

Fields read: `x`, `y`, `width`, `height`.

---

## Style Extraction

`parse_style(node, child_type)` is called for every child node. It returns a `ParsedStyle` with only the fields that were actually found in the XML populated. Fields with no data remain `None`.

### Fill Color and Opacity

1. Finds the first `<fill>` under `<fills>` where `visible != "false"` (skips hidden fills)
2. Reads the `color` attribute — hex string like `"#d9d9d9"` → stripped and parsed as integer
3. **Context-aware:** if the node is a `Text` node or maps to `UI_CHILD_LABEL`, fill color goes to `ParsedStyleText.color` (text color), not `ParsedStyleBox.bg_color`
4. Reads `opacity` attribute on the fill element → converted to 0–255 range (`float * 255`, rounded). Only applies to non-text nodes as `bg_opa`.

### Border / Stroke

1. Reads the first `<stroke>` under `<strokes>` — reads `color` attribute → `ParsedStyleBox.border_color`
2. Reads `strokeWeight` attribute on the node itself → `ParsedStyleBox.border_width`
3. **Default width rule:** if a border color is found but no `strokeWeight` attribute is present, `border_width` defaults to `1` so the border is visible in LVGL

### Corner Radius

Reads `cornerRadius` attribute on the node → `ParsedStyleBox.radius` (rounded float → int)

### Text Properties

Only extracted when `node_type == "TEXT"` or `child_type == "UI_CHILD_LABEL"`:

- `fontSize` attribute → `ParsedStyleText.size`
- `textAlignHorizontal` attribute — accepted values: `"LEFT"`, `"CENTER"`, `"RIGHT"` → `ParsedStyleText.align`. Any other value is ignored.

### Whole-Widget Opacity

`opacity` attribute on the node itself (not the fill) → `ParsedStyleEffects.opacity` (float * 255, rounded). Applies to all widget types.

---

## Empty Style Handling

After extraction, `ParsedStyle.is_empty()` checks whether all fields across all three sub-structs are `None`. If true, the generator emits `{ .box = { 0 }, .text = { 0 }, .effects = { 0 } }` for the style field rather than an explicit struct initialiser.

---

## ID Uniqueness Rule

Duplicate IDs within a single screen are detected immediately during parsing:

```python
existing_ids = {c.id for c in screen.children}
if child_id in existing_ids:
    raise ValueError(f"Duplicate child id '{child_id}' in screen '{frame_name}'")
```

This catches cases where two Figma nodes normalize to the same ID (e.g. nodes named `"Time Label"` and `"time_label"` would both normalize to `"time_label"`).

---

## Naming Conventions Summary

| Figma element | Naming requirement | Maps to |
|--------------|-------------------|---------|
| Frame | Any name | Screen (name → snake_case) |
| Text node | Any name | `UI_CHILD_LABEL` |
| Any node with `bar` in name | Must contain `bar` (case-insensitive) | `UI_CHILD_BAR` |
| Any node with `icon`/`image` in name | Must contain `icon` or `image` | `UI_CHILD_IMAGE` |
| Everything else | Any name | `UI_CHILD_LABEL` (fallback) |
