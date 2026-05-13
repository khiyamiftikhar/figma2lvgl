# figma2lvgl — Overview

## What It Is

`figma2lvgl` is a **CLI code-generation tool** that converts a Figma UI design — exported as XML — into LVGL v9 C source files ready to drop into any embedded project.

It is a **build-time tool**, not a runtime library. It runs on a developer's machine, reads a Figma XML file and a folder of PNG assets, and produces a self-contained `ui_src/` folder of C and H files. No part of the generator ships to the device.

---

## Design Philosophy

| Principle | What it means in practice |
|-----------|--------------------------|
| **Figma = layout + visual style** | All positioning, sizing, colors, fonts, and borders come from Figma. Nothing is hardcoded in C. |
| **Naming conventions = semantics** | The Figma node name determines the widget type. A node named `progress_bar` becomes an LVGL bar; a node named `icon_wifi` becomes an LVGL image. |
| **Generator = metadata + style builder** | The generator emits static C structs (`ui_screen_t`, `ui_child_t`) that carry layout metadata and style data. No dynamic parsing happens on the device. |
| **Output = portable C** | Generated code has no dependencies beyond LVGL v9. It works on ESP-IDF, Zephyr, bare-metal, or any platform with LVGL. |
| **Zero runtime layout parsing** | All geometry and style values are baked into static struct initialisers at generation time. The device just reads the structs and calls LVGL APIs. |
| **Thread-safe UI updates** | All runtime UI mutations go through setter functions generated per widget. Setters reference the struct by index, keeping access deterministic. |

---

## Key Features

- Figma XML → deterministic, reproducible C code
- Figma styles (color, fill opacity, font size, text alignment, radius, border) → `ui_style_t` struct fields → LVGL style calls at init time
- Static metadata-driven UI — `ui_screen_t` and `ui_child_t` arrays are compile-time constants (aside from `lv_obj` pointers filled at init)
- Generates a self-contained `ui_src/` folder with no external dependencies beyond LVGL
- PNG image assets converted to LVGL C arrays via `LVGLImage.py` (fetched and cached automatically on first run)
- Extensible widget type system — new widget types are added by registering a `ChildSpec` in `child_registry.py`
- Cross-platform — runs on Windows, Linux, macOS
- Installable via `pip install figma2lvgl`

---

## Supported LVGL Version

LVGL **v9** only. Generated code uses `lv_image_set_src`, `lv_bar_set_value`, `lv_label_set_text`, and `lv_obj_set_style_*` APIs from the v9 API surface.

---

## Supported Widget Types (current release)

| Widget | Figma trigger | Generated LVGL object |
|--------|--------------|----------------------|
| Label | Any `Text` node | `lv_label_create` |
| Image | Name contains `icon` or `image` | `lv_image_create` |
| Bar | Name contains `bar` | `lv_bar_create` |

Type detection is name-based and case-insensitive. The Figma frame name becomes the screen name.

---

## Supported Style Properties

| Figma property | Applies to | C field | LVGL call |
|---------------|-----------|---------|-----------|
| Fill color | All widgets | `ui_style_box_t.bg` | `lv_obj_set_style_bg_color` |
| Fill opacity | All widgets | `ui_style_box_t.bg_opa` | `lv_obj_set_style_bg_opa` |
| Text color | Labels | `ui_style_text_t.color` | `lv_obj_set_style_text_color` |
| Font size | Labels | `ui_style_text_t.size` | `lv_obj_set_style_text_font` |
| Text alignment | Labels | `ui_style_text_t.align` | `lv_obj_set_style_text_align` |
| Corner radius | All widgets | `ui_style_box_t.radius` | `lv_obj_set_style_radius` |
| Stroke color | All widgets | `ui_style_box_t.border_color` | `lv_obj_set_style_border_color` |
| Stroke weight | All widgets | `ui_style_box_t.border_width` | `lv_obj_set_style_border_width` |
| Opacity | All widgets | `ui_style_effects_t.opacity` | `lv_obj_set_style_opa` |

Font sizes supported: 10, 12, 14, 16, 18, 20, 22, 24 (Montserrat). Any unmapped size falls back to `LV_FONT_DEFAULT`.

---

## Input Requirements

| Input | Description |
|-------|-------------|
| Figma XML file | Exported via the **FigML — Figma XML Exporter** plugin. Each top-level `<Frame>` in the XML becomes one screen. |
| PNG images | Required for any widget whose name contains `icon` or `image`. Filename must match the Figma node name exactly (e.g. node `icon_wifi` → `icon_wifi.png`). |

---

## Output Layout

```
ui_src/
  src/              ← Generated screen .c files (one per Figma frame)
  include/          ← Generated screen .h files (one per Figma frame)
  priv_src/         ← Converted image .c files + static ui_style.c
  priv_include/     ← assets.h, ui_defs.h, ui_style.h
```

---

## Installation and Basic Usage

```bash
pip install figma2lvgl

# Minimal — XML and images in the same folder
figma2lvgl -x layout.xml

# Full control
figma2lvgl -x layout.xml -i assets/images -d build/output
```

| Argument | Description | Default |
|----------|-------------|---------|
| `-x` | Path to Figma XML file | **Required** |
| `-i` | Folder containing PNG images | Same directory as XML |
| `-d` | Destination for `ui_src/` output | Same directory as XML |
