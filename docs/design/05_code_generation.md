# figma2lvgl — Code Generation

Code generation is handled entirely by `core/generator.py`. The public entry point is `generate_screen(screen)`, which takes a `ParsedScreen` and returns the full text of two files: the screen's `.c` file and its `.h` file.

---

## `generate_screen(screen)` — Overview

**Input:** one `ParsedScreen`

**Output:** `(c_filename, h_filename, h_text, c_text)` — four strings

The function:
1. Derives C identifiers from the screen name
2. Builds the screen struct initialiser (the static `ui_screen_t`)
3. Iterates children (per-instance) to generate setter functions
4. Iterates unique widget types (per-type) to generate callbacks and init cases
5. Assembles everything into the file-level C and H layout templates

---

## Identifier Derivation

From `screen.snake` (already computed by the parser):

| Derived name | Pattern | Example |
|-------------|---------|---------|
| C variable name | `{snake}` | `home_screen` |
| `.c` filename | `ui_{snake}.c` | `ui_home_screen.c` |
| `.h` filename | `ui_{snake}.h` | `ui_home_screen.h` |
| Include guard | `UI_{SNAKE}_H` | `UI_HOME_SCREEN_H` |
| Init function | `ui_{snake}_init` | `ui_home_screen_init` |
| Load function | `ui_{snake}_load` | `ui_home_screen_load` |
| Load callback | `ui_{snake}_load_job` | `ui_home_screen_load_job` (in C layout, unused at runtime currently) |

---

## Screen Struct Generation

The static `ui_screen_t` initialiser is built by iterating `screen.children` and producing one struct literal per child:

```c
ui_screen_t home_screen = {
    .name        = "HomeScreen",
    .child_count = 3,
    .children    = {
        {
            .type   = UI_CHILD_LABEL,
            .id     = "time_label",
            .lv_obj = NULL,
            .x = 10, .y = 20, .w = 100, .h = 30,
            .style = {
                .text = {
                    .has_color = true,
                    .color     = 0xFFFFFF,
                    .has_size  = true,
                    .size      = 16,
                    .has_align = true,
                    .align     = LV_TEXT_ALIGN_CENTER,
                }
            },
            .data.label = { .text = "" }
        },
        ...
    },
    .lv_screen = NULL
};
```

Each child's data union is pre-initialised with defaults:
- `UI_CHILD_LABEL` → `.data.label = { .text = "" }`
- `UI_CHILD_IMAGE` → `.data.image = { .src = NULL }`
- `UI_CHILD_BAR` → `.data.bar = { .value = 0 }`

---

## Style Block Rendering

`_render_style_block(style, indent)` converts a `ParsedStyle` into a C struct initialiser fragment.

If `style.is_empty()` is `True`:
```c
.style = { .box = { 0 }, .text = { 0 }, .effects = { 0 } },
```

Otherwise, only sub-structs that have at least one field set are emitted. Each field is preceded by its `has_*` boolean:

```c
.style = {
    .box = {
        .has_bg = true,
        .bg = 0x4CAF50,
        .has_radius = true,
        .radius = 4
    },
    .text = {
        .has_color = true,
        .color = 0xFFFFFF
    },
},
```

Text alignment strings are mapped via `_ALIGN_MAP`:

| Python value | C value |
|-------------|---------|
| `"LEFT"` | `LV_TEXT_ALIGN_LEFT` |
| `"CENTER"` | `LV_TEXT_ALIGN_CENTER` |
| `"RIGHT"` | `LV_TEXT_ALIGN_RIGHT` |

---

## Per-Child Loop — Setters

For each child in `screen.children` (by index):

1. Looks up `CHILDREN[child.type]` for the `ChildSpec`
2. Determines `fn_name` (setter function name) and `cb_name` (callback name, used in the bar setter to reference the exec callback)
3. Loads the setter template via `load_template(spec.setter_template)`
4. Substitutes variables using `string.Template.safe_substitute()`
5. Appends the rendered setter to the setters list
6. Appends the setter prototype to `setter_prototypes` list (for the `.h` file)

**Setter naming:**

| Type | Setter function name |
|------|---------------------|
| `UI_CHILD_LABEL` | `ui_{screen}_set_{child_id}` |
| `UI_CHILD_IMAGE` | `ui_{screen}_display_{child_id}` |
| `UI_CHILD_BAR` | `ui_{screen}_set_{child_id}` |

---

## Per-Type Loop — Callbacks and Init Cases

Runs over `unique_types` — the set of widget type strings that actually appear in the screen's children.

For each unique type:
1. Loads callback template → substitutes → appends to `job_callbacks`
2. Loads init template → substitutes → appends to `init_cases`

This guarantees:
- One `case UI_CHILD_*:` block per type in the `switch`, regardless of how many instances exist
- One callback function per type (not per instance)

---

## Template Variable Reference

Variables available during substitution (via `string.Template.safe_substitute`):

| Variable | Available in | Description |
|----------|-------------|-------------|
| `${fn_name}` | setter | Full setter function name |
| `${child_index}` | setter | Index of the child in `screen.children[]` array |
| `${screen_var}` | setter, callback, init | C variable name of the screen struct |
| `${child_id}` | setter | Normalized child ID string |
| `${cb_name}` | setter, callback | Callback function name (empty for types with no callback) |

---

## File Assembly

### C file

Built from `C_FILE_LAYOUT` in `core/emit/layouts.py` using `string.Template.safe_substitute()`:

```
${header_filename}     → "ui_home_screen.h"
${screen_struct}       → the full ui_screen_t initialiser
${job_callbacks}       → joined callback code blocks
${setters}             → joined setter function bodies
${sc_fn_cb_name}       → "ui_home_screen_load_job" (in layout but unused currently)
${sc_fn_name}          → "ui_home_screen_load"
${init_fn}             → "ui_home_screen_init"
${screen_var}          → "home_screen"
${init_body}           → joined switch case blocks
```

### H file

Built from `H_FILE_LAYOUT`:

```
${guard}               → "UI_HOME_SCREEN_H"
${init_fn}             → "ui_home_screen_init"
${sc_fn_name}          → "ui_home_screen_load"
${setter_prototypes}   → newline-joined prototype declarations
```

---

## Generated File Structure

### `.c` file sections (in order)

1. `#include` — own header, `assets.h`, `ui_defs.h`, `ui_style.h`, `<stdio.h>`
2. Screen struct (`ui_screen_t` static initialiser)
3. Commented-out job struct section (placeholder)
4. Job callbacks (static animation exec callbacks, one per type that needs one)
5. Setters (one per widget instance)
6. `ui_{screen}_load()` — calls `lv_scr_load()`
7. `ui_{screen}_init()` — creates `lv_obj`, iterates children via `switch`, calls `ui_apply_style()`

### `.h` file sections (in order)

1. Include guard + `extern "C"`
2. `#include <stdint.h>`
3. `void ui_{screen}_init(void);`
4. `void ui_{screen}_load(void);`
5. Setter prototypes (one per widget instance)
6. Close `extern "C"` + `#endif`

---

## Style Application at Runtime

At the end of `ui_{screen}_init()`, for every child whose `lv_obj` was successfully created:

```c
if (c->lv_obj)
    ui_apply_style(c->lv_obj, c->type, &c->style);
```

`ui_apply_style()` is defined in the static `ui_style.c`. It reads the `has_*` flags in the style struct and makes the corresponding LVGL API calls. See `08_static_runtime.md` for full details.
