# figma2lvgl — Data Model

Two parallel data models exist: the **Python model** used during generation and the **C model** that runs on the device.

---

## Python Model (Generator Side)

### `ParsedScreen`

Represents one Figma `<Frame>` — one full screen.

| Field | Type | Description |
|-------|------|-------------|
| `name` | `str` | Raw Figma frame name (e.g. `"ili9486_home"`) |
| `snake` | `str` | Snake-case version used in C identifiers (`to_snake_case(name)`) |
| `children` | `list[ParsedChild]` | Ordered list of UI children parsed from the frame |

**Key method:** `get_required_assets(child_registry)` — returns IDs for children whose `ChildSpec.requires_asset` is `True`. Used by `main.py` to validate PNG files exist.

---

### `ParsedChild`

Represents one UI element within a screen.

| Field | Type | Description |
|-------|------|-------------|
| `type` | `WidgetType` | Enum value (e.g. `WidgetType.LABEL`) |
| `id` | `str` | Normalized Figma node name used as C identifier |
| `x` | `int` | X position in pixels |
| `y` | `int` | Y position in pixels |
| `w` | `int` | Width in pixels |
| `h` | `int` | Height in pixels |
| `style` | `ParsedStyle` | Extracted style properties |
| `text_content` | `str` | Design-time label text from Figma `characters` attribute. Non-empty for `LABEL` nodes only. Sanitized via `sanitize_c_string()`. |

`text_content` lifecycle: baked into `.data.label.text` in the C struct initialiser → `_init()` applies it via `lv_label_set_text()` on first render → firmware calls the setter to update at runtime. The struct field holds the design-time default; it is not updated by the setter.

IDs are normalized via `normalize_id()`: camelCase/PascalCase boundaries split, spaces/hyphens/non-alnum replaced with underscores. Duplicate IDs within a screen raise `ValueError`.

---

### `ParsedStyle`

Holds three style sub-groups. `is_empty()` returns `True` when all fields are `None`; the generator emits `{ .box = {0}, .text = {0}, .effects = {0} }`.

#### `ParsedStyleBox`

| Field | Type | Description |
|-------|------|-------------|
| `bg_color` | `int \| None` | Background color (e.g. `0xd9d9d9`) |
| `bg_opa` | `int \| None` | Background opacity, 0–255 |
| `border_color` | `int \| None` | Border/stroke color |
| `border_width` | `int \| None` | Border width in pixels (defaults to 1 if color set but width absent) |
| `radius` | `int \| None` | Corner radius in pixels |

#### `ParsedStyleText`

| Field | Type | Description |
|-------|------|-------------|
| `color` | `int \| None` | Text color |
| `size` | `int \| None` | Font size in points |
| `align` | `str \| None` | `"LEFT"`, `"CENTER"`, or `"RIGHT"` — not populated currently (FigML does not export horizontal text alignment) |

#### `ParsedStyleEffects`

| Field | Type | Description |
|-------|------|-------------|
| `opacity` | `int \| None` | Whole-widget opacity, 0–255 |

---

### `WidgetType`

Python `Enum` in `core/widget_type.py`. Used as keys in `CHILDREN` registry and stored in `ParsedChild.type`.

| Member | C value |
|--------|---------|
| `WidgetType.LABEL` | `"UI_CHILD_LABEL"` |
| `WidgetType.IMAGE` | `"UI_CHILD_IMAGE"` |
| `WidgetType.BAR` | `"UI_CHILD_BAR"` |

`c_enum_name()` returns the string value for embedding in generated C code.

---

### `ChildSpec`

Describes how the generator handles one widget type. Stored in `CHILDREN` in `child_registry.py`.

| Field | Type | Description |
|-------|------|-------------|
| `type_name` | `WidgetType` | Enum value (matches the key in `CHILDREN`) |
| `callback_template` | `str` | Template name for animation callback; `""` = none |
| `setter_template` | `str` | Template name for the public setter |
| `init_template` | `str` | Template name for the `switch` init case |
| `setter_args` | `str` | C argument list for the setter |
| `requires_asset` | `bool` | If `True`, main.py validates a matching PNG exists |
| `setter_name_pattern` | `str` | Pattern for setter function name, e.g. `"ui_{screen}_set_{child_id}"` |
| `callback_name_pattern` | `str` | Pattern for callback function name; `""` = no callback |

`derive_setter_name(screen_snake, child_id)` and `derive_callback_name(screen_snake)` format these patterns. This is why `generator.py` has no per-type `if/elif` branches for naming — all naming is driven by the registry.

---

## C Model (Device Side)

Structs defined in `static_src/ui_defs.h`. Constants defined in the generated `ui_config.h` (included by `ui_defs.h`).

---

### `ui_child_type_t` — Widget Type Enum

```c
typedef enum {
    UI_CHILD_LABEL,
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
} ui_child_type_t;
```

---

### Style Structs

```c
typedef struct {
    bool        has_bg;          uint32_t    bg;
    bool        has_bg_opa;      uint8_t     bg_opa;
    bool        has_border_color; uint32_t   border_color;
    bool        has_border_width; lv_coord_t border_width;
    bool        has_radius;      lv_coord_t  radius;
} ui_style_box_t;

typedef struct {
    bool            has_color;  uint32_t        color;
    bool            has_size;   uint16_t        size;
    bool            has_align;  lv_text_align_t align;
} ui_style_text_t;

typedef struct {
    bool    has_opacity;
    uint8_t opacity;
} ui_style_effects_t;

typedef struct {
    ui_style_box_t      box;
    ui_style_text_t     text;
    ui_style_effects_t  effects;
} ui_style_t;
```

Each property has a `has_*` boolean guard — `ui_apply_style()` only calls the LVGL API if the guard is `true`. Zero-initialised structs mean "no style set".

---

### `ui_child_t` — UI Element

```c
typedef struct {
    ui_child_type_t  type;
    char             id[UI_MAX_ID_LENGTH];
    lv_obj_t        *lv_obj;       // NULL until _init() runs
    int              x, y, w, h;
    ui_style_t       style;
    union {
        struct { char text[UI_MAX_STRING_LENGTH]; } label;
        struct { int32_t value; }                  bar;
        struct { const lv_image_dsc_t *src; }      image;
    } data;
} ui_child_t;
```

`lv_obj` is `NULL` in the static initialiser. `_init()` creates the LVGL objects and fills these pointers. All setters guard against `lv_obj == NULL`.

`data.label.text` holds the design-time default from Figma. `_init()` applies it via `lv_label_set_text()`. The setter writes directly to the live LVGL object and does not update the struct field.

---

### `ui_screen_t` — Screen

```c
typedef struct {
    const char  *name;
    ui_child_t   children[UI_MAX_CHILDREN];  // auto-sized in ui_config.h
    uint8_t      child_count;
    lv_obj_t    *lv_screen;     // NULL until _init() runs
} ui_screen_t;
```

---

### `ui_config.h` — Generated Constants

`ui_config.h` is **auto-generated** by `config_writer.py` on every run. It defines all numeric constants so the C model always matches the actual design:

```c
/* Auto-generated by figma2lvgl — do not edit */
/* Largest screen: ili9486_home (4 children) */
#define UI_MAX_CHILDREN      4
#define UI_MAX_STRING_LENGTH 30
#define UI_MAX_ID_LENGTH     30
#define UI_MAX_ICON_STATES   8
```

`ui_defs.h` includes this header — no constants are hardcoded in `ui_defs.h` itself.

> **Build system note:** `ui_style.c` (in `priv_src/`) includes `ui_defs.h`, which now requires `priv_include/` on the include path. ESP-IDF and Zephyr handle this automatically. For bare-metal Makefiles, add `-Iui_src/priv_include` to the CFLAGS that compile `priv_src/`.

---

## Python → C Model Mapping

| Python field | C field |
|-------------|---------|
| `ParsedChild.type` | `ui_child_t.type` (via `.c_enum_name()`) |
| `ParsedChild.id` | `ui_child_t.id[]` |
| `ParsedChild.x/y/w/h` | `ui_child_t.x/y/w/h` |
| `ParsedChild.text_content` | `ui_child_t.data.label.text[]` (LABEL only) |
| `ParsedStyleBox.bg_color` | `ui_style_box_t.has_bg` + `.bg` |
| `ParsedStyleBox.bg_opa` | `ui_style_box_t.has_bg_opa` + `.bg_opa` |
| `ParsedStyleBox.border_color` | `ui_style_box_t.has_border_color` + `.border_color` |
| `ParsedStyleBox.border_width` | `ui_style_box_t.has_border_width` + `.border_width` |
| `ParsedStyleBox.radius` | `ui_style_box_t.has_radius` + `.radius` |
| `ParsedStyleText.color` | `ui_style_text_t.has_color` + `.color` |
| `ParsedStyleText.size` | `ui_style_text_t.has_size` + `.size` |
| `ParsedStyleText.align` | `ui_style_text_t.has_align` + `.align` |
| `ParsedStyleEffects.opacity` | `ui_style_effects_t.has_opacity` + `.opacity` |
| `ParsedScreen.name` | `ui_screen_t.name` |
| `len(ParsedScreen.children)` | `ui_screen_t.child_count` |
| `max(len(s.children))` | `UI_MAX_CHILDREN` in `ui_config.h` |
