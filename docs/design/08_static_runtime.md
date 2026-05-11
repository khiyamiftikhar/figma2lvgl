# figma2lvgl — Static Runtime

The static runtime consists of three files in `static_src/` that are copied verbatim into `ui_src/priv_src/` and `ui_src/priv_include/` on every generation run. They are not generated — they are fixed support files that the generated screen code depends on.

```
static_src/
  ui_defs.h      → priv_include/ui_defs.h
  ui_style.h     → priv_include/ui_style.h
  ui_style.c     → priv_src/ui_style.c
```

---

## `ui_defs.h` — Struct and Enum Definitions

The single header that all generated `.c` files include. Contains every type the generated code uses.

### Constants

| Constant | Value | Purpose |
|----------|-------|---------|
| `UI_MAX_CHILDREN` | 16 | Maximum `ui_child_t` slots in `ui_screen_t.children[]` |
| `UI_MAX_ICON_STATES` | 8 | Maximum states for multi-state icon (future use) |
| `UI_MAX_STRING_LENGTH` | 30 | `ui_child_t.data.label.text[]` size — label text is truncated to this |
| `UI_MAX_ID_LENGTH` | 30 | `ui_child_t.id[]` size — widget ID string |

### `ui_child_type_t`

```c
typedef enum {
    UI_CHILD_ICON,       // defined but no active ChildSpec yet
    UI_CHILD_LABEL,
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
} ui_child_type_t;
```

Used in `ui_child_t.type` and in the `switch` statement in `_init()`.

### Style Structs

Three structs, nested inside `ui_style_t`. Each property has a boolean `has_*` guard so zero-initialised (`{0}`) is safe — it means "no style applied".

**`ui_style_box_t`**

```c
typedef struct {
    bool        has_bg;
    uint32_t    bg;              // raw hex e.g. 0x4CAF50
    bool        has_bg_opa;
    uint8_t     bg_opa;          // 0–255
    bool        has_border_color;
    uint32_t    border_color;
    bool        has_border_width;
    lv_coord_t  border_width;
    bool        has_radius;
    lv_coord_t  radius;
} ui_style_box_t;
```

**`ui_style_text_t`**

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

**`ui_style_effects_t`**

```c
typedef struct {
    bool    has_opacity;
    uint8_t opacity;    // 0–255
} ui_style_effects_t;
```

**`ui_style_t`**

```c
typedef struct {
    ui_style_box_t      box;
    ui_style_text_t     text;
    ui_style_effects_t  effects;
} ui_style_t;
```

### `ui_child_t`

```c
typedef struct {
    ui_child_type_t  type;
    char             id[UI_MAX_ID_LENGTH];
    lv_obj_t        *lv_obj;     // NULL until screen _init() runs
    int              x, y, w, h;
    ui_style_t       style;
    union {
        struct { char text[UI_MAX_STRING_LENGTH]; } label;
        struct { int32_t value; }                  bar;
        struct { const lv_image_dsc_t *src; }      image;
    } data;
} ui_child_t;
```

`lv_obj` is `NULL` in the static initialiser. The screen's `_init()` function creates the LVGL objects and fills these pointers. All setter functions guard against `lv_obj == NULL` before calling LVGL.

### `ui_screen_t`

```c
typedef struct {
    const char  *name;
    ui_child_t   children[UI_MAX_CHILDREN];
    uint8_t      child_count;
    lv_obj_t    *lv_screen;     // NULL until screen _init() runs
} ui_screen_t;
```

The `children` array is fixed at `UI_MAX_CHILDREN` (16). Screens with more than 16 widgets will overflow — there is no runtime or generation-time guard for this currently.

---

## `ui_style.h` — Style API Declaration

```c
void ui_apply_style(lv_obj_t *obj, ui_child_type_t type, const ui_style_t *s);
```

This is the only public API in the static runtime. Called once per child at the end of each screen's `_init()` function.

---

## `ui_style.c` — Style Application and Font Mapping

### `ui_apply_style()`

Reads each `has_*` flag in the style struct and makes the corresponding LVGL API call if the flag is set. All calls use `LV_PART_MAIN`.

**Box styles (all widget types):**

| Flag | LVGL call |
|------|----------|
| `has_bg` | `lv_obj_set_style_bg_color(obj, lv_color_hex(s->box.bg), LV_PART_MAIN)` |
| `has_bg_opa` | `lv_obj_set_style_bg_opa(obj, s->box.bg_opa, LV_PART_MAIN)` |
| `has_border_color` | `lv_obj_set_style_border_color(obj, lv_color_hex(s->box.border_color), LV_PART_MAIN)` |
| `has_border_width` | `lv_obj_set_style_border_width(obj, s->box.border_width, LV_PART_MAIN)` |
| `has_radius` | `lv_obj_set_style_radius(obj, s->box.radius, LV_PART_MAIN)` |

**Text styles (labels only — guarded by `type == UI_CHILD_LABEL`):**

| Flag | LVGL call |
|------|----------|
| `has_color` | `lv_obj_set_style_text_color(obj, lv_color_hex(s->text.color), LV_PART_MAIN)` |
| `has_size` | `lv_obj_set_style_text_font(obj, ui_get_font(s->text.size), LV_PART_MAIN)` |
| `has_align` | `lv_obj_set_style_text_align(obj, s->text.align, LV_PART_MAIN)` |

**Effects (all widget types):**

| Flag | LVGL call |
|------|----------|
| `has_opacity` | `lv_obj_set_style_opa(obj, s->effects.opacity, LV_PART_MAIN)` |

**Bar indicator special case:**

When `type == UI_CHILD_BAR` and `has_bg` is set, the background color is also applied to `LV_PART_INDICATOR`:
```c
lv_obj_set_style_bg_color(obj, lv_color_hex(s->box.bg), LV_PART_INDICATOR);
```
This ensures the Figma fill color controls the filled portion of the bar, not just its track background.

---

### `ui_get_font()`

Maps a font size integer to an LVGL Montserrat font pointer. Guarded by `#if LV_FONT_MONTSERRAT_N` so only fonts enabled in `lv_conf.h` compile in. Falls back to `LV_FONT_DEFAULT` for unmapped sizes.

| Size | Font |
|------|------|
| 10 | `lv_font_montserrat_10` |
| 12 | `lv_font_montserrat_12` |
| 14 | `lv_font_montserrat_14` |
| 16 | `lv_font_montserrat_16` |
| 18 | `lv_font_montserrat_18` |
| 20 | `lv_font_montserrat_20` |
| 22 | `lv_font_montserrat_22` |
| 24 | `lv_font_montserrat_24` |
| any other | `LV_FONT_DEFAULT` |

Each font must be enabled in `lv_conf.h` to be available. Only enable the sizes your design actually uses — each Montserrat font adds significant flash usage on embedded targets.

---

## What Generated Code Depends On

A generated screen `.c` file depends on:

| Dependency | Source |
|-----------|--------|
| `ui_defs.h` | Copied from `static_src/` |
| `ui_style.h` | Copied from `static_src/` |
| `assets.h` | Generated by image pipeline |
| `lvgl.h` | Must be provided by the target project |
| Own `.h` file | Generated |

The generated `.h` file exposes only:
- `void ui_{screen}_init(void)`
- `void ui_{screen}_load(void)`
- Setter prototypes

Application code should call `_init()` once at startup, then `_load()` to switch the active screen, then setters to update individual widgets.
