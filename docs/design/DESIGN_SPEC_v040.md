# figma2lvgl — v0.4.0 Architecture Specification

## Purpose of This Document

This document is the authoritative design spec for v0.4.0. It supersedes the
v0.3.0 design docs where they conflict. A new implementation session reading
this document alongside the v0.3.0 repo should be able to generate the full
v0.4.0 implementation without additional context.

---

## 1. Tool Philosophy (Do Not Compromise)

figma2lvgl is a **translator** between two worlds that have incompatible
assumptions:

- **Figma** — designed for app/web UI: responsive layouts, gestures, GPU
  rendering, dynamic content, design systems
- **LVGL** — designed for embedded targets: fixed display, limited RAM,
  no heap at runtime (ideally), static object trees, direct pointer access

The tool is the adapter between these worlds. It must be **transparent to both
sides**:

- The **UI designer** opens Figma and designs the way Figma is meant to be
  used — frames, components, nesting, auto-layout, text layers. No naming
  tricks. No workarounds. Just design.
- The **firmware developer** gets clean, idiomatic LVGL C code that reflects
  the design intent. No generic arrays, no index magic. Named access, direct
  pointers, self-documenting API.

**The embedded lens rule:** Not every Figma feature maps to LVGL. The tool
extracts what LVGL can represent faithfully, makes reasonable approximations
for partial matches (e.g. corner radius → `lv_obj_set_style_radius`), and
silently ignores what has no embedded equivalent (gradients, shadows, hover
states, scroll physics, responsive breakpoints). This is not a limitation —
it is a deliberate choice. Embedded UIs have different constraints.

---

## 2. What Changed from v0.3.0

### v0.3.0 model (retired in v0.4.0)

```c
ui_screen_t home = {
    .child_count = 4,
    .children = {
        { .type = UI_CHILD_LABEL, .id = "time", ... },
        { .type = UI_CHILD_BAR,   .id = "battery_bar", ... },
        ...
    }
};
```

**Problems with this:**
- Generic flat array — index-based access (`children[2]`)
- Every label gets a RAM buffer (`char text[30]`) even if it never changes
- Supports only direct children of the Frame (no nesting)
- No interactive widget support (button, slider)
- `ui_child_t` is a one-size-fits-all struct: union grows with every new type

### v0.4.0 model (this document)

Each screen gets its own **typed, hierarchical, file-static struct** that
mirrors the Figma frame hierarchy. The generic `ui_child_t` and `ui_screen_t`
structs are **removed from ui_defs.h entirely**.

---

## 3. C Data Model — Screen-Specific Struct

### Core principle

Every Figma `<Frame>` that represents a screen generates its own C struct type.
The struct mirrors the Figma hierarchy — nested Figma containers become nested
C structs.

### The struct is file-static and private

The struct is defined and instantiated as a `static` variable in the generated
`.c` file. It is **never exposed in the `.h` file**. The firmware developer
interacts only through the generated API functions.

### Text allocation rule

| Situation | C representation | Stored in |
|-----------|-----------------|-----------|
| Label/button text that **can be updated** at runtime (a setter is generated) | `char text[UI_MAX_STRING_LENGTH]` | RAM |
| Label/button text that is **static** (no setter generated, set once in Figma) | `const char *text` pointing to string literal | Flash (RO) |

### Example: home screen with panel, button, slider

Figma hierarchy:
```
home (Frame — screen)
  ├── panel_top (Frame — container)
  │     ├── time (Text — dynamic label)
  │     └── icon_wifi (Instance — image)
  ├── btn_ok (Frame — button)
  └── brightness_slider (Rectangle — slider)
```

Generated struct (in `ui_home.c`, file-static):
```c
static struct {

    lv_obj_t *lv_screen;

    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;

        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH]; /* dynamic — setter generated */
        } time;

        struct {
            lv_obj_t              *lv_obj;
            ui_style_t             style;
            const lv_image_dsc_t  *src;
        } icon_wifi;

    } panel_top;

    struct {
        lv_obj_t      *lv_obj;
        ui_style_t     style;
        const char    *label_text;  /* static — "Ok" lives in Flash */
    } btn_ok;

    struct {
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

| Widget type | Fields in struct | Notes |
|------------|-----------------|-------|
| Any | `lv_obj_t *lv_obj` | NULL until `_init()` runs |
| Any | `ui_style_t style` | Style data extracted from Figma |
| LABEL (dynamic) | `char text[UI_MAX_STRING_LENGTH]` | RAM buffer, setter generated |
| LABEL (static) | `const char *text` | Flash pointer, no setter |
| IMAGE | `const lv_image_dsc_t *src` | NULL until setter called |
| BAR | `int32_t value` | Initial value (0) |
| BUTTON | `const char *label_text` | Flash pointer (button labels are usually static) |
| SLIDER | `int32_t value, min, max` | Min/max from Figma name convention (see §5) |
| PANEL | `lv_obj_t *lv_obj` + `ui_style_t style` + child structs | Container only |
| DYNAMIC CONTAINER | `lv_obj_t *lv_obj` | Generator stops here, no child structs |

---

## 4. Generated API — Public Header

The `.h` file exposes only the public API. No struct types, no internal fields.

```c
/* ui_home.h */
#ifndef UI_HOME_H
#define UI_HOME_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

/* ── Lifecycle ────────────────────────────────────────────── */
void ui_home_init(void);
void ui_home_load(void);

/* ── Setters: dynamic widgets only ───────────────────────── */
void ui_home_panel_top_time_set_text(const char *text);
void ui_home_panel_top_display_icon_wifi(void);
void ui_home_btn_ok_set_label(const char *text);     /* if button label is dynamic */
void ui_home_brightness_slider_set_value(int32_t value);
void ui_home_battery_bar_set_value(int value, uint32_t duration_ms);

/* ── Dynamic container access (if any) ───────────────────── */
/* lv_obj_t *ui_home_get_list_devices(void); */

/* ── Event callbacks — override in application .c ────────── */
void ui_home_on_btn_ok(lv_event_t *e);
void ui_home_on_brightness_slider(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
```

### API naming convention

Function names encode the hierarchy path from the screen root to the widget:

```
ui_{screen}_{parent?}_{grandparent?}_{widget_id}_{action}
```

Examples:
- `ui_home_time_set_text` — time is a direct child of home
- `ui_home_panel_top_time_set_text` — time is inside panel_top
- `ui_home_on_btn_ok` — event callback for btn_ok

**Depth limit in names:** paths deeper than 4 levels are truncated to the
last 3 path segments after the screen name. Generator emits a warning.
Never truncate the widget ID (last segment) or the action (last word).

**No setters generated for:**
- Structural containers (Panel, auto-layout frames)
- Static text (const char *) — text is fixed from Figma design
- Widget-internal sub-elements (button's inner label is accessed via the
  button setter, not a separate function)

---

## 5. Widget Type System

### Detection priority order

For any Figma XML node, the generator applies these rules in order:

1. **Figma node tag is `Text`** → `LABEL`
2. **Name starts with `btn_` or `button_`** → `BUTTON`
3. **Name starts with `slider_` or ends with `_slider`** → `SLIDER`
4. **Name starts with `list_` or `grid_`** → `DYNAMIC_CONTAINER` (stop recursion)
5. **Name contains `bar`** → `BAR`
6. **Name contains `icon` or `image`** → `IMAGE`
7. **Has children AND (has fill OR has border OR is named)** → `PANEL` (recurse into children)
8. **Has children AND no visual properties AND no significant name** → structural frame: **drop it**, promote children to parent
9. **No children AND no type match** → skip with WARNING

### Widget type reference

#### LABEL

```
Figma: Text node (any name)
LVGL:  lv_label_create()
Data:  text content from `characters` attribute
       dynamic (char[]) if setter generated
       static (const char *) if no setter
Setter: ui_{screen}_{path}_set_text(const char *text)
```

#### IMAGE

```
Figma: Node with "icon" or "image" in name
LVGL:  lv_image_create()
Data:  const lv_image_dsc_t *src (NULL until setter called)
       PNG file must exist: <images_dir>/<node_id>.png
Setter: ui_{screen}_{path}_display_{id}(void)
```

#### BAR

```
Figma: Rectangle/Frame with "bar" in name
LVGL:  lv_bar_create()
Data:  int32_t value (initial: 0)
       range: 0–100 default
       override: encode in name: "battery_bar_0_100"
Setter: ui_{screen}_{path}_set_{id}(int value, uint32_t duration_ms)
Callback: none
```

#### BUTTON

```
Figma: Frame/Rectangle/Component named btn_* or button_*
       Optional: first child Text node provides label_text
LVGL:  lv_button_create()
       lv_label_create(button_obj) — internal, not a separate widget
Data:  const char *label_text
       → from first Text child's `characters` attr if present
       → from node name if no Text child (strip prefix, capitalize)
Setter: ui_{screen}_{path}_{id}_set_label(const char *text) — only if dynamic
Callback: ui_{screen}_on_{id}(lv_event_t *e) — __attribute__((weak))
Event: LV_EVENT_CLICKED
```

**Button label lookup (one level peek):**
The generator looks inside the button's `<children>` for the first `<Text>`
node. If found, its `characters` attribute becomes `label_text`. This is the
ONLY case where the generator reads one level deeper as an internal detail —
it does not create a separate LABEL widget for it.

#### SLIDER

```
Figma: Rectangle/Frame named slider_* or *_slider
LVGL:  lv_slider_create()
Data:  int32_t value (initial), min, max
       range from name: "brightness_slider_0_255" → min=0, max=255
       default range: 0–100
Setter: ui_{screen}_{path}_{id}_set_value(int32_t value)
Callback: ui_{screen}_on_{id}(lv_event_t *e) — __attribute__((weak))
Event: LV_EVENT_VALUE_CHANGED
```

#### PANEL (container)

```
Figma: Frame with children, has visual properties (fill/border/radius)
       OR has a meaningful name (not unnamed/auto-named)
LVGL:  lv_obj_create()
       scrollbars disabled
       click events disabled (LV_OBJ_FLAG_CLICKABLE off)
Data:  ui_style_t style only
       child structs nested inside
No setter generated
No callback generated
```

#### DYNAMIC_CONTAINER

```
Figma: Frame named list_* or grid_*
LVGL:  lv_obj_create() — only the container, no children
Generator: STOPS recursion here
Public accessor: ui_{screen}_get_{id}(void) → lv_obj_t *
Firmware: calls lv_obj_create(ui_home_get_list_devices()) at runtime
```

#### Structural frame (dropped)

```
Figma: Frame with no fill, no border, no meaningful name
       Used purely for spacing/grouping in Figma
Generator: DROPS this node from the tree
           All children promoted to this frame's parent
           Warning emitted in verbose mode only
```

---

## 6. Event Callback Pattern

Interactive widgets (BUTTON, SLIDER) generate weak callback declarations:

```c
/* In ui_home.c — generated, overrideable */
__attribute__((weak)) void ui_home_on_btn_ok(lv_event_t *e)
{
    (void)e;
}

__attribute__((weak)) void ui_home_on_brightness_slider(lv_event_t *e)
{
    (void)e;
    /* tip: int32_t val = lv_slider_get_value(lv_event_get_target(e)); */
}
```

The firmware developer overrides these in their own `.c` file:

```c
/* app/ui_callbacks.c — firmware developer's file */
void ui_home_on_btn_ok(lv_event_t *e)
{
    ui_settings_load();   /* navigate to settings screen */
}
```

**Platform note:** `__attribute__((weak))` is GCC/Clang only. This is
acceptable — all supported embedded targets (ESP-IDF, Zephyr, bare-metal ARM)
use GCC. Document this constraint clearly.

---

## 7. Generated `_init()` function

`_init()` is a flat sequence of LVGL creation calls, ordered parent-before-child.
No recursion, no deeply nested C braces that increase stack depth.

```c
void ui_home_init(void)
{
    s_home.lv_screen = lv_obj_create(NULL);

    /* ── panel_top (parent: screen) ────────────────────── */
    s_home.panel_top.lv_obj = lv_obj_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.panel_top.lv_obj,  0, 0);
    lv_obj_set_size(s_home.panel_top.lv_obj, 320, 80);
    lv_obj_clear_flag(s_home.panel_top.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_home.panel_top.lv_obj, LV_SCROLLBAR_MODE_OFF);
    ui_apply_style(s_home.panel_top.lv_obj, UI_CHILD_PANEL,
                   &s_home.panel_top.style);

    /* ── time label (parent: panel_top) ────────────────── */
    s_home.panel_top.time.lv_obj = lv_label_create(s_home.panel_top.lv_obj);
    lv_obj_set_pos(s_home.panel_top.time.lv_obj, 10, 20);
    lv_obj_set_width(s_home.panel_top.time.lv_obj, 100);
    lv_label_set_long_mode(s_home.panel_top.time.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_home.panel_top.time.lv_obj, s_home.panel_top.time.text);
    ui_apply_style(s_home.panel_top.time.lv_obj, UI_CHILD_LABEL,
                   &s_home.panel_top.time.style);

    /* ── icon_wifi image (parent: panel_top) ───────────── */
    s_home.panel_top.icon_wifi.lv_obj = lv_image_create(s_home.panel_top.lv_obj);
    lv_obj_set_pos(s_home.panel_top.icon_wifi.lv_obj, 270, 16);
    lv_obj_set_size(s_home.panel_top.icon_wifi.lv_obj, 32, 32);
    ui_apply_style(s_home.panel_top.icon_wifi.lv_obj, UI_CHILD_IMAGE,
                   &s_home.panel_top.icon_wifi.style);

    /* ── btn_ok button (parent: screen) ────────────────── */
    s_home.btn_ok.lv_obj = lv_button_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.btn_ok.lv_obj,  100, 380);
    lv_obj_set_size(s_home.btn_ok.lv_obj, 120,  44);
    {
        lv_obj_t *lbl = lv_label_create(s_home.btn_ok.lv_obj);
        lv_label_set_text(lbl, s_home.btn_ok.label_text);
        lv_obj_center(lbl);
    }
    lv_obj_add_event_cb(s_home.btn_ok.lv_obj,
                        ui_home_on_btn_ok, LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_home.btn_ok.lv_obj, UI_CHILD_BUTTON,
                   &s_home.btn_ok.style);

    /* ── brightness_slider (parent: screen) ────────────── */
    s_home.brightness_slider.lv_obj = lv_slider_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.brightness_slider.lv_obj,  40, 200);
    lv_obj_set_size(s_home.brightness_slider.lv_obj, 240,  20);
    lv_slider_set_range(s_home.brightness_slider.lv_obj,
                        s_home.brightness_slider.min,
                        s_home.brightness_slider.max);
    lv_slider_set_value(s_home.brightness_slider.lv_obj,
                        s_home.brightness_slider.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_home.brightness_slider.lv_obj,
                        ui_home_on_brightness_slider,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_home.brightness_slider.lv_obj, UI_CHILD_SLIDER,
                   &s_home.brightness_slider.style);
}
```

**Order rule:** parents always appear before their children in the init
sequence. The generator sorts nodes by depth-first traversal order.

---

## 8. Changes to `ui_defs.h`

### Removed (v0.4.0)

```c
/* REMOVED — no longer needed */
typedef struct { ... } ui_child_t;    /* generic child struct */
typedef struct { ... } ui_screen_t;   /* generic screen struct */
#define UI_MAX_CHILDREN 16            /* moved to ui_config.h, now unused */
```

### Kept / Updated

```c
/* KEPT — used by ui_apply_style() and generated struct field types */
typedef enum {
    UI_CHILD_LABEL,
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
    UI_CHILD_BUTTON,      /* new in v0.4.0 */
    UI_CHILD_SLIDER,      /* new in v0.4.0 */
    UI_CHILD_PANEL,       /* new in v0.4.0 */
    UI_CHILD_DYNAMIC,     /* new in v0.4.0 — dynamic container */
} ui_child_type_t;

/* KEPT — ui_style_t and all sub-structs unchanged */
typedef struct { ... } ui_style_box_t;
typedef struct { ... } ui_style_text_t;
typedef struct { ... } ui_style_effects_t;
typedef struct { ... } ui_style_t;
```

`ui_apply_style(lv_obj_t *obj, ui_child_type_t type, const ui_style_t *s)`
stays in `ui_style.c` / `ui_style.h` with additions for BUTTON, SLIDER, PANEL.

**`ui_config.h`** (auto-generated, in `priv_include/`) removes `UI_MAX_CHILDREN`
since the flat children array no longer exists. It retains:
- `UI_MAX_STRING_LENGTH 30`
- `UI_MAX_ID_LENGTH 30`

---

## 9. Parser Changes (Python)

### v0.3.0 parser

Walked only direct children of a `<Frame>`. Returned a `ParsedScreen` with
a flat `list[ParsedChild]`.

### v0.4.0 parser

Recursively walks the full Figma XML tree. Returns a tree of `ParsedNode`
objects that mirrors the Figma hierarchy.

#### `ParsedNode` (replaces `ParsedChild`)

```python
@dataclass
class ParsedNode:
    widget_type: WidgetType       # enum: LABEL, BAR, IMAGE, BUTTON, etc.
    id: str                       # normalized from Figma node name
    raw_name: str                 # original Figma name
    x: int
    y: int
    w: int
    h: int
    style: ParsedStyle
    text_content: str             # for LABEL and BUTTON (from characters attr
                                  # or first Text child)
    is_dynamic_text: bool         # True → char[], False → const char *
    slider_min: int               # for SLIDER
    slider_max: int               # for SLIDER
    children: list['ParsedNode']  # recursive — empty for leaf widgets
    depth: int                    # depth in tree (screen root = 0)
```

#### `ParsedScreen` (updated)

```python
@dataclass
class ParsedScreen:
    name: str
    snake: str
    children: list[ParsedNode]   # direct children only — recurse via ParsedNode.children
```

#### Tree walker algorithm

```python
def parse_node(xml_node, parent_type, depth) -> ParsedNode | None:

    widget_type = detect_widget_type(xml_node)

    if widget_type is None:
        # Skip with warning
        return None

    if widget_type == WidgetType.STRUCTURAL_FRAME:
        # Drop this node — promote its children to caller's level
        # Return a sentinel and let the caller flatten
        return STRUCTURAL_SENTINEL

    node = ParsedNode(...)
    node.depth = depth

    if depth > MAX_DEPTH:
        emit_warning(f"[UI GEN WARN] depth {depth} at '{node.id}'")

    if widget_type == WidgetType.DYNAMIC_CONTAINER:
        # Stop recursion — no children parsed
        return node

    if widget_type == WidgetType.BUTTON:
        # One-level peek for label text only
        node.text_content = find_button_label(xml_node)
        # Do NOT recurse into full children
        return node

    # Recurse for PANEL and any future container types
    for child_xml in get_children(xml_node):
        child_node = parse_node(child_xml, widget_type, depth + 1)
        if child_node is STRUCTURAL_SENTINEL:
            # Flatten — add structural frame's children to current node
            node.children.extend(flatten_structural(child_xml, depth))
        elif child_node is not None:
            node.children.append(child_node)

    return node
```

#### Widget type detection (detect_widget_type)

Applied in priority order:

```python
def detect_widget_type(node) -> WidgetType | None:
    tag  = node.tag
    name = node.attrib.get("name", "").lower()
    node_type = node.attrib.get("type", "")

    if tag == "Text":
        return WidgetType.LABEL

    if name.startswith(("btn_", "button_")):
        return WidgetType.BUTTON

    if name.startswith("slider_") or name.endswith("_slider"):
        return WidgetType.SLIDER

    if name.startswith(("list_", "grid_")):
        return WidgetType.DYNAMIC_CONTAINER

    if "bar" in name:
        return WidgetType.BAR

    if "icon" in name or "image" in name:
        return WidgetType.IMAGE

    has_children = get_children(node) is not None
    has_visual   = has_fill(node) or has_border(node) or has_radius(node)
    has_name     = not is_auto_named(node)   # "Frame 12" is auto-named

    if has_children:
        if has_visual or has_name:
            return WidgetType.PANEL       # container — recurse
        else:
            return WidgetType.STRUCTURAL  # invisible grouping — drop+promote

    # Leaf node with no type match
    logger.warning("Skipping '%s' — no widget type matched", name)
    return None
```

---

## 10. Generator Changes (Python)

The generator walks the `ParsedNode` tree and produces a screen-specific C
struct + flat init sequence. Two passes:

### Pass 1 — Struct definition

Recursively emit nested struct fields from the `ParsedNode` tree. Depth-first.
Each `ParsedNode` becomes one nested struct block with `lv_obj_t *lv_obj`,
`ui_style_t style`, widget-specific data fields, and child struct blocks.

```python
def emit_struct_fields(node: ParsedNode, indent: int) -> str:
    fields = []
    fields.append(f"lv_obj_t *lv_obj;")
    fields.append(f"ui_style_t style;")
    fields.extend(widget_data_fields(node))  # type-specific

    for child in node.children:
        child_block = emit_struct_fields(child, indent + 1)
        fields.append(f"struct {{ {child_block} }} {child.id};")

    return "\n".join(fields)
```

### Pass 2 — Init sequence

Iterates the tree in BFS (breadth-first) order so parents always appear before
children. Emits one flat `lv_obj_create()` / widget-specific create call per
node. Uses the field path to reference the lv_obj_t of the parent.

```python
def emit_init_calls(screen: ParsedScreen) -> str:
    calls = []
    queue = [(node, "s_" + screen.snake + ".lv_screen")
             for node in screen.children]

    while queue:
        node, parent_lv_obj = queue.pop(0)
        field_path = build_field_path(node)   # e.g. "s_home.panel_top.time"
        calls.append(emit_create_call(node, parent_lv_obj, field_path))
        for child in node.children:
            child_parent = f"{field_path}.lv_obj"
            queue.append((child, child_parent))

    return "\n\n".join(calls)
```

### Setter generation

For each `ParsedNode` where a setter is appropriate (LABEL with
`is_dynamic_text=True`, IMAGE, BAR, BUTTON, SLIDER, DYNAMIC_CONTAINER):
- Build the function name from the path
- Emit the setter body referencing `field_path.lv_obj`

No setter is generated for PANEL or static-text LABELs.

### Callback generation

For each BUTTON and SLIDER node: emit one weak callback declaration in the
`.c` file.

---

## 11. `ui_apply_style()` — Updated for New Widget Types

Add handling for `UI_CHILD_BUTTON`, `UI_CHILD_SLIDER`, `UI_CHILD_PANEL`
in `ui_style.c`:

```c
case UI_CHILD_BUTTON:
    /* box styles apply to the button body */
    /* text styles (color, size) apply to LV_PART_MAIN for text */
    /* Note: button's internal label inherits from button's text style */
    break;

case UI_CHILD_SLIDER:
    /* box.bg → LV_PART_MAIN (track background) */
    /* box.bg also → LV_PART_INDICATOR (the filled portion) */
    /* same pattern as BAR */
    break;

case UI_CHILD_PANEL:
    /* only box styles — no text, no indicator */
    break;
```

---

## 12. Nesting Depth Limits

| Limit | Value | Action |
|-------|-------|--------|
| `MAX_DEPTH` | 5 | Parser warning emitted |
| Hard stop | 7 | Parser error, screen skipped |
| API name segments | 4 | Truncate middle segments, emit warning |

Configurable via a future `--max-depth N` CLI flag.

---

## 13. Files Changed vs v0.3.0

### Modified

| File | Change |
|------|--------|
| `core/figma_parser.py` | Tree walker, `ParsedNode` replaces `ParsedChild`, detect_widget_type expanded |
| `core/generator.py` | Full rewrite: two-pass (struct + init), BFS ordering, setter logic |
| `core/widget_type.py` | Add `BUTTON`, `SLIDER`, `PANEL`, `DYNAMIC_CONTAINER`, `STRUCTURAL` |
| `core/child_registry.py` | Updated `ChildSpec` entries for new types |
| `core/generic_child.py` | Updated `ChildSpec` with button/slider-specific fields |
| `core/emit/layouts.py` | New C/H layout templates for hierarchical struct output |
| `static_src/ui_defs.h` | Remove `ui_child_t`, `ui_screen_t`; add `UI_CHILD_BUTTON` etc. to enum |
| `static_src/ui_style.c` | Add BUTTON, SLIDER, PANEL cases to `ui_apply_style()` |
| `core/config_writer.py` | Remove `UI_MAX_CHILDREN` (flat array gone) |
| `tests/test_parser.py` | Update for tree walker, `ParsedNode` |
| `tests/test_generator.py` | Update golden files for new output format |

### Removed

| File | Reason |
|------|--------|
| `core/templates/*.py` | Template strings replaced by struct-aware code emitters in generator.py |
| `core/utils/template_loader.py` | No longer needed |

### New

| File | Purpose |
|------|---------|
| `core/node_emitter.py` | Emits C struct field block for one ParsedNode (recursive) |
| `core/init_emitter.py` | Emits flat init call sequence from BFS tree walk |
| `core/setter_emitter.py` | Emits setter + callback functions |

---

## 14. What Does NOT Change

- CLI interface and all flags (`-x`, `-i`, `-d`, `--yes`, `--lvgl-tool`, `-f`, etc.)
- `config_writer.py` (writes `ui_config.h`) — remove `UI_MAX_CHILDREN` only
- `tools/image_converter.py` (PNG→LVGL C arrays) — unchanged
- `main.py` orchestration — unchanged except calling updated generator
- `ui_style.h` public API — `ui_apply_style()` signature unchanged
- Test infrastructure (`pytest`, `regen_golden.py`) — updated golden files only
- `pyproject.toml`, packaging — unchanged

---

## 15. Known Limitations (v0.4.0 Scope)

These are intentionally deferred to a later version:

| Limitation | Reason deferred |
|-----------|----------------|
| Screen navigation system | Requires inter-screen coordination, screen stack, transition API |
| Auto Layout → LVGL flex | LVGL flex is complex; embedded UIs rarely need it |
| Font family support | Requires custom font pipeline separate from Montserrat |
| Gradients, shadows | No direct LVGL equivalent for embedded targets |
| Scrolling containers | Requires scroll mode flags; defer to when list/grid is mature |
| Component variant mapping | Figma variants → LVGL state (pressed, disabled, checked) |
