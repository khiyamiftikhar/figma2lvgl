# figma2lvgl — Template System

The template system produces the C code blocks that make up each generated file. It operates at two levels: **per-widget templates** (one file per widget type) and **file-level layout templates** (one template for the entire `.c` or `.h` file structure).

All substitution uses Python's `string.Template.substitute()`. Variables are `${variable_name}`. A missing variable raises `KeyError` at generation time — there is no silent fallback.

---

## Two Levels of Templates

### Level 1 — Per-Widget Templates (`core/templates/`)

Define the code blocks contributed by each widget type. Stored as string constants in Python files.

| File | Widget | Constants defined |
|------|--------|------------------|
| `label_templates.py` | Label | `LABEL_CALLBACK`, `LABEL_SETTER`, `LABEL_INIT` |
| `image_templates.py` | Image | `IMAGE_CALLBACK`, `IMAGE_SETTER`, `IMAGE_INIT` |
| `bar_templates.py` | Bar | `BAR_CALLBACK`, `BAR_SETTER`, `BAR_INIT` |

### Level 2 — File Layout Templates (`core/emit/layouts.py`)

Define the full structure of the generated `.c` and `.h` files. Per-widget blocks are inserted as substitution variables.

| Constant | Generates |
|----------|-----------|
| `C_FILE_LAYOUT` | Complete `.c` file |
| `H_FILE_LAYOUT` | Complete `.h` file |

---

## Template Lookup

`core/utils/template_loader.py` maps template name strings to their constant values. Called by `generator.py` via `load_template(name)`.

```python
TEMPLATE_MAP = {
    "label_callback": label_templates.LABEL_CALLBACK,
    "label_setter":   label_templates.LABEL_SETTER,
    "label_init":     label_templates.LABEL_INIT,
    "image_callback": image_templates.IMAGE_CALLBACK,
    "image_setter":   image_templates.IMAGE_SETTER,
    "image_init":     image_templates.IMAGE_INIT,
    "bar_callback":   bar_templates.BAR_CALLBACK,
    "bar_setter":     bar_templates.BAR_SETTER,
    "bar_init":       bar_templates.BAR_INIT,
}
```

`load_template("")` returns `""`. This is the convention for widget types that have no callback — an empty template produces no output.

---

## Per-Widget Template Reference

### Label

**`LABEL_CALLBACK`** — empty string (labels have no async callback)

**`LABEL_SETTER`**
```c
void ${fn_name}(const char *text)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (c->lv_obj) {
        lv_label_set_text(c->lv_obj, text);
    }
}
```

**`LABEL_INIT`**
```c
case UI_CHILD_LABEL:
    c->lv_obj = lv_label_create(${screen_var}.lv_screen);
    lv_obj_set_pos(c->lv_obj, c->x, c->y);
    lv_obj_set_width(c->lv_obj, c->w);
    lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(c->lv_obj, c->data.label.text);
    break;
```

The final `lv_label_set_text(c->lv_obj, c->data.label.text)` call applies the Figma design-time text on first render. Without this call, the label would appear empty even though `data.label.text` was correctly populated from the XML. See the label text lifecycle in `05_code_generation.md` for the full two-phase picture.

Labels use `LV_LABEL_LONG_CLIP` — text wider than the widget width is clipped. Only width is set; height is left at LVGL's default.

---

### Image

**`IMAGE_CALLBACK`** — empty string (images have no async callback)

**`IMAGE_SETTER`**
```c
void ${fn_name}(void)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (c->type != UI_CHILD_IMAGE || c->lv_obj == NULL)
        return;
    c->data.image.src = &${child_id};
    lv_image_set_src(c->lv_obj, c->data.image.src);
}
```

The setter takes `void` — it binds the image source that was declared in `assets.h` by the child's ID. The image data is already embedded in firmware as a C array; the setter points the LVGL object at it.

**`IMAGE_INIT`**
```c
case UI_CHILD_IMAGE:
    c->lv_obj = lv_image_create(${screen_var}.lv_screen);
    lv_obj_set_pos(c->lv_obj, c->x, c->y);
    lv_obj_set_size(c->lv_obj, c->w, c->h);
    if(c->data.image.src)
        lv_image_set_src(c->lv_obj, c->data.image.src);
    break;
```

Both `lv_obj_set_pos` and `lv_obj_set_size` are set from the Figma geometry. `lv_obj_set_size` constrains the bounding box to match the design. The image pixel data is not scaled — `@2x` exports will be clipped or overflow if the source PNG is larger than the LVGL object bounds.

---

### Bar

**`BAR_CALLBACK`**
```c
static void ${cb_name}_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value(obj, v, LV_ANIM_OFF);
}
```

This is the LVGL animation `exec_cb`. Used by the setter when `duration_ms > 0` to drive smooth animated value changes.

**`BAR_SETTER`**
```c
void ${fn_name}(int value, uint32_t duration_ms)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (!c->lv_obj || c->type != UI_CHILD_BAR)
        return;
    if (duration_ms == 0)
    {
        lv_bar_set_value(c->lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, c->lv_obj);
    lv_anim_set_exec_cb(&a, ${cb_name}_exec_cb);
    lv_anim_set_values(&a, lv_bar_get_value(c->lv_obj), value);
    lv_anim_set_time(&a, duration_ms);
    lv_anim_start(&a);
}
```

`duration_ms == 0` → instant update. Any non-zero value → LVGL animation from current to target value.

**`BAR_INIT`**
```c
case UI_CHILD_BAR:
    c->lv_obj = lv_bar_create(${screen_var}.lv_screen);
    lv_obj_set_pos(c->lv_obj, c->x, c->y);
    lv_obj_set_size(c->lv_obj, c->w, c->h);
    lv_bar_set_range(c->lv_obj, 0, 100);
    lv_bar_set_value(c->lv_obj, c->data.bar.value, LV_ANIM_OFF);
    break;
```

Bar range is hardcoded to 0–100. A per-screen `/* TODO */` comment above `_init()` lists which bar IDs may need adjustment. `c->data.bar.value` is always `0` at init time.

---

## File Layout Templates (`core/emit/layouts.py`)

### `C_FILE_LAYOUT` Variable Reference

| Variable | Substituted with |
|----------|-----------------|
| `${header_filename}` | e.g. `"ui_ili9486_home.h"` |
| `${screen_struct}` | Full `ui_screen_t` initialiser block |
| `${job_callbacks}` | Callback function bodies (joined with newline) |
| `${setters}` | Setter function bodies (joined with newline) |
| `${bars_comment}` | Bar range TODO comment, or `""` |
| `${sc_fn_name}` | e.g. `ui_ili9486_home_load` |
| `${init_fn}` | e.g. `ui_ili9486_home_init` |
| `${screen_var}` | e.g. `ili9486_home` |
| `${init_body}` | Switch case blocks (joined with newline) |

### `H_FILE_LAYOUT` Variable Reference

| Variable | Substituted with |
|----------|-----------------|
| `${guard}` | e.g. `UI_ILI9486_HOME_H` |
| `${init_fn}` | e.g. `ui_ili9486_home_init` |
| `${sc_fn_name}` | e.g. `ui_ili9486_home_load` |
| `${setter_prototypes}` | Prototype declarations (joined with newline) |

---

## Design Notes

**No template files on disk.** All templates are Python string constants imported directly. No file I/O, no Jinja2.

**Per-instance vs per-type.** The child loop produces one setter per child instance (3 labels → 3 setters). The type loop produces one callback and one init case per unique type (3 labels → 1 `case UI_CHILD_LABEL:` block). This is enforced by the ordered `unique_types` list with deduplication.

**Deterministic output.** `unique_types` is an ordered list (first-appearance order in `screen.children`), not a set. This guarantees the same XML always produces the same C output regardless of Python runtime hash randomisation. The golden test files in `tests/golden/` validate this.

**`substitute()` not `safe_substitute()`.** A missing template variable raises `KeyError` at generation time. This is the right failure mode for a code generator — fail loud and fast at generation time, not silently at C compile time.
