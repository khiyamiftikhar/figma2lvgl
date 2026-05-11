# figma2lvgl — Template System

The template system produces the C code blocks that make up each generated file. It operates at two levels: **per-widget templates** (one file per widget type) and **file-level layout templates** (one template for the entire `.c` or `.h` file structure).

All substitution uses Python's `string.Template.safe_substitute()` — variables are written as `${variable_name}`.

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

Define the full structure of the generated `.c` and `.h` files. Per-widget blocks are inserted into these layouts as substitution variables.

| Constant | Generates |
|----------|-----------|
| `C_FILE_LAYOUT` | The complete `.c` file |
| `H_FILE_LAYOUT` | The complete `.h` file |

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

`load_template("")` returns `""` (empty string). This is the convention for widget types that have no callback.

---

## Per-Widget Template Reference

### Label

**`LABEL_CALLBACK`** — empty string (labels have no async callback)

**`LABEL_SETTER`**
```c
void ${fn_name}(const char *text)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (c->lv_obj)
        lv_label_set_text(c->lv_obj, text);
}
```

**`LABEL_INIT`**
```c
case UI_CHILD_LABEL:
    c->lv_obj = lv_label_create(${screen_var}.lv_screen);
    lv_obj_set_pos(c->lv_obj, c->x, c->y);
    lv_obj_set_width(c->lv_obj, c->w);
    lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
    break;
```

> Labels use `LV_LABEL_LONG_CLIP` — text wider than the widget is clipped, not wrapped or scrolled. Height is not set; only width constrains the label.

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

> The setter takes `void` — it binds the image source that was declared in `assets.h` by name. The image data is already embedded in the firmware as a C array; the setter just points the LVGL object at it.

**`IMAGE_INIT`**
```c
case UI_CHILD_IMAGE:
    c->lv_obj = lv_image_create(${screen_var}.lv_screen);
    lv_obj_set_pos(c->lv_obj, c->x, c->y);
    if (c->data.image.src)
        lv_image_set_src(c->lv_obj, c->data.image.src);
    break;
```

---

### Bar

**`BAR_CALLBACK`**
```c
static void ${cb_name}_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value(obj, v, LV_ANIM_OFF);
}
```

This is the LVGL animation `exec_cb`. It is used by the setter when `duration_ms > 0` to drive a smooth animated value change.

**`BAR_SETTER`**
```c
void ${fn_name}(int value, uint32_t duration_ms)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (!c->lv_obj || c->type != UI_CHILD_BAR)
        return;
    if (duration_ms == 0) {
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

> `duration_ms == 0` triggers an instant update. Any non-zero value triggers an LVGL animation from the current bar value to the target value.

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

> Bar range is hardcoded to 0–100. `c->data.bar.value` is `0` at init time (set in the struct initialiser).

---

## File Layout Templates (`core/emit/layouts.py`)

### `C_FILE_LAYOUT` Variable Reference

| Variable | Substituted with |
|----------|-----------------|
| `${header_filename}` | e.g. `"ui_home_screen.h"` |
| `${screen_struct}` | Full `ui_screen_t` static initialiser |
| `${job_callbacks}` | All callback function bodies joined with newline |
| `${sc_fn_cb_name}` | Load job callback name (in layout but currently unused) |
| `${sc_fn_name}` | e.g. `ui_home_screen_load` |
| `${init_fn}` | e.g. `ui_home_screen_init` |
| `${screen_var}` | e.g. `home_screen` |
| `${init_body}` | All `case` blocks joined with newline |
| `${setters}` | All setter function bodies joined with newline |

### `H_FILE_LAYOUT` Variable Reference

| Variable | Substituted with |
|----------|-----------------|
| `${guard}` | e.g. `UI_HOME_SCREEN_H` |
| `${init_fn}` | e.g. `ui_home_screen_init` |
| `${sc_fn_name}` | e.g. `ui_home_screen_load` |
| `${setter_prototypes}` | All setter prototypes joined with newline |

---

## Design Notes

**`safe_substitute` vs `substitute`:** `safe_substitute()` is used throughout. This means unknown `${variables}` are left as-is rather than raising a `KeyError`. This is intentional — it prevents crashes if a template uses a variable that a particular widget type doesn't provide.

**No template files on disk:** All templates are Python string constants imported directly. There is no template file loading from disk, no Jinja2, no file I/O in the template layer.

**Per-instance vs per-type generation:** The child loop generates one setter per child instance (so 3 labels → 3 setters). The type loop generates one callback and one init case per unique type (so 3 labels → 1 `case UI_CHILD_LABEL:` block). This is enforced by iterating `unique_types = set(child.type for child in screen.children)` in the type loop.
