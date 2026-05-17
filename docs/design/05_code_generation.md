# figma2lvgl — Code Generation

Code generation is a three-pass process orchestrated by `core/generator.py`. Each pass is handled by a dedicated module. The public entry point is `generate_screen(screen)`.

---

## `generate_screen(screen)` — Overview

**Input:** one `ParsedScreen`

**Output:** `(c_filename, h_filename, h_text, c_text)` — four strings

```python
def generate_screen(screen: ParsedScreen):
    struct_block = emit_screen_struct_type(screen)    # Pass 1
    init_body    = emit_init_body(screen)             # Pass 2
    sc           = collect_setters_and_callbacks(screen)  # Pass 3
    # assemble into C_LAYOUT / H_LAYOUT
    return c_fname, h_fname, h_text, c_text
```

---

## Widget Name Parsing — Modifiers Stripped Before ID Formation

Before any struct field name or API function name is formed, `parse_widget_name()` strips behavioral modifier suffixes from the raw Figma node name:

```python
"btn_ok_lp"              → base "btn_ok",          event_modifiers ["lp"]
"brightness_slider_0_255"→ base "brightness_slider", range (0, 255)
"btn_cancel"             → base "btn_cancel",       event_modifiers []
```

The base name goes to `normalize_id()` and becomes the C struct field. The modifiers drive code generation (event registrations, range values) but never appear in generated names. This separation is enforced in the parser — the generator only ever sees the clean base name.

---

## Pass 1 — Struct Emitter (`core/node_emitter.py`)

Produces the file-static C struct that mirrors the Figma hierarchy.

### Struct type definition

`emit_struct_fields(node, indent)` — recursive. For each `ParsedNode`:
1. Emits `lv_obj_t *lv_obj;` and `ui_style_t style;`
2. Emits widget-specific data fields (e.g. `char text[30]` for LABEL)
3. For PANEL: recurses into children, emitting nested `struct { ... } child_id;` blocks

`emit_screen_struct_type(screen)` — wraps the above into `static struct { lv_obj_t *lv_screen; [child blocks] } s_{snake} = { [initializer] };`

### Static initializer

`emit_node_initializer(node, indent)` — recursive. Always emits `.style` first, then widget-specific fields:
- LABEL with text → `.text = "Hello",`
- BUTTON → `.label_text = "Ok",`
- SLIDER → `.value = 50, .min = 0, .max = 100,`
- PANEL → recurses into children

The result is a clean designated initializer. `lv_obj_t *` pointers are always `NULL` at compile time (they're not in the initializer; C zero-initializes them).

### Style rendering

`render_style_init(style, indent)` in `node_emitter.py` converts a `ParsedStyle` to a C struct initializer fragment. Empty styles emit `.style = { .text = {0}, .box = {0} }`. Non-empty styles emit only the sub-structs and fields that are set.

---

## Pass 2 — Init Emitter (`core/init_emitter.py`)

Produces the flat `_init()` function body.

`emit_init_body(screen)` runs a **BFS traversal** of the screen's children. For each node, it emits the LVGL creation and configuration calls.

**BFS ordering ensures parents are always created before children.** The generated `_init()` is a flat sequence of calls — no C recursion, no stack depth from nesting.

Queue entry format: `(node, struct_path, parent_lv_obj_expr)`

- `struct_path` — e.g. `"s_home.panel_top.time"` — used to reference the node's fields
- `parent_lv_obj_expr` — e.g. `"s_home.panel_top.lv_obj"` — passed to `lv_*_create()`

**Button init — two-target style application:**

```c
/* Button container: box styles only */
s_home.btn_ok.lv_obj = lv_button_create(parent);
lv_obj_set_pos(...);
lv_obj_set_size(...);
{
    lv_obj_t *_lbl = lv_label_create(s_home.btn_ok.lv_obj);
    lv_label_set_text(_lbl, s_home.btn_ok.label_text);
    lv_obj_center(_lbl);
    /* text color/font applied directly to label — not via inheritance */
    ui_apply_style(_lbl, UI_CHILD_LABEL, &s_home.btn_ok.style);
}
lv_obj_add_event_cb(..., ui_home_on_btn_ok_clicked, LV_EVENT_CLICKED, NULL);
/* btn_ok_lp also registers: */
lv_obj_add_event_cb(..., ui_home_on_btn_ok_long_pressed, LV_EVENT_LONG_PRESSED, NULL);
ui_apply_style(s_home.btn_ok.lv_obj, UI_CHILD_BUTTON, &s_home.btn_ok.style);
```

The same `ui_style_t` struct is passed to both `ui_apply_style` calls.
`UI_CHILD_BUTTON` applies only box styles (bg, radius, border).
`UI_CHILD_LABEL` applies only text styles (color, font).
This avoids relying on LVGL's style inheritance, which is brittle when a
button has mixed children (icon + label).

`_emit_node_init(node, path, parent_lv)` handles each widget type. At the end of each node's init block, `ui_apply_style()` is called.

---

## Pass 3 — Setter/Callback Emitter (`core/setter_emitter.py`)

`collect_setters_and_callbacks(screen)` does a BFS walk and for each addressable node emits:

**Setters generated for:**
- LABEL → `void ui_{screen}_{path}_set_text(const char *text)`
- IMAGE → `void ui_{screen}_{path}_display(void)`
- BAR → `void ui_{screen}_{path}_set_value(int value, uint32_t duration_ms)`
- BUTTON → `void ui_{screen}_{path}_{id}_set_label(const char *text)`
- SLIDER → `void ui_{screen}_{path}_{id}_set_value(int32_t value)`
- DYNAMIC → `lv_obj_t *ui_{screen}_get_{id}(void)`

**No setter for:** PANEL (not addressable from firmware), STRUCTURAL (dropped).

**Callbacks generated for:** BUTTON and SLIDER. One callback declaration per registered event, named with the event type:

```c
/* Declared in .h — you must implement these in your application .c */
void ui_home_on_btn_ok_clicked(lv_event_t *e);
void ui_home_on_btn_ok_long_pressed(lv_event_t *e);   /* only if btn_ok_lp in Figma */
```

The generator registers them with `lv_obj_add_event_cb()` in `_init()` but emits **no stub definition**. If the application does not implement a declared callback, the linker reports an undefined reference. This is intentional — missing handlers are compile-time errors, not silent no-ops.

Only the events explicitly requested via Figma name suffix are registered. Zero overhead for events that are not needed.

> **Design decision record — why not `__attribute__((weak))`:**
> Weak stub definitions were used in v0.4.2 and earlier. The pattern is clean
> on GCC/Clang (user defines the same name, linker picks their version), but
> `__attribute__((weak))` is not supported on MSVC. The Visual Studio LVGL
> simulator is the primary development/testing environment, so MSVC
> compatibility is a hard requirement. A `#if defined(__GNUC__) || defined(__clang__)`
> guard compiles cleanly on MSVC but loses the override semantics entirely —
> the stub body is compiled in on GCC/Clang where it's overridable, but on
> MSVC the symbol is simply undefined, which gives a linker error anyway.
> The linker-error model is consistent across all compilers and makes the
> contract explicit: every declared callback must be implemented.
> If a future revision adds a portable no-op mechanism (e.g. a generated
> `ui_home_callbacks.c` stub file the user edits), that would restore the
> "silent no-op until implemented" behaviour without compiler-specific attributes.

Returns a dict: `setters`, `callbacks`, `prototypes`, `cb_declarations`, `bar_anim_needed`.

If `bar_anim_needed` is True, a `_bar_anim_exec_cb` helper is added to the C file.

---

## API Naming Convention

Function names encode the path from screen to widget:

```
ui_{screen}_{path}_set_text
ui_{screen}_{path}_display
ui_{screen}_{path}_set_value
ui_{screen}_on_{widget_id}     ← callbacks use widget_id only (screen root)
ui_{screen}_get_{widget_id}    ← dynamic container accessor
```

`_setter_fn_name(screen_snake, path_parts)` caps path at 3 segments to prevent name explosion. The last segment (widget ID) is never truncated.

---

## File Assembly

### C file sections (in order)

1. `#include` — own header, `assets.h`, `ui_defs.h`, `ui_style.h`, `<stdio.h>`
2. File-static screen struct + initializer
3. Bar animation helper (if any bars)
4. Setter functions
5. `ui_{screen}_load()` — calls `lv_scr_load()`
6. `ui_{screen}_init()` — BFS flat sequence of LVGL create + configure calls + `lv_obj_add_event_cb()` registrations

### H file sections (in order)

1. Include guard + `extern "C"`
2. `#include <stdint.h>`, `#include "lvgl.h"`
3. `void ui_{screen}_init(void);`
4. `void ui_{screen}_load(void);`
5. Setter prototypes (one per addressable widget)
6. Event callback declarations (one per interactive widget)
7. Close `extern "C"` + `#endif`

---

## Label Text Lifecycle

| Phase | Mechanism | Who controls |
|-------|-----------|-------------|
| Initial render | `data.text` baked from Figma `characters` attr; `_init()` calls `lv_label_set_text()` | Designer (via Figma) |
| Runtime update | Generated setter calls `lv_label_set_text()` on the live object | Firmware |

`data.text` holds the design-time default only. On screen reload (`_init()` re-called), the label reverts to the Figma value. This is intentional — Figma value is the known-good initial state.

---

## Generated File Example

For a screen named `home` with a `panel_top` container containing a `time` label and a `btn_ok` button:

```
ui_src/
  src/     ui_home.c
  include/ ui_home.h
```

Firmware usage:
```c
#include "ui_home.h"

/* Implement declared callbacks in your application .c.
   Linker error if missing — implement a no-op if not needed yet. */
void ui_home_on_btn_ok_clicked(lv_event_t *e) {
    ui_settings_load();
}

void app_start(void) {
    ui_home_init();
    ui_home_load();
    ui_home_panel_top_time_set_text("17:45");
}
```
