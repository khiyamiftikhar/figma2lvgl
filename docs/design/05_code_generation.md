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
4. Iterates unique widget types (ordered, deterministic) to generate callbacks and init cases
5. Generates a per-screen bar range comment if any bar widgets are present
6. Assembles everything into the file-level C and H layout templates

---

## Identifier Derivation

From `screen.snake` (computed by the parser via `to_snake_case(name)`):

| Derived name | Pattern | Example |
|-------------|---------|---------|
| C variable | `{snake}` | `ili9486_home` |
| `.c` filename | `ui_{snake}.c` | `ui_ili9486_home.c` |
| `.h` filename | `ui_{snake}.h` | `ui_ili9486_home.h` |
| Include guard | `UI_{SNAKE}_H` | `UI_ILI9486_HOME_H` |
| Init function | `ui_{snake}_init` | `ui_ili9486_home_init` |
| Load function | `ui_{snake}_load` | `ui_ili9486_home_load` |

---

## Screen Struct Generation

The static `ui_screen_t` initialiser is built by iterating `screen.children`:

```c
ui_screen_t ili9486_home = {
    .name        = "ili9486_home",
    .child_count = 4,
    .children    = {
        {
            .type   = UI_CHILD_LABEL,
            .id     = "time",
            .lv_obj = NULL,
            .x = 94, .y = 79, .w = 133, .h = 34,
            .style = {
                .text = {
                    .has_color = true, .color = 0x000000,
                    .has_size  = true, .size  = 12,
                }
            },
            .data.label = { .text = "Time is \n15:00" }
        },
        ...
    },
    .lv_screen = NULL
};
```

Key points:
- `child.type.c_enum_name()` produces the C enum literal (e.g. `UI_CHILD_LABEL`)
- `child.text_content` is already sanitized by `sanitize_c_string()` — embedded newlines are `\n`, quotes are `\"`
- `data.label.text` holds the **design-time default** from Figma. `_init()` applies it on first render. See [Label Text Lifecycle](#label-text-lifecycle) below.
- `data.image.src` is always `NULL` in the struct; the image setter fills it at runtime
- `data.bar.value` is always `0` in the struct

Each `data_block` is selected by `WidgetType` enum comparison — no string comparisons in the generator.

---

## Style Block Rendering

`_render_style_block(style)` converts a `ParsedStyle` into a C struct initialiser fragment. Uses direct dict lookups (`_ALIGN_MAP`) — not `Template` substitution. Not affected by the `substitute()` migration.

If `style.is_empty()`:
```c
.style = { .box = { 0 }, .text = { 0 }, .effects = { 0 } },
```

Otherwise only sub-structs with at least one field set are emitted:
```c
.style = {
    .text = {
        .has_color = true,
        .color     = 0x000000,
        .has_size  = true,
        .size      = 12
    }
},
```

Text alignment mapping (`_ALIGN_MAP`):

| Python | C |
|--------|---|
| `"LEFT"` | `LV_TEXT_ALIGN_LEFT` |
| `"CENTER"` | `LV_TEXT_ALIGN_CENTER` |
| `"RIGHT"` | `LV_TEXT_ALIGN_RIGHT` |

Note: `ParsedStyleText.align` is currently always `None` since FigML does not export horizontal text alignment. This dict exists for future use.

---

## Per-Child Loop — Setters

For each child in `screen.children` (by index):

1. Looks up `CHILDREN[child.type]` for the `ChildSpec`
2. Derives `fn_name` via `spec.derive_setter_name(screen_snake, child.id)`
3. Derives `cb_name` via `spec.derive_callback_name(screen_snake)`
4. Loads the setter template via `load_template(spec.setter_template)`
5. Substitutes variables using `string.Template.substitute()` — raises `KeyError` immediately on any missing variable
6. Appends setter body to `setters` list
7. Appends setter prototype to `setter_prototypes` list (for the `.h` file)

**Naming (derived from ChildSpec patterns):**

| Type | Pattern | Example |
|------|---------|---------|
| `LABEL` | `ui_{screen}_set_{child_id}` | `ui_ili9486_home_set_time` |
| `IMAGE` | `ui_{screen}_display_{child_id}` | `ui_ili9486_home_display_icon_wifi` |
| `BAR` | `ui_{screen}_set_{child_id}` | `ui_ili9486_home_set_bar` |

There are no `if/elif` branches per widget type in this loop. `derive_setter_name()` and `derive_callback_name()` handle all naming via the pattern strings in `ChildSpec`.

---

## Per-Type Loop — Callbacks and Init Cases

Runs over `unique_types` — an **ordered list** of widget type enum values that appear in the screen's children. The list preserves first-appearance order, making generated C output deterministic across runs.

For each unique type:
1. Derives `cb_name` via `spec.derive_callback_name(screen_snake)`
2. Loads and substitutes the callback template → appends to `job_callbacks` (only if non-empty)
3. Loads and substitutes the init template → appends to `init_cases`

This guarantees: one `case UI_CHILD_*:` block per type, one callback function per type (not per instance).

---

## Bar Range Comment

After the child loop, if any `WidgetType.BAR` children exist in the screen, a comment is generated that appears once above `_init()` in the C file:

```c
/* TODO: Bar range is hardcoded to 0-100 in ui_ili9486_home_init() below.
 * Adjust lv_bar_set_range() for: bar
 * If all bars share a range, consider making it a parameter. */
void ui_ili9486_home_init(void) { ... }
```

This is actionable — it names the specific bar IDs to look at. Screens with no bar widgets produce no comment.

---

## Template Substitution

All template variable substitution uses `string.Template.substitute()` — not `safe_substitute()`. If a template references `${some_variable}` and it is not provided by the generator, a `KeyError` is raised immediately at generation time. This is intentional: a clear Python error at generation time is better than a `${some_variable}` literal appearing in the generated C file (which would fail at compile time with a confusing error).

---

## File Assembly

### C file

Built from `C_FILE_LAYOUT` in `core/emit/layouts.py` using `Template.substitute()`:

| Variable | Content |
|----------|---------|
| `${header_filename}` | e.g. `"ui_ili9486_home.h"` |
| `${screen_struct}` | Full `ui_screen_t` initialiser |
| `${job_callbacks}` | Joined callback function bodies |
| `${setters}` | Joined setter function bodies |
| `${bars_comment}` | Bar range TODO comment, or empty string |
| `${sc_fn_name}` | e.g. `ui_ili9486_home_load` |
| `${init_fn}` | e.g. `ui_ili9486_home_init` |
| `${screen_var}` | e.g. `ili9486_home` |
| `${init_body}` | Joined switch case blocks |

### H file

Built from `H_FILE_LAYOUT`:

| Variable | Content |
|----------|---------|
| `${guard}` | e.g. `UI_ILI9486_HOME_H` |
| `${init_fn}` | e.g. `ui_ili9486_home_init` |
| `${sc_fn_name}` | e.g. `ui_ili9486_home_load` |
| `${setter_prototypes}` | Newline-joined prototype declarations |

---

## Generated File Structure

### `.c` file sections (in order)

1. `#include` — own header, `assets.h`, `ui_defs.h`, `ui_style.h`, `<stdio.h>`
2. Screen struct (`ui_screen_t` static initialiser)
3. Job callbacks (static animation exec callbacks, one per type that needs one)
4. Setters (one per widget instance)
5. `ui_{screen}_load()` — calls `lv_scr_load()`
6. Bar range comment (if any bars present)
7. `ui_{screen}_init()` — creates `lv_screen`, iterates children via `switch`, calls `ui_apply_style()`

### `.h` file sections (in order)

1. Include guard + `extern "C"`
2. `#include <stdint.h>`
3. `void ui_{screen}_init(void);`
4. `void ui_{screen}_load(void);`
5. Setter prototypes (one per widget instance)
6. Close `extern "C"` + `#endif`

---

## Label Text Lifecycle

The generated code for labels has two distinct phases:

| Phase | Mechanism | Who controls it |
|-------|-----------|----------------|
| Initial render | `data.label.text` baked from Figma `characters` at generation time; `_init()` applies it via `lv_label_set_text(c->lv_obj, c->data.label.text)` | Designer (via Figma) |
| Runtime updates | Generated setter `ui_{screen}_set_{id}(const char *text)` calls `lv_label_set_text()` on the live object | Firmware developer |

`data.label.text` holds the design-time default only. The setter does not update it — it writes directly to the LVGL object. If `_init()` is called again (e.g. on screen reload), the label reverts to the Figma value. This is intentional: the Figma value is the known-good initial state; the firmware has full control after that.

---

## Style Application at Runtime

At the end of `ui_{screen}_init()`, for every child:

```c
if (c->lv_obj)
    ui_apply_style(c->lv_obj, c->type, &c->style);
```

`ui_apply_style()` is in the static `ui_style.c`. See `08_static_runtime.md` for full details.
