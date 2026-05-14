# figma2lvgl — Widget Type System

The widget type system is the **primary extension point** of figma2lvgl. Adding a new LVGL widget type means:
1. Adding a value to `WidgetType` enum
2. Adding detection rules in `figma_helpers.py`
3. Adding struct field logic in `node_emitter.py`
4. Adding LVGL creation calls in `init_emitter.py`
5. Adding setter/callback logic in `setter_emitter.py`
6. Adding style handling in `static_src/ui_style.c`

No changes to `generator.py` or `figma_parser.py` are needed for a standard new widget type.

---

## `WidgetType` Enum (`core/widget_type.py`)

| Value | C enum | Description |
|-------|--------|-------------|
| `LABEL` | `UI_CHILD_LABEL` | Text label — `lv_label_create` |
| `IMAGE` | `UI_CHILD_IMAGE` | Image/icon — `lv_image_create` |
| `BAR` | `UI_CHILD_BAR` | Progress bar — `lv_bar_create` |
| `BUTTON` | `UI_CHILD_BUTTON` | Clickable button — `lv_button_create` |
| `SLIDER` | `UI_CHILD_SLIDER` | Value slider — `lv_slider_create` |
| `PANEL` | `UI_CHILD_PANEL` | Container frame — `lv_obj_create` |
| `DYNAMIC` | `UI_CHILD_DYNAMIC` | Runtime-filled container — `lv_obj_create` |
| `STRUCTURAL` | `"_STRUCTURAL"` | Internal sentinel — never in output |

`WidgetType.is_interactive` → True for BUTTON and SLIDER (these get weak callbacks).
`WidgetType.is_container` → True for PANEL and DYNAMIC.
`.c_enum_name()` → returns the C enum string for use in generated code.

---

## Widget Reference

### LABEL

```
Figma:      Text node (any name)
LVGL:       lv_label_create(parent)
            lv_label_set_long_mode(LV_LABEL_LONG_CLIP)
            lv_label_set_text(initial text from Figma)
Struct:     char text[UI_MAX_STRING_LENGTH]  ← RAM, setter generated
Setter:     ui_{screen}_{path}_set_text(const char *text)
Style:      text color, font size applied via ui_apply_style()
```

Initial text is read from the `characters` attribute in FigML. Firmware can update at runtime via the setter. On screen reload (`_init()` re-called), the label reverts to the Figma value.

---

### IMAGE

```
Figma:      Any node with "icon"/"image" in name
            OR: component INSTANCE with Vector children (icon component)
LVGL:       lv_image_create(parent)
            lv_obj_set_size(w, h)
Struct:     const lv_image_dsc_t *src  ← NULL until setter called
Setter:     ui_{screen}_{path}_display(void)
Asset:      <normalized_id>.png required in images folder
```

The setter assigns the image source array (declared in `assets.h`) and calls `lv_image_set_src()`. `lv_obj_set_size` ensures the LVGL object matches the Figma geometry regardless of the PNG's native pixel dimensions.

---

### BAR

```
Figma:      Rectangle/Frame with "bar" in name
LVGL:       lv_bar_create(parent)
            lv_bar_set_range(0, 100)  ← range configurable via name
            lv_bar_set_value(initial: 0)
Struct:     int32_t value
Setter:     ui_{screen}_{path}_set_value(int value, uint32_t duration_ms)
Style:      fill color applied to LV_PART_MAIN and LV_PART_INDICATOR
```

`duration_ms == 0` → instant update. Non-zero → LVGL animation via `_bar_anim_exec_cb`.
Range from name: `battery_bar_0_100` → (0, 100), `temp_bar_n20_50` → (-20, 50).
A `/* TODO: adjust range */` comment is emitted above `_init()` listing all bar IDs.

---

### BUTTON

```
Figma:      Frame named btn_* or button_*
            Optional: first Text child provides label text
LVGL:       lv_button_create(parent)
            lv_label_create(button) ← internal, not a separate ParsedNode
            lv_obj_add_event_cb(LV_EVENT_CLICKED)
Struct:     const char *label_text  ← Flash (static)
Setter:     ui_{screen}_{path}_{id}_set_label(const char *text)  (if needed)
Callback:   ui_{screen}_on_{id}(lv_event_t *e)  __attribute__((weak))
```

The button's internal label is not a separate `ParsedNode` — it's an LVGL implementation detail created inside the button init case. The setter accesses it via `lv_obj_get_child(btn.lv_obj, 0)`.

Override the callback in application code:
```c
void ui_home_on_btn_ok(lv_event_t *e) {
    ui_settings_load();   /* navigate to settings */
}
```

---

### SLIDER

```
Figma:      Rectangle/Frame named slider_* or *_slider
LVGL:       lv_slider_create(parent)
            lv_slider_set_range(min, max)
            lv_slider_set_value(initial: 0)
            lv_obj_add_event_cb(LV_EVENT_VALUE_CHANGED)
Struct:     int32_t value, min, max
Setter:     ui_{screen}_{path}_{id}_set_value(int32_t value)
Callback:   ui_{screen}_on_{id}(lv_event_t *e)  __attribute__((weak))
```

Range from name (same convention as bar): `brightness_slider_0_255` → (0, 255).

---

### PANEL

```
Figma:      Frame with fill/border/radius OR meaningful name
LVGL:       lv_obj_create(parent)
            LV_OBJ_FLAG_CLICKABLE cleared
            LV_SCROLLBAR_MODE_OFF
Struct:     lv_obj_t *lv_obj + ui_style_t style + nested child structs
No setter, no callback
```

Panel children are nested inside the panel's struct block and created under the panel's `lv_obj` in `_init()`.

---

### DYNAMIC

```
Figma:      Frame named list_* or grid_*
LVGL:       lv_obj_create(parent)  ← container only, firmware fills
            LV_SCROLLBAR_MODE_AUTO
Generator:  STOPS recursion here — no children parsed
Accessor:   ui_{screen}_get_{id}(void) → lv_obj_t *
```

Firmware fills the container at runtime:
```c
lv_obj_t *list = ui_home_get_list_devices();
for (int i = 0; i < n; i++) {
    lv_obj_t *item = lv_obj_create(list);
    /* populate item */
}
```

---

## Adding a New Widget Type — Step-by-Step

Example: adding `UI_CHILD_ARC`.

**Step 1 — `core/widget_type.py`:** Add `ARC = "UI_CHILD_ARC"` to the enum.

**Step 2 — `core/utils/figma_helpers.py`:** Add detection rule before the container analysis:
```python
if "arc" in name_lower or name_lower.startswith("arc_"):
    return WidgetType.ARC
```

**Step 3 — `core/node_emitter.py`:** Add a branch in `emit_struct_fields()` for the new type's data fields:
```python
elif wt == WidgetType.ARC:
    lines.append(f"{indent}int32_t value;")
    lines.append(f"{indent}int32_t min;")
    lines.append(f"{indent}int32_t max;")
```
And in `emit_node_initializer()`:
```python
elif wt == WidgetType.ARC:
    lines.append(f"{indent}.value = 0,")
    lines.append(f"{indent}.min = 0,")
    lines.append(f"{indent}.max = 100,")
```

**Step 4 — `core/init_emitter.py`:** Add a branch in `_emit_node_init()`:
```python
elif wt == WidgetType.ARC:
    lines += [
        f"    {path}.lv_obj = lv_arc_create({parent_lv});",
        f"    lv_obj_set_pos({path}.lv_obj, {node.x}, {node.y});",
        f"    lv_obj_set_size({path}.lv_obj, {node.w}, {node.h});",
        f"    lv_arc_set_range({path}.lv_obj, {path}.min, {path}.max);",
        f"    lv_arc_set_value({path}.lv_obj, {path}.value);",
        f"    ui_apply_style({path}.lv_obj, UI_CHILD_ARC, &{path}.style);",
    ]
```

**Step 5 — `core/setter_emitter.py`:** Add setter emission in `collect_setters_and_callbacks()`:
```python
elif wt == WidgetType.ARC:
    body, proto = _emit_arc_setter(fn_base, path)
    setters.append(body)
    prototypes.append(proto)
```

**Step 6 — `static_src/ui_defs.h`:** Add `UI_CHILD_ARC` to the `ui_child_type_t` enum.

**Step 7 — `static_src/ui_style.c`:** Add a case in `ui_apply_style()` if the arc needs special style handling.

**Done.** `generator.py` and `figma_parser.py` require zero changes for a standard new widget type.
