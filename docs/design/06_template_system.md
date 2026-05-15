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

BUTTON, SLIDER, PANEL, and DYNAMIC widgets are handled directly by the emitter modules (`init_emitter.py`, `setter_emitter.py`) without template strings — their code patterns are more complex and tightly coupled to the emitter's BFS traversal state.

### Level 2 — File Layout Templates (`core/emit/layouts.py`)

Define the full structure of the generated `.c` and `.h` files. Per-widget blocks are inserted as substitution variables.

| Constant | Generates |
|----------|-----------| 
| `_C_LAYOUT` | Complete `.c` file |
| `_H_LAYOUT` | Complete `.h` file |

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

Templates use `${path}` to reference the widget's struct field in the screen-specific static struct — e.g. `s_home.panel_top.time`. This is a direct named path into the generated hierarchy, never a generic array index.

### Label

**`LABEL_CALLBACK`** — empty string (labels have no async callback)

**`LABEL_SETTER`**
```c
void ${fn_name}(const char *text)
{
    if (!${path}.lv_obj) return;
    lv_label_set_text(${path}.lv_obj, text);
    snprintf(${path}.text, UI_MAX_STRING_LENGTH, "%s", text);
}
```

The setter updates both the live LVGL object and the struct's `text[]` buffer. The buffer holds the current runtime value; the LVGL object reflects it immediately.

**`LABEL_INIT`**
```c
${path}.lv_obj = lv_label_create(${parent_lv_obj});
lv_obj_set_pos(${path}.lv_obj, ${x}, ${y});
lv_obj_set_width(${path}.lv_obj, ${w});
lv_label_set_long_mode(${path}.lv_obj, LV_LABEL_LONG_CLIP);
lv_label_set_text(${path}.lv_obj, ${path}.text);
ui_apply_style(${path}.lv_obj, UI_CHILD_LABEL, &${path}.style);
```

`lv_label_set_text(${path}.lv_obj, ${path}.text)` applies the Figma design-time text on first render. Without this call, the label would appear empty even though `${path}.text` was correctly populated from the XML. See the label text lifecycle in `05_code_generation.md` for the full two-phase picture.

Labels use `LV_LABEL_LONG_CLIP` — text wider than the widget width is clipped. Only width is set; height is left at LVGL's default.

---

### Image

**`IMAGE_CALLBACK`** — empty string (images have no async callback)

**`IMAGE_SETTER`**
```c
void ${fn_name}(void)
{
    if (!${path}.lv_obj) return;
    ${path}.src = &${child_id};
    lv_image_set_src(${path}.lv_obj, ${path}.src);
}
```

The setter takes `void` — it binds the image source array (declared in `assets.h`) by the widget's normalized ID. The image data is already embedded in firmware as a C array; the setter points the LVGL object at it.

**`IMAGE_INIT`**
```c
${path}.lv_obj = lv_image_create(${parent_lv_obj});
lv_obj_set_pos(${path}.lv_obj, ${x}, ${y});
lv_obj_set_size(${path}.lv_obj, ${w}, ${h});
if (${path}.src)
    lv_image_set_src(${path}.lv_obj, ${path}.src);
ui_apply_style(${path}.lv_obj, UI_CHILD_IMAGE, &${path}.style);
```

Both `lv_obj_set_pos` and `lv_obj_set_size` are set from the Figma geometry. `lv_obj_set_size` constrains the bounding box to match the design. The image pixel data is not scaled — `@2x` exports will be clipped or overflow if the source PNG is larger than the LVGL object bounds.

---

### Bar

**`BAR_CALLBACK`**
```c
static void _bar_anim_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value(obj, v, LV_ANIM_OFF);
}
```

This is the LVGL animation `exec_cb`. Used by the setter when `duration_ms > 0` to drive smooth animated value changes. Emitted once per `.c` file (not once per bar instance) when `bar_anim_needed` is True.

**`BAR_SETTER`**
```c
void ${fn_name}(int value, uint32_t duration_ms)
{
    if (!${path}.lv_obj) return;
    if (duration_ms == 0)
    {
        lv_bar_set_value(${path}.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ${path}.lv_obj);
    lv_anim_set_exec_cb(&a, _bar_anim_exec_cb);
    lv_anim_set_values(&a, lv_bar_get_value(${path}.lv_obj), value);
    lv_anim_set_time(&a, duration_ms);
    lv_anim_start(&a);
}
```

`duration_ms == 0` → instant update. Any non-zero value → LVGL animation from current to target value.

**`BAR_INIT`**
```c
${path}.lv_obj = lv_bar_create(${parent_lv_obj});
lv_obj_set_pos(${path}.lv_obj, ${x}, ${y});
lv_obj_set_size(${path}.lv_obj, ${w}, ${h});
lv_bar_set_range(${path}.lv_obj, ${path}.min, ${path}.max);
lv_bar_set_value(${path}.lv_obj, ${path}.value, LV_ANIM_OFF);
ui_apply_style(${path}.lv_obj, UI_CHILD_BAR, &${path}.style);
```

Range comes from the struct's `.min` and `.max` fields, which were set from the Figma node name (e.g. `battery_bar_0_100` → min=0, max=100). A `/* TODO: adjust range */` comment is emitted above `_init()` listing all bar IDs.

---

## File Layout Templates (`core/emit/layouts.py`)

### `_C_LAYOUT` Variable Reference

| Variable | Substituted with |
|----------|-----------------| 
| `${h_fname}` | e.g. `"ui_home.h"` |
| `${struct}` | File-static screen struct + initializer block (from `node_emitter.py`) |
| `${bar_anim}` | `_bar_anim_exec_cb` helper, or `""` if no bars |
| `${callbacks}` | Weak event callback function bodies (joined with newline) |
| `${setters}` | Setter function bodies (joined with newline) |
| `${load_fn}` | e.g. `ui_home_load` |
| `${init_fn}` | e.g. `ui_home_init` |
| `${sv}` | Screen variable name, e.g. `s_home` |
| `${init_body}` | Flat BFS sequence of LVGL create/configure calls (from `init_emitter.py`) |

### `_H_LAYOUT` Variable Reference

| Variable | Substituted with |
|----------|-----------------| 
| `${guard}` | e.g. `UI_HOME_H` |
| `${init_fn}` | e.g. `ui_home_init` |
| `${load_fn}` | e.g. `ui_home_load` |
| `${setters}` | Setter prototype declarations (joined with newline) |
| `${cb_decls}` | Callback `__attribute__((weak))` declarations (joined with newline) |

---

## Design Notes

**No template files on disk.** All templates are Python string constants imported directly. No file I/O, no Jinja2.

**Direct struct paths, never array indices.** All templates reference widgets via their typed struct path (`${path}.lv_obj`, `${path}.text`) — never via `children[N]` or any generic array. The path is computed by the emitter's BFS traversal and passed as a substitution variable.

**Per-instance setters.** The setter emitter produces one setter per widget instance (3 labels → 3 setter functions with three different `${path}` values). This is enforced by the BFS walk in `setter_emitter.py`.

**Deterministic output.** BFS traversal order is deterministic (same XML always produces same node order). The golden test files in `tests/golden/` validate this — same XML in, same C out, regardless of Python runtime state.

**`substitute()` not `safe_substitute()`.** A missing template variable raises `KeyError` at generation time. This is the right failure mode for a code generator — fail loud and fast at generation time, not silently at C compile time.
