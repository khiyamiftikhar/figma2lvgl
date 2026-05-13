# figma2lvgl — Architecture

## High-Level Pipeline

```
Figma XML + PNG assets
        │
        ▼
┌───────────────────┐
│   CLI / main.py   │  Argument parsing, validation, orchestration
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│   figma_parser    │  XML → ParsedScreen / ParsedChild / ParsedStyle
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│    generator      │  ParsedScreen → C/H text via templates
└────────┬──────────┘
         │
         ├─── templates/     (per-widget code blocks)
         ├─── emit/layouts   (file-level C/H scaffolding)
         └─── child_registry (WidgetType → ChildSpec lookup)
         │
         ▼
┌───────────────────┐    ┌──────────────────────┐
│  image_converter  │    │   config_writer.py   │
│  (importable fn)  │    │   → ui_config.h      │
└────────┬──────────┘    └──────────┬───────────┘
         │                          │
         ▼                          │
┌───────────────────┐               │
│   static_src/     │               │
│  ui_defs.h        │               │
│  ui_style.c/h     │               │
└────────┬──────────┘               │
         └──────────────────────────┘
                      ▼
                    ui_src/
                      src/          ← screen .c files
                      include/      ← screen .h files
                      priv_src/     ← image .c files + ui_style.c
                      priv_include/ ← assets.h, ui_defs.h, ui_style.h,
                                      ui_config.h (generated)
```

---

## Component Map

### `figma2lvgl/main.py` — Orchestrator

The single entry point. Owns the full pipeline:

1. Parses CLI arguments (including `--yes`, `--lvgl-tool`, `--color-format`, `--patch-esp-includes`, `--verbose`)
2. Validates XML file and images directory exist
3. Parses all `<Frame>` nodes into `ParsedScreen` objects
4. Validates required image assets (PNG files) are present
5. Resolves `LVGLImage.py` via `--lvgl-tool` override, platform cache, or interactive download
6. Prompts the user if `ui_src/` has existing content (skipped with `--yes`)
7. Resets output directories
8. Runs image conversion by calling `convert_images()` directly — no subprocess
9. Optionally patches LVGL include guards (`--patch-esp-includes` only; off by default)
10. Copies static sources (`ui_defs.h`, `ui_style.c`, `ui_style.h`)
11. Generates `ui_config.h` with auto-computed `UI_MAX_CHILDREN`
12. Calls `generate_screen()` for each screen and writes `.c`/`.h` files

---

### `core/widget_type.py` — Widget Type Enum

Defines `WidgetType`, a Python `Enum` replacing bare string identifiers like `"UI_CHILD_LABEL"`.

```python
class WidgetType(Enum):
    LABEL = "UI_CHILD_LABEL"
    IMAGE = "UI_CHILD_IMAGE"
    BAR   = "UI_CHILD_BAR"
```

A typo (e.g. `WidgetType.LABLE`) raises `AttributeError` at import time rather than returning silent `None` from a dict lookup. `c_enum_name()` returns the string value for use in generated C code.

---

### `core/figma_parser.py` — Parser

Owns all XML-reading logic. Produces Python data objects; knows nothing about C code.

- `parse_screen(frame_node)` — walks a `<Frame>` node, returns a `ParsedScreen`
- `parse_style(node, child_type)` — extracts fill, stroke, radius, font, opacity. `child_type` is a `WidgetType` enum value; the `is_text` routing check compares against `WidgetType.LABEL`
- Text content from the `characters` XML attribute; sanitized via `sanitize_c_string()` to escape control characters
- Unrecognized nodes → warning with parent screen name → skip. No silent fallback to LABEL.
- Duplicate child ID detection (raises `ValueError`)

---

### `core/generator.py` — Code Generator

- `generate_screen(screen)` → `(c_filename, h_filename, h_text, c_text)`
- `_render_style_block(style)` — `ParsedStyle` → C struct initialiser (uses direct dict lookups, not Template substitution)
- Uses `string.Template.substitute()` — raises `KeyError` immediately on missing variables
- Naming derived from `ChildSpec` patterns (`derive_setter_name()`, `derive_callback_name()`) — no per-type `if/elif` branches
- Unique widget types tracked in an ordered list for deterministic init case ordering

---

### `core/child_registry.py` — Widget Type Registry

```python
CHILDREN = {
    WidgetType.LABEL: ChildSpec(...),
    WidgetType.IMAGE: ChildSpec(...),
    WidgetType.BAR:   ChildSpec(...),
}
```

The extension point for new widget types. Adding a new type requires no changes to `generator.py`.

---

### `core/generic_child.py` — ChildSpec

Holds template names, metadata, and naming patterns for one widget type.

| Field | Purpose |
|-------|---------|
| `type_name` | `WidgetType` enum value |
| `callback_template` | Template name for animation callback (`""` = none) |
| `setter_template` | Template name for the public setter |
| `init_template` | Template name for the `switch` init case |
| `setter_args` | C argument signature for the setter |
| `requires_asset` | If `True`, main.py validates a matching PNG exists |
| `setter_name_pattern` | f-string pattern for setter name (e.g. `"ui_{screen}_set_{child_id}"`) |
| `callback_name_pattern` | f-string pattern for callback name (`""` = none) |

---

### `core/config_writer.py` — Config Generator

`write_ui_config(screens, out_dir)` computes `UI_MAX_CHILDREN` as `max(len(s.children) for s in screens)` and writes `ui_config.h` into `priv_include/`. Always exactly what the project requires — no hardcoded cap.

---

### `core/templates/` — Per-Widget Code Templates

One file per widget type. Each defines `*_CALLBACK`, `*_SETTER`, `*_INIT` string constants. Variables use `${variable_name}` syntax. `LABEL_INIT` calls `lv_label_set_text(c->lv_obj, c->data.label.text)` to apply the Figma text on first render.

---

### `core/emit/layouts.py` — File-Level Templates

`C_FILE_LAYOUT` and `H_FILE_LAYOUT`. Includes `${bars_comment}` — a per-screen comment above `_init()` listing bar widget IDs with a reminder to adjust `lv_bar_set_range()`.

---

### `core/utils/figma_helpers.py` — Type Detection

`map_tag_to_child_type(node)` → `WidgetType | None`:

- `Text` tag → `WidgetType.LABEL`
- Name contains `bar` → `WidgetType.BAR`
- Name contains `icon` or `image` → `WidgetType.IMAGE`
- No match → `None` (caller emits warning with screen name, skips node)

---

### `core/utils/utils.py` — String Utilities

- `normalize_id(name)` — splits camelCase/PascalCase boundaries, lowercases, replaces non-alnum with `_`. `"BatteryBar"` → `"battery_bar"`.
- `to_snake_case(s)` — strips non-alnum, collapses underscores, lowercases (used for screen names).
- `sanitize_c_string(s, maxlen)` — escapes `"`, `\`, `\n`, `\r`, `\t`, U+2028, U+2029. Truncates to `maxlen - 1`.
- `int_attr(node, key)` — XML attribute → `int`; returns `0` if absent.
- `write_file(path, text)` — thin wrapper over `open().write()`.

---

### `tools/image_converter.py` — Image Pipeline

Exposes `convert_images(assets_dir, src_dir, inc_dir, lvgl_tool, color_format)` as a directly importable function. `main.py` calls it inline — no subprocess boundary. Color format defaults to `"RGB565"`, configurable via `--color-format`.

---

### `static_src/` — Static Runtime Files

| File | Role |
|------|------|
| `ui_defs.h` | C struct/enum definitions; `#include "ui_config.h"` for constants |
| `ui_style.h` | Declaration of `ui_apply_style()` |
| `ui_style.c` | Implementation of `ui_apply_style()` and font mapping |

---

## Data Flow Summary

```
XML <Frame>
    └─ parse_screen()
         └─ for each child node:
              ├─ map_tag_to_child_type() → WidgetType | None (None → skip+warn)
              ├─ parse_style()           → ParsedStyle
              ├─ characters attr         → text_content (sanitized)
              └─ ParsedChild(type, id, x, y, w, h, style, text_content)
         └─ ParsedScreen(name, children[])

ParsedScreen
    └─ generate_screen()
         ├─ for each child:
         │    ├─ CHILDREN[type]              → ChildSpec
         │    ├─ spec.derive_setter_name()   → fn_name
         │    ├─ spec.derive_callback_name() → cb_name
         │    ├─ load_template(setter)       → Template.substitute() → setter code
         │    └─ _render_style_block(style)  → C struct literal
         ├─ for each unique type (ordered list — deterministic):
         │    ├─ load_template(callback) → Template.substitute() → callback code
         │    └─ load_template(init)     → Template.substitute() → init case code
         └─ Template(C_FILE_LAYOUT).substitute() → .c text
            Template(H_FILE_LAYOUT).substitute() → .h text

screens → write_ui_config() → ui_config.h (#define UI_MAX_CHILDREN N)
```
