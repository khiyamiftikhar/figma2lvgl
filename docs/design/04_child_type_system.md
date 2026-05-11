# figma2lvgl — Child Type System

The child type system is the **primary extension point** of figma2lvgl. Adding a new LVGL widget type means registering a `ChildSpec` in `child_registry.py` and providing the three template strings it references. No other file needs to change for a basic new widget.

---

## Components

```
child_registry.py      ← CHILDREN dict: "UI_CHILD_*" → ChildSpec
generic_child.py       ← ChildSpec dataclass definition
core/templates/        ← Template strings for each widget
template_loader.py     ← Name → template string lookup
figma_helpers.py       ← Figma XML node → "UI_CHILD_*" type detection
ui_defs.h              ← ui_child_type_t enum (C side)
```

---

## `ChildSpec` Fields

Defined in `core/generic_child.py`:

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type_name` | `str` | Yes | The `UI_CHILD_*` constant string — must match the key in `CHILDREN` and the C enum value |
| `callback_template` | `str` | Yes | Template name for the widget's animation/job callback. Pass `""` for widgets with no async callback. |
| `setter_template` | `str` | Yes | Template name for the public setter function |
| `init_template` | `str` | Yes | Template name for the `switch` case in `_init()` |
| `setter_args` | `str` | Yes | C argument list string for the setter signature |
| `requires_asset` | `bool` | No | Default `False`. Set `True` if the widget requires a matching PNG asset file. `main.py` will validate the file exists before running the pipeline. |

---

## Current Registry

```python
CHILDREN = {
    "UI_CHILD_LABEL": ChildSpec(
        type_name        = "UI_CHILD_LABEL",
        callback_template= "",               # no callback
        setter_template  = "label_setter",
        init_template    = "label_init",
        setter_args      = "const char *text",
    ),
    "UI_CHILD_IMAGE": ChildSpec(
        type_name        = "UI_CHILD_IMAGE",
        callback_template= "",               # no callback
        setter_template  = "image_setter",
        init_template    = "image_init",
        setter_args      = "void",
        requires_asset   = True,
    ),
    "UI_CHILD_BAR": ChildSpec(
        type_name        = "UI_CHILD_BAR",
        callback_template= "bar_callback",
        setter_template  = "bar_setter",
        init_template    = "bar_init",
        setter_args      = "int value, uint32_t duration_ms",
    ),
}
```

---

## What Each Template Generates

### Callback template

A static C function used internally by the widget (e.g. an LVGL animation `exec_cb`). Generated **once per widget type** per screen (not once per widget instance). Empty string = nothing generated.

Example (bar):
```c
static void ui_home_screen_bar_job_cb_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value(obj, v, LV_ANIM_OFF);
}
```

### Setter template

A public C function that application code calls to update the widget at runtime. Generated **once per widget instance**. The function name encodes the screen name and widget ID.

Example (label):
```c
void ui_home_screen_set_time_label(const char *text)
{
    ui_child_t *c = &home_screen.children[0];
    if (c->lv_obj)
        lv_label_set_text(c->lv_obj, text);
}
```

### Init template

A `case` block inside the screen's `_init()` function. Generated **once per widget type** per screen (shared across all instances of the same type). Creates the LVGL object and sets geometry.

Example (label):
```c
case UI_CHILD_LABEL:
    c->lv_obj = lv_label_create(home_screen.lv_screen);
    lv_obj_set_pos(c->lv_obj, c->x, c->y);
    lv_obj_set_width(c->lv_obj, c->w);
    lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
    break;
```

---

## How the Generator Uses the Registry

In `generator.py`, two separate loops walk the children:

**Child loop (setter per instance):**
```python
for index, child in enumerate(screen.children):
    spec = CHILDREN.get(child.type)
    setter_tpl = load_template(spec.setter_template)
    # substitute fn_name, child_index, screen_var, child_id, cb_name
```

**Type loop (callback + init once per type):**
```python
for type_name in unique_types:
    spec = CHILDREN.get(type_name)
    callback_tpl = load_template(spec.callback_template)
    init_tpl     = load_template(spec.init_template)
    # substitute cb_name, screen_var
```

This means: if a screen has 3 labels, you get 3 setter functions but only 1 `case UI_CHILD_LABEL:` block and no callback (since `callback_template` is `""`).

---

## Adding a New Widget Type — Step-by-Step

This example adds a `UI_CHILD_ARC` widget.

### Step 1 — Add the C enum value in `ui_defs.h`

```c
typedef enum {
    UI_CHILD_ICON,
    UI_CHILD_LABEL,
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
    UI_CHILD_ARC,       // ← add here
} ui_child_type_t;
```

Also add a `data` union member if the widget needs runtime data:
```c
union {
    struct { char text[UI_MAX_STRING_LENGTH]; } label;
    struct { int32_t value; }                  bar;
    struct { const lv_image_dsc_t *src; }      image;
    struct { int16_t value; }                  arc;    // ← add here
} data;
```

### Step 2 — Create `core/templates/arc_templates.py`

```python
ARC_CALLBACK = ""   # no animation callback needed

ARC_SETTER = """
void ${fn_name}(int value)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (!c->lv_obj || c->type != UI_CHILD_ARC)
        return;
    lv_arc_set_value(c->lv_obj, value);
}
"""

ARC_INIT = """
    case UI_CHILD_ARC:
        c->lv_obj = lv_arc_create(${screen_var}.lv_screen);
        lv_obj_set_pos(c->lv_obj, c->x, c->y);
        lv_obj_set_size(c->lv_obj, c->w, c->h);
        lv_arc_set_range(c->lv_obj, 0, 100);
        lv_arc_set_value(c->lv_obj, c->data.arc.value);
        break;
"""
```

### Step 3 — Register the templates in `template_loader.py`

```python
from figma2lvgl.core.templates import arc_templates   # ← add import

TEMPLATE_MAP = {
    # ... existing entries ...
    "arc_callback": arc_templates.ARC_CALLBACK,   # ← add
    "arc_setter":   arc_templates.ARC_SETTER,     # ← add
    "arc_init":     arc_templates.ARC_INIT,       # ← add
}
```

### Step 4 — Register the `ChildSpec` in `child_registry.py`

```python
"UI_CHILD_ARC": ChildSpec(
    type_name        = "UI_CHILD_ARC",
    callback_template= "arc_callback",
    setter_template  = "arc_setter",
    init_template    = "arc_init",
    setter_args      = "int value",
),
```

### Step 5 — Add detection in `figma_helpers.py`

```python
def map_tag_to_child_type(node):
    name = node.attrib.get("name", "").lower()

    if node.tag == "Text":
        return "UI_CHILD_LABEL"
    if "bar" in name:
        return "UI_CHILD_BAR"
    if "arc" in name:         # ← add before image check
        return "UI_CHILD_ARC"
    if "icon" in name or "image" in name:
        return "UI_CHILD_IMAGE"
    return "UI_CHILD_LABEL"
```

### Step 6 — Handle the new type in `generator.py`

In `generate_screen()`, the sections that assign `cb_name` and `fn_name` per child and per type need `elif` branches for `UI_CHILD_ARC`:

```python
# In the child loop (setter naming):
elif child.type == "UI_CHILD_ARC":
    cb_name = ""
    fn_name = f"ui_{screen_snake}_set_{child.id}"

# In the type loop (callback naming):
elif type_name == "UI_CHILD_ARC":
    cb_name = ""
```

> **Note:** This manual branching in `generator.py` is a design limitation. Currently `cb_name` and `fn_name` naming is hardcoded per type rather than derived from `ChildSpec`. A future refactor could move naming conventions into `ChildSpec` to make step 6 unnecessary.

---

## `requires_asset` Flag

When `requires_asset=True`, `main.py` calls `screen.get_required_assets(CHILDREN)` before running the pipeline. For each child whose `ChildSpec.requires_asset` is `True`, the child's `id` is added to a set of required asset names. `main.py` then checks that `<images_dir>/<id>.png` exists. If any are missing, the pipeline aborts with a clear error listing all missing files.

Only `UI_CHILD_IMAGE` uses this today. The flag exists to support any future widget type that requires an external asset file.
