# figma2lvgl — Data Model

Two parallel models exist: the **Python model** used during generation (in-process, discarded after output is written) and the **C model** that lives in the generated output and runs on the device.

---

## Python Model (Generator Side)

### `ParsedScreen`

Represents one Figma `<Frame>` — one screen.

| Field | Type | Description |
|-------|------|-------------|
| `name` | `str` | Raw Figma frame name (e.g. `"Home Screen"`) |
| `snake` | `str` | Snake-case version for C identifiers (e.g. `"home_screen"`) |
| `children` | `list[ParsedNode]` | Direct children (each may have their own children) |

**Methods:**
- `get_required_assets()` → `list[str]` — recursively collects IDs of all IMAGE nodes. Used by `main.py` to validate PNG files exist before running.
- `all_nodes_bfs()` → `list[tuple]` — BFS traversal yielding `(node, struct_path, parent_lv_obj_expr)` tuples. Used by the init emitter.

---

### `ParsedNode`

Represents one UI element in the Figma hierarchy. Forms a tree — `ParsedNode.children` contains nested `ParsedNode` objects for PANEL containers. Leaf widgets have empty children.

| Field | Type | Description |
|-------|------|-------------|
| `widget_type` | `WidgetType` | Enum value: LABEL, IMAGE, BAR, BUTTON, SLIDER, PANEL, DYNAMIC |
| `id` | `str` | Normalized Figma node name used as the C identifier |
| `raw_name` | `str` | Original Figma name before normalization |
| `x`, `y`, `w`, `h` | `int` | Position and size in pixels |
| `style` | `ParsedStyle` | Extracted style properties |
| `text_content` | `str` | For LABEL: text from `characters` attr (sanitized). For BUTTON: label text from first child Text node. Empty for all others. |
| `slider_min`, `slider_max` | `int` | Range for SLIDER (0–100 default, or from name convention) |
| `depth` | `int` | Nesting depth in the tree (0 = direct child of screen) |
| `children` | `list[ParsedNode]` | Child nodes (PANEL only; empty for all other types) |

**Property `is_dynamic_text`:** `True` → `char[]` in RAM, setter generated. `False` (BUTTON) → `const char *` in Flash, no setter.

---

### `ParsedStyle`

Composed of three sub-structs. Fields are `None` when the corresponding Figma property was absent or not exported.

#### `ParsedStyleBox`

| Field | Type | Maps to C |
|-------|------|-----------|
| `bg_color` | `int \| None` | `ui_style_box_t.has_bg` + `.bg` |
| `bg_opa` | `int \| None` | `ui_style_box_t.has_bg_opa` + `.bg_opa` |
| `border_color` | `int \| None` | `ui_style_box_t.has_border_color` + `.border_color` |
| `border_width` | `int \| None` | `ui_style_box_t.has_border_width` + `.border_width` |
| `radius` | `int \| None` | `ui_style_box_t.has_radius` + `.radius` |

#### `ParsedStyleText`

| Field | Type | Maps to C |
|-------|------|-----------|
| `color` | `int \| None` | `ui_style_text_t.has_color` + `.color` |
| `size` | `int \| None` | `ui_style_text_t.has_size` + `.size` |
| `align` | `str \| None` | `ui_style_text_t.has_align` + `.align` (always `None` — FigML doesn't export horizontal alignment) |

#### `ParsedStyleEffects`

| Field | Type | Maps to C |
|-------|------|-----------|
| `opacity` | `int \| None` | `ui_style_effects_t.has_opacity` + `.opacity` |

`ParsedStyle.is_empty()` returns `True` when all fields are `None`. The generator emits `.style = {0}` for empty styles.

---

## C Model (Device Side)

The generated C code does **not** use a generic `ui_child_t` array. Instead, each screen gets its own **typed, file-static struct** that mirrors the Figma node hierarchy.

### Screen-specific struct (generated per screen)

```c
/* ui_home.c — file-static, not exposed in .h */
static struct {

    lv_obj_t *lv_screen;

    struct {                         /* panel_top (PANEL) */
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {                     /* time (LABEL, dynamic) */
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } time;
        struct {                     /* icon_wifi (IMAGE) */
            lv_obj_t              *lv_obj;
            ui_style_t             style;
            const lv_image_dsc_t  *src;
        } icon_wifi;
    } panel_top;

    struct {                         /* btn_ok (BUTTON) */
        lv_obj_t      *lv_obj;
        ui_style_t     style;
        const char    *label_text;   /* Flash — static */
    } btn_ok;

    struct {                         /* brightness_slider (SLIDER) */
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t     value;
        int32_t     min;
        int32_t     max;
    } brightness_slider;

} s_home = {
    .panel_top = {
        .time = { .text = "16:30" },
    },
    .btn_ok            = { .label_text = "Ok" },
    .brightness_slider = { .value = 50, .min = 0, .max = 100 },
};
```

### Struct field rules per widget type

| Widget type | `lv_obj_t *lv_obj` | `ui_style_t style` | Additional fields |
|------------|--------------------|--------------------|------------------|
| LABEL (dynamic) | ✓ | ✓ | `char text[UI_MAX_STRING_LENGTH]` |
| IMAGE | ✓ | ✓ | `const lv_image_dsc_t *src` |
| BAR | ✓ | ✓ | `int32_t value` |
| BUTTON | ✓ | ✓ | `const char *label_text` |
| SLIDER | ✓ | ✓ | `int32_t value, min, max` |
| PANEL | ✓ | ✓ | child structs (nested) |
| DYNAMIC | ✓ | ✓ | (no data — firmware fills) |

`lv_obj` is `NULL` at compile time. `_init()` fills it at runtime.

---

## C Style Types (in `ui_defs.h`)

These are shared across all generated screens and remain in `ui_defs.h`.

```c
typedef enum {
    UI_CHILD_LABEL,
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
    UI_CHILD_BUTTON,
    UI_CHILD_SLIDER,
    UI_CHILD_PANEL,
    UI_CHILD_DYNAMIC,
} ui_child_type_t;
```

`ui_child_type_t` is passed to `ui_apply_style()` to apply type-specific style properties (e.g. text color only for LABEL/BUTTON, indicator color for BAR/SLIDER).

```c
typedef struct { bool has_bg; uint32_t bg; ... } ui_style_box_t;
typedef struct { bool has_color; uint32_t color; ... } ui_style_text_t;
typedef struct { bool has_opacity; uint8_t opacity; } ui_style_effects_t;
typedef struct { ui_style_box_t box; ui_style_text_t text; ui_style_effects_t effects; } ui_style_t;
```

Each property has a `has_*` boolean guard. Zero-initialised (`{0}`) safely means "no style applied."

---

## Constants (`ui_config.h` — auto-generated)

```c
#define UI_MAX_STRING_LENGTH 30
#define UI_MAX_ID_LENGTH     30
#define UI_MAX_ICON_STATES   8
```

`ui_config.h` is written by `config_writer.py` into `priv_include/` on every run. `ui_defs.h` includes it. There is no `UI_MAX_CHILDREN` — the screen struct is always exactly the right size for the design.
