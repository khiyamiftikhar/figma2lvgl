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
         └─── child_registry (widget type → ChildSpec lookup)
         │
         ▼
┌───────────────────┐
│  image_converter  │  PNG → LVGL C arrays via LVGLImage.py
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│   static_src/     │  ui_defs.h, ui_style.c/h — copied as-is
└────────┬──────────┘
         │
         ▼
        ui_src/
          src/          ← screen .c files
          include/      ← screen .h files
          priv_src/     ← image .c files + ui_style.c
          priv_include/ ← assets.h, ui_defs.h, ui_style.h
```

---

## Component Map

### `figma2lvgl/main.py` — Orchestrator

The single entry point. Owns the full pipeline:

1. Parses CLI arguments
2. Validates the XML file and images directory exist
3. Parses all `<Frame>` nodes in the XML into `ParsedScreen` objects
4. Validates that all required image assets (from `requires_asset=True` children) are present as PNG files
5. Resolves or downloads `LVGLImage.py`
6. Prompts the user if `ui_src/` already has content (overwrite guard)
7. Resets output directories
8. Runs image conversion
9. Patches LVGL include guards in converted image files
10. Copies static sources (`ui_defs.h`, `ui_style.c`, `ui_style.h`)
11. Calls `generate_screen()` for each screen and writes the resulting `.c`/`.h` files

`main.py` contains no parsing or generation logic itself — it only orchestrates.

---

### `core/figma_parser.py` — Parser

Owns all XML-reading logic. Produces Python data objects; knows nothing about C code.

- `parse_screen(frame_node)` — entry point; walks a `<Frame>` XML node and returns a `ParsedScreen`
- `parse_style(node, child_type)` — extracts fill, stroke, radius, font, opacity from a node's attributes and child elements
- Duplicate child ID detection (raises `ValueError`)

Outputs: `ParsedScreen`, `ParsedChild`, `ParsedStyle` (composed of `ParsedStyleBox`, `ParsedStyleText`, `ParsedStyleEffects`)

---

### `core/generator.py` — Code Generator

Takes a `ParsedScreen` and produces two strings: the `.c` file text and the `.h` file text.

- `generate_screen(screen)` — sole public function; returns `(c_filename, h_filename, h_text, c_text)`
- `_render_style_block(style)` — converts a `ParsedStyle` into a C struct initialiser literal
- Uses `string.Template.safe_substitute()` for all variable substitution into templates

Knows about widget types only through `CHILDREN` registry lookups. Does not import parser types.

---

### `core/child_registry.py` — Widget Type Registry

A single dict `CHILDREN` mapping `UI_CHILD_*` string keys to `ChildSpec` instances. This is the **extension point** for new widget types.

```
CHILDREN = {
    "UI_CHILD_LABEL": ChildSpec(...),
    "UI_CHILD_IMAGE": ChildSpec(...),
    "UI_CHILD_BAR":   ChildSpec(...),
}
```

---

### `core/generic_child.py` — ChildSpec

A plain dataclass holding the template names and metadata for one widget type:

| Field | Purpose |
|-------|---------|
| `type_name` | The `UI_CHILD_*` string constant |
| `callback_template` | Template name for the animation/job callback (empty string = none) |
| `setter_template` | Template name for the public setter function |
| `init_template` | Template name for the `switch` case in `_init()` |
| `setter_args` | C argument signature for the setter |
| `requires_asset` | If `True`, main.py validates that a matching PNG exists |

---

### `core/templates/` — Per-Widget Code Templates

One file per widget type (`label_templates.py`, `image_templates.py`, `bar_templates.py`). Each file defines string constants for the three code blocks a widget contributes:

- `*_CALLBACK` — static animation/job callback (empty string for label and image)
- `*_SETTER` — the public setter function body
- `*_INIT` — the `case UI_CHILD_*:` block inside `_init()`

Template variables use `string.Template` syntax: `${variable_name}`.

---

### `core/emit/layouts.py` — File-Level Templates

Defines `C_FILE_LAYOUT` and `H_FILE_LAYOUT` — the top-level string templates for the generated `.c` and `.h` files. Per-widget blocks (struct, callbacks, setters, init cases) are substituted into these layouts by the generator.

---

### `core/utils/template_loader.py` — Template Registry

Maps template name strings (e.g. `"label_setter"`) to the actual template string constants imported from `core/templates/`. Called by `generator.py` via `load_template(name)`.

---

### `core/utils/figma_helpers.py` — Type Detection

`map_tag_to_child_type(node)` — maps a Figma XML node to a `UI_CHILD_*` string by inspecting the node tag and name:

- XML tag `Text` → `UI_CHILD_LABEL`
- Name contains `bar` → `UI_CHILD_BAR`
- Name contains `icon` or `image` → `UI_CHILD_IMAGE`
- Fallback → `UI_CHILD_LABEL`

---

### `core/utils/utils.py` — String Utilities

- `normalize_id(name)` — lowercases, replaces `-` and spaces with `_`
- `to_snake_case(s)` — strips non-alphanumeric chars, collapses underscores, lowercases
- `write_file(path, text)` — thin wrapper over `open().write()`
- `sanitize_c_string(s, maxlen)` — escapes quotes, truncates to `UI_MAX_STRING_LENGTH`

---

### `tools/image_converter.py` — Image Pipeline

Called as a subprocess by `main.py`. Converts all PNGs in the images directory to LVGL C arrays using `LVGLImage.py`, then generates `assets.h` with `LV_IMG_DECLARE()` macros for each image.

---

### `static_src/` — Static Runtime Files

Files that are copied verbatim into `ui_src/priv_include/` and `ui_src/priv_src/` on every run. They are not generated — they are fixed support files for the generated code.

| File | Role |
|------|------|
| `ui_defs.h` | All C struct and enum definitions the generated code depends on |
| `ui_style.h` | Declaration of `ui_apply_style()` |
| `ui_style.c` | Implementation of `ui_apply_style()` and font mapping |

---

## Dead / Unused Code

The following modules exist in the repo but are **not used** by the current pipeline. They appear to be leftovers from an earlier design iteration:

| Module | Status | Notes |
|--------|--------|-------|
| `core/model/screen.py` | Unused | Earlier `Screen` class, superseded by `ParsedScreen` in parser |
| `core/model/child.py` | Unused | Earlier `Child` class, superseded by `ParsedChild` |
| `core/context.py` | Unused | `GenerationContext` accumulator, never instantiated |
| `core/emit/c_file.py` | Unused | `CFile` builder class, generator uses `layouts.py` instead |
| `core/emit/h_file.py` | Unused | `HFile` builder class, same reason |
| `core/utils/code_buffer.py` | Unused | `CodeBuffer` helper, never imported by live code |
| `core/cmake_generator.py` | Dormant | Called in a commented-out block in `main.py` |

These can be removed in the next cleanup release without any functional impact.

---

## Data Flow Summary

```
XML <Frame>
    └─ parse_screen()
         └─ for each child node:
              ├─ map_tag_to_child_type()   → UI_CHILD_* string
              ├─ parse_style()              → ParsedStyle
              └─ ParsedChild(type, id, x, y, w, h, style)
         └─ ParsedScreen(name, children[])

ParsedScreen
    └─ generate_screen()
         ├─ for each child:
         │    ├─ CHILDREN[type] → ChildSpec
         │    ├─ load_template(setter_template) → string
         │    ├─ Template.safe_substitute()     → setter code
         │    └─ _render_style_block(style)     → C struct literal
         ├─ for each unique type:
         │    ├─ load_template(callback_template) → callback code
         │    └─ load_template(init_template)     → init case code
         └─ Template(C_FILE_LAYOUT).safe_substitute() → .c text
            Template(H_FILE_LAYOUT).safe_substitute() → .h text
```
