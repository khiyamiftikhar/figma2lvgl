# figma2lvgl — Overview

## What It Is

`figma2lvgl` is a **CLI code-generation tool** that converts a Figma UI design — exported as XML — into LVGL v9 C source files ready to drop into any embedded project.

It is a **build-time tool**, not a runtime library. It runs on a developer's machine, reads a Figma XML file and a folder of PNG assets, and produces a self-contained `ui_src/` folder. No part of the generator ships to the device.

---

## Design Philosophy

**The tool is the translator between two worlds:**

- **Figma** — designed for apps and web: responsive layouts, any font, GPU rendering, dynamic content, gradients, hover states
- **LVGL** — designed for embedded targets: fixed display, limited RAM, static object trees, direct pointer access

The tool is transparent to both sides:
- The **UI designer** opens Figma and designs naturally — frames, components, nesting, text layers. No workarounds. Just design.
- The **firmware developer** gets clean LVGL C code. Named access, direct pointers, self-documenting API that mirrors the Figma hierarchy.

**The embedded lens:** Not everything Figma supports maps to LVGL. Gradients, shadows, hover states, responsive breakpoints — these are silently ignored. The tool extracts what LVGL can represent faithfully and ignores the rest.

---

## Key Features

- Figma XML → deterministic, reproducible C code
- Full Figma hierarchy — nested frames, containers, buttons with labels
- Generated **screen-specific C struct mirrors the Figma hierarchy** — firmware accesses `home.panel_top.time`, not `children[2]`
- Figma styles → LVGL style calls at init time
- Interactive widgets: button (with overrideable weak callback), slider
- Dynamic container escape hatch (`list_*`, `grid_*`) for runtime-filled lists
- Automatic icon detection — Figma icon components recognised from internal Vector structure without requiring naming convention
- Generates self-contained `ui_src/` folder
- Extensible widget system — add a new widget type in one file, zero generator changes

---

## Supported LVGL Version

LVGL **v9** only.

---

## Supported Widget Types

| Widget | Figma trigger | LVGL object |
|--------|--------------|-------------|
| Label | Any `Text` node | `lv_label_create` |
| Image | Name contains `icon`/`image`, OR component instance with Vector children | `lv_image_create` |
| Bar | Name contains `bar` | `lv_bar_create` |
| Button | Name starts with `btn_` or `button_` | `lv_button_create` |
| Slider | Name starts with `slider_` or ends with `_slider` | `lv_slider_create` |
| Panel | Named frame with fill/border/radius | `lv_obj_create` (container) |
| Dynamic container | Name starts with `list_` or `grid_` | `lv_obj_create` (firmware fills) |

---

## Supported Style Properties

| Figma property | LVGL call |
|---------------|-----------|
| Fill color | `lv_obj_set_style_bg_color` |
| Fill opacity | `lv_obj_set_style_bg_opa` |
| Text color (labels, buttons) | `lv_obj_set_style_text_color` |
| Font size | `lv_obj_set_style_text_font` (Montserrat 10–24pt) |
| Corner radius | `lv_obj_set_style_radius` |
| Stroke color | `lv_obj_set_style_border_color` |
| Stroke weight | `lv_obj_set_style_border_width` |
| Opacity | `lv_obj_set_style_opa` |

Font family is always Montserrat. Unsupported font sizes fall back to `LV_FONT_DEFAULT`.

---

## Output Layout

```
ui_src/
  src/              ← Generated screen .c files (one per Figma Frame)
  include/          ← Generated screen .h files
  priv_src/         ← Converted image .c files + ui_style.c
  priv_include/     ← assets.h, ui_config.h, ui_defs.h, ui_style.h
```

---

## Installation

```bash
pip install figma2lvgl

figma2lvgl -x layout.xml
figma2lvgl -x layout.xml -i assets/images -d build/output
figma2lvgl -x layout.xml --yes --lvgl-tool ./tools/LVGLImage.py   # CI
```

See `09_cli_execution_flow.md` for all CLI flags.
