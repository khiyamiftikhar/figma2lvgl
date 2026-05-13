# figma2lvgl — Child Type System

The child type system is the **primary extension point** of figma2lvgl. Adding a new LVGL widget type requires registering a `ChildSpec` in `child_registry.py` and providing three template strings. The generator needs no changes.

---

## Components

```
widget_type.py         ← WidgetType enum: LABEL / IMAGE / BAR
child_registry.py      ← CHILDREN dict: WidgetType → ChildSpec
generic_child.py       ← ChildSpec dataclass (+ naming pattern methods)
core/templates/        ← Template strings for each widget
template_loader.py     ← Template name → string lookup
figma_helpers.py       ← Figma XML node → WidgetType detection
ui_defs.h              ← ui_child_type_t enum (C side)
```

---

## `ChildSpec` Fields

Defined in `core/generic_child.py`:

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `type_name` | `WidgetType` | Yes | Enum value — matches the key in `CHILDREN` and the C enum |
| `callback_template` | `str` | Yes | Template name for animation callback; `""` = no callback |
| `setter_template` | `str` | Yes | Template name for the public setter |
| `init_template` | `str` | Yes | Template name for the `switch` init case |
| `setter_args` | `str` | Yes | C argument list for the setter |
| `requires_asset` | `bool` | No | Default `False`; if `True`, main.py validates a matching PNG exists |
| `setter_name_pattern` | `str` | No | Default `"ui_{screen}_set_{child_id}"` |
| `callback_name_pattern` | `str` | No | Default `""`; pattern for callback name |

`derive_setter_name(screen_snake, child_id)` and `derive_callback_name(screen_snake)` format these patterns. Because naming is in `ChildSpec`, `generator.py` has **no per-type if/elif branches** — it calls `spec.derive_*` for every widget type uniformly.

---

## Current Registry

```python
CHILDREN = {
    WidgetType.LABEL: ChildSpec(
        type_name             = WidgetType.LABEL,
        callback_template     = "",
        setter_template       = "label_setter",
        init_template         = "label_init",
        setter_args           = "const char *text",
        setter_name_pattern   = "ui_{screen}_set_{child_id}",
        callback_name_pattern = "",
    ),
    WidgetType.IMAGE: ChildSpec(
        type_name             = WidgetType.IMAGE,
        callback_template     = "",
        setter_template       = "image_setter",
        init_template         = "image_init",
        setter_args           = "void",
        requires_asset        = True,
        setter_name_pattern   = "ui_{screen}_display_{child_id}",
        callback_name_pattern = "",
    ),
    WidgetType.BAR: ChildSpec(
        type_name             = WidgetType.BAR,
        callback_template     = "bar_callback",
        setter_template       = "bar_setter",
        init_template         = "bar_init",
        setter_args           = "int value, uint32_t duration_ms",
        setter_name_pattern   = "ui_{screen}_set_{child_id}",
        callback_name_pattern = "ui_{screen}_bar_job_cb",
    ),
}
```

---

## What Each Template Generates

### Callback template

A static C function called internally (e.g. LVGL animation `exec_cb`). Generated **once per widget type** per screen. Empty string = nothing generated.

### Setter template

A public C function firmware calls to update the widget at runtime. Generated **once per widget instance**. Function name derived from `setter_name_pattern`.

### Init template

A `case UI_CHILD_*:` block inside `_init()`. Generated **once per widget type** per screen (one block handles all instances of that type via the `switch`).

---

## Adding a New Widget Type — Step-by-Step

This example adds `WidgetType.ARC`.

### Step 1 — Add to `WidgetType` enum in `widget_type.py`

```python
class WidgetType(Enum):
    LABEL = "UI_CHILD_LABEL"
    IMAGE = "UI_CHILD_IMAGE"
    BAR   = "UI_CHILD_BAR"
    ARC   = "UI_CHILD_ARC"       # ← add
```

### Step 2 — Add to `ui_child_type_t` in `ui_defs.h`

```c
typedef enum {
    UI_CHILD_LABEL,
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
    UI_CHILD_ARC,       // ← add
} ui_child_type_t;
```

Add a `data` union member if the widget needs runtime data:
```c
struct { int16_t value; } arc;    // ← inside the union
```

### Step 3 — Create `core/templates/arc_templates.py`

```python
ARC_CALLBACK = ""

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
        break;
"""
```

### Step 4 — Register templates in `template_loader.py`

```python
from figma2lvgl.core.templates import arc_templates

TEMPLATE_MAP = {
    # ... existing entries ...
    "arc_callback": arc_templates.ARC_CALLBACK,
    "arc_setter":   arc_templates.ARC_SETTER,
    "arc_init":     arc_templates.ARC_INIT,
}
```

### Step 5 — Register `ChildSpec` in `child_registry.py`

```python
WidgetType.ARC: ChildSpec(
    type_name             = WidgetType.ARC,
    callback_template     = "arc_callback",
    setter_template       = "arc_setter",
    init_template         = "arc_init",
    setter_args           = "int value",
    setter_name_pattern   = "ui_{screen}_set_{child_id}",
    callback_name_pattern = "",
),
```

### Step 6 — Add detection in `figma_helpers.py`

```python
def map_tag_to_child_type(node):
    name = node.attrib.get("name", "").lower()
    if node.tag == "Text":              return WidgetType.LABEL
    if "bar" in name:                   return WidgetType.BAR
    if "arc" in name:                   return WidgetType.ARC    # ← before image
    if "icon" in name or "image" in name: return WidgetType.IMAGE
    return None
```

### Step 7 — Update `figma_parser.py` (if the widget has a data union member)

In the screen struct generation section of `generator.py`, add a data block for `WidgetType.ARC`:

```python
elif child.type == WidgetType.ARC:
    data_block = """
        .data.arc = {
            .value = 0
        }
"""
```

**That's all.** Because naming is in `ChildSpec`, `generator.py` derives `fn_name` and `cb_name` via `spec.derive_*()` without any per-type branching.

---

## `requires_asset` Flag

When `True`, `main.py` calls `screen.get_required_assets(CHILDREN)` for each screen. For each child whose `ChildSpec.requires_asset` is `True`, it checks that `<images_dir>/<child_id>.png` exists. If any are missing, the pipeline aborts before writing any output.

Only `WidgetType.IMAGE` uses this currently.
