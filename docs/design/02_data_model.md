# figma2lvgl — Data Model

Two parallel data models exist: the **Python model** used during generation (in-process, discarded after output is written) and the **C model** that lives in the generated output and runs on the device.

---

## Python Model (Generator Side)

### `ParsedScreen`

Represents one Figma `<Frame>` — one full screen.

| Field | Type | Description |
|-------|------|-------------|
| `name` | `str` | Raw Figma frame name (e.g. `"Home Screen"`) |
| `snake` | `str` | Snake-case version used in C identifiers (e.g. `"home_screen"`) |
| `children` | `list[ParsedChild]` | Ordered list of UI children parsed from the frame |

**Key method:** `get_required_assets(child_registry)` — walks children and returns a list of IDs for any child whose `ChildSpec.requires_asset` is `True`. Used by `main.py` to validate PNG files exist before running the pipeline.

---

### `ParsedChild`

Represents one UI element within a screen.

| Field | Type | Description |
|-------|------|-------------|
| `type` | `str` | `UI_CHILD_*` string constant (e.g. `"UI_CHILD_LABEL"`) |
| `id` | `str` | Normalized Figma node name used as the C identifier |
| `x` | `int` | X position in pixels |
| `y` | `int` | Y position in pixels |
| `w` | `int` | Width in pixels |
| `h` | `int` | Height in pixels |
| `style` | `ParsedStyle` | Extracted style properties |

IDs are normalized via `normalize_id()`: lowercased, spaces and hyphens replaced with underscores. Duplicate IDs within a screen raise `ValueError` at parse time.

---

### `ParsedStyle`

A composite holding three style sub-groups. A `ParsedStyle` where all fields are `None` is considered empty — `is_empty()` returns `True` and the generator emits `{ 0 }` for the struct.

#### `ParsedStyleBox` — box / background properties

| Field | Type | Description |
|-------|------|-------------|
| `bg_color` | `int \| None` | Background color as integer (e.g. `0xd9d9d9`) |
| `bg_opa` | `int \| None` | Background opacity, 0–255 |
| `border_color` | `int \| None` | Border/stroke color as integer |
| `border_width` | `int \| None` | Border width in pixels (defaults to 1 if color present but width absent) |
| `radius` | `int \| None` | Corner radius in pixels |

#### `ParsedStyleText` — text / label properties

| Field | Type | Description |
|-------|------|-------------|
| `color` | `int \| None` | Text color as integer |
| `size` | `int \| None` | Font size in points |
| `align` | `str \| None` | `"LEFT"`, `"CENTER"`, or `"RIGHT"` |

#### `ParsedStyleEffects` — widget-level effects

| Field | Type | Description |
|-------|------|-------------|
| `opacity` | `int \| None` | Whole-widget opacity, 0–255 |

---

### `ChildSpec`

Describes how the generator handles one widget type. Stored in `CHILDREN` registry in `child_registry.py`.

| Field | Type | Description |
|-------|------|-------------|
| `type_name` | `str` | `UI_CHILD_*` string (matches the key in `CHILDREN`) |
| `callback_template` | `str` | Template name for animation callback. Empty string = no callback generated. |
| `setter_template` | `str` | Template name for the public setter function |
| `init_template` | `str` | Template name for the `switch` init case |
| `setter_args` | `str` | C argument list for the setter (e.g. `"const char *text"`) |
| `requires_asset` | `bool` | If `True`, main.py checks that a PNG matching the child ID exists |

---

## C Model (Device Side)

Defined in `static_src/ui_defs.h`. These structs are what the generated `.c` files populate as static initialisers, and what the device firmware reads at runtime.

---

### `ui_child_type_t` — Widget Type Enum

```c
typedef enum {
    UI_CHILD_ICON,
    UI_CHILD_LABEL,
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
} ui_child_type_t;
```

> Note: `UI_CHILD_ICON` is defined in the enum but has no corresponding `ChildSpec` in the current registry. It is a placeholder for future use.

---

### `ui_style_box_t` — Box Style

```c
typedef struct {
    bool        has_bg;
    uint32_t    bg;              // raw hex e.g. 0xFFFFFF
    bool        has_bg_opa;
    uint8_t     bg_opa;
    bool        has_border_color;
    uint32_t    border_color;
    bool        has_border_width;
    lv_coord_t  border_width;
    bool        has_radius;
    lv_coord_t  radius;
} ui_style_box_t;
```

Each property has a `has_*` boolean guard. `ui_apply_style()` only calls the corresponding LVGL API if the guard is `true`. This lets zero-initialised structs safely mean "no style set".

---

### `ui_style_text_t` — Text Style

```c
typedef struct {
    bool            has_color;
    uint32_t        color;
    bool            has_size;
    uint16_t        size;
    bool            has_align;
    lv_text_align_t align;
} ui_style_text_t;
```

---

### `ui_style_effects_t` — Effects

```c
typedef struct {
    bool    has_opacity;
    uint8_t opacity;
} ui_style_effects_t;
```

---

### `ui_style_t` — Composite Style

```c
typedef struct {
    ui_style_box_t      box;
    ui_style_text_t     text;
    ui_style_effects_t  effects;
} ui_style_t;
```

This is the type of the `style` field in `ui_child_t`.

---

### `ui_child_t` — UI Element

```c
typedef struct {
    ui_child_type_t  type;
    char             id[UI_MAX_ID_LENGTH];   // 30 chars
    lv_obj_t        *lv_obj;                 // NULL until _init() runs
    int              x;
    int              y;
    int              w;
    int              h;
    ui_style_t       style;
    union {
        struct { char text[UI_MAX_STRING_LENGTH]; } label;  // 30 chars
        struct { int32_t value; }                  bar;
        struct { const lv_image_dsc_t *src; }      image;
    } data;
} ui_child_t;
```

The `lv_obj` pointer is `NULL` at compile time. It is filled by the screen's `_init()` function at runtime.

---

### `ui_screen_t` — Screen

```c
typedef struct {
    const char  *name;
    ui_child_t   children[UI_MAX_CHILDREN];  // 16 slots
    uint8_t      child_count;
    lv_obj_t    *lv_screen;                  // NULL until _init() runs
} ui_screen_t;
```

---

## Constants

Defined in `ui_defs.h`:

| Constant | Value | Description |
|----------|-------|-------------|
| `UI_MAX_CHILDREN` | 16 | Maximum widgets per screen |
| `UI_MAX_ICON_STATES` | 8 | Maximum states for multi-state icon (future use) |
| `UI_MAX_STRING_LENGTH` | 30 | Maximum label text length including null terminator |
| `UI_MAX_ID_LENGTH` | 30 | Maximum widget ID string length |

---

## Python → C Model Mapping

| Python field | C field |
|-------------|---------|
| `ParsedChild.type` | `ui_child_t.type` |
| `ParsedChild.id` | `ui_child_t.id[]` |
| `ParsedChild.x/y/w/h` | `ui_child_t.x/y/w/h` |
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
