# figma2lvgl — Figma to LVGL C Code Generator

A **code-generation tool** that converts **Figma UI layouts** into
**LVGL C source files** ready to drop into any embedded project.

- Works with **any LVGL v9 project** (ESP-IDF, Zephyr, bare-metal, etc.)
- Installable via **pip** — no manual script setup
- Fully cross-platform — Windows, Linux, macOS

---

## ✨ Key Features

- 📐 Figma XML → Deterministic, reproducible C code
- 🎨 Figma styles (color, font, radius, border) → LVGL style calls
- 🔤 Figma label text → baked into generated struct as design-time default
- 🧱 Static metadata-driven UI (`ui_screen_t`, `ui_child_t`)
- 📦 Generates self-contained `ui_src/` folder
- 🧩 Extensible widget type system via template registration
- 🎯 Zero dynamic layout parsing at runtime
- 🔁 Auto-sized child array — `UI_MAX_CHILDREN` computed from your design

---

## 🚀 Installation

```bash
pip install figma2lvgl
```

### Prerequisite — LVGLImage.py

Image conversion requires `LVGLImage.py` from the official LVGL repository (MIT licensed).

**Interactive (first run):** figma2lvgl will ask to download and cache it automatically.

**CI / air-gapped builds:** provide it directly with `--lvgl-tool`:
```bash
figma2lvgl -x layout.xml --yes --lvgl-tool ./tools/LVGLImage.py
```

---

## 📖 Usage

```bash
figma2lvgl -x diagram.xml
```

### All Arguments

| Argument | Description | Default |
|---|---|---|
| `-x` / `--xml` | Path to Figma XML file | **Required** |
| `-i` / `--images` | Folder containing PNG images | Same directory as XML |
| `-d` / `--dest` | Destination for generated output | Same directory as XML |
| `-y` / `--yes` | Skip all prompts; auto-download LVGLImage.py if not cached | off |
| `--lvgl-tool PATH` | Path to LVGLImage.py — bypasses cache and download (for CI) | auto |
| `-f` / `--color-format` | PNG pixel encoding: `RGB565`, `RGB888`, `ARGB8888`, `L8` | `RGB565` |
| `--patch-esp-includes` | Patch LVGL include guards in image files for ESP-IDF¹ | off |
| `-v` / `--verbose` | Enable debug-level logging | off |
| `--verify-compile` | Syntax-check generated C files with `gcc` after generation | off |

> ¹ Prefer `target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_LVGL_H_INCLUDE_SIMPLE=1)`
> in your `CMakeLists.txt` instead of this flag.

### Examples

```bash
# Minimal — everything next to the XML
figma2lvgl -x /home/user/project/layout.xml

# Full control
figma2lvgl -x layout.xml -i assets/images -d build/output

# CI pipeline (no prompts, pinned tool, 32-bit display)
figma2lvgl -x layout.xml --yes --lvgl-tool ./tools/LVGLImage.py -f ARGB8888

# Windows
figma2lvgl -x E:\project\layout.xml -i E:\project\images -d E:\project\output
```

---

## 📁 Output Layout

Running the tool produces a `ui_src/` folder at the destination:

```
ui_src/
  src/              ← Generated screen .c files (one per Figma frame)
  include/          ← Generated screen .h files
  priv_src/         ← Converted image .c files + ui_style.c
  priv_include/     ← assets.h, ui_config.h, ui_defs.h, ui_style.h
```

Drop `ui_src/` into your project and add the source files to your build system.

> **Build system note:** ensure `priv_include/` is on the include path for
> **all** source files in `ui_src/`, including those in `priv_src/`.
> ESP-IDF and Zephyr handle this automatically. For bare-metal Makefiles,
> add `-Iui_src/priv_include` to your CFLAGS.

### Using a generated screen

```c
#include "ui_home_screen.h"

// In your app init:
ui_home_screen_init();   // creates LVGL objects, applies styles, shows Figma text
ui_home_screen_load();   // makes this screen active

// At runtime — update widgets:
ui_home_screen_set_time("16:30");          // label setter
ui_home_screen_set_battery_bar(85, 300);   // bar setter — animated over 300ms
ui_home_screen_display_icon_wifi();        // image setter
```

---

## 🎨 Designing in Figma

figma2lvgl reads the **Figma node name** to identify each UI element type and
**Figma styles** to generate matching LVGL style calls.

### Exporting XML from Figma

figma2lvgl reads XML exported via the **FigML — Figma XML Exporter Plugin**.

1. Right-click your frame in Figma
2. Go to **Plugins → FigML - Figma XML Exporter Plugin → FigML**
3. Export and save the `.xml` file
4. Pass it to figma2lvgl with `-x`

![Figma XML Export](https://raw.githubusercontent.com/khiyamiftikhar/figma2lvgl/main/docs/figma-export.png)

---

### Supported Widgets

#### Text / Label

Any `Text` node is automatically mapped to an LVGL label. The text content
you typed in Figma is baked into the generated struct as the initial display value.
Your firmware can update it at runtime via the generated setter.

```
Figma node type: Text
Figma name:      anything (e.g. "time", "welcome_label", "status")
Maps to:         lv_label_create()
Initial text:    taken from Figma text content automatically
```

#### Image

Any node whose name contains `icon` or `image` maps to an LVGL image.
The node name must match the PNG filename in your images folder.

```
Figma node type: INSTANCE or FRAME
Figma name:      must contain "icon" or "image" (e.g. "icon_wifi", "image_logo")
Maps to:         lv_image_create()
Asset required:  icon_wifi.png / image_logo.png in your images folder
```

#### Bar

Any node whose name contains `bar` maps to an LVGL bar widget.
The generated setter supports instant updates and animated transitions.

```
Figma node type: RECTANGLE
Figma name:      must contain "bar" (e.g. "battery_bar", "progress_bar")
Maps to:         lv_bar_create()
Range:           0–100 by default (adjust lv_bar_set_range in generated _init)
```

### Naming Rules Summary

| Widget | Figma Node Type | Name Requirement |
|--------|----------------|-----------------|
| Label  | Text           | any name |
| Image  | any            | must contain `icon` or `image` |
| Bar    | Rectangle      | must contain `bar` |

> **Names are case-insensitive.** `Bar`, `BAR`, and `bar` all work.

> **Unknown nodes are skipped with a warning.** If a node doesn't match any
> rule, figma2lvgl logs a warning naming the screen and the node, and tells
> you exactly how to rename it to generate a widget from it. Nothing is
> silently generated for unrecognized nodes.

> **The Figma frame name becomes the screen name.** `Home Screen` →
> `home_screen` → `ui_home_screen_init()`, `ui_home_screen_load()`.

---

### Supported Styles

Styles applied in Figma are automatically extracted and translated to LVGL
style calls at runtime. No manual style code needed.

| Figma Property | Applies To | LVGL Call |
|---|---|---|
| Fill color | All widgets | `lv_obj_set_style_bg_color` |
| Fill opacity | All widgets | `lv_obj_set_style_bg_opa` |
| Text color | Labels | `lv_obj_set_style_text_color` |
| Font size | Labels | `lv_obj_set_style_text_font` |
| Corner radius | All widgets | `lv_obj_set_style_radius` |
| Stroke color | All widgets | `lv_obj_set_style_border_color` |
| Stroke weight | All widgets | `lv_obj_set_style_border_width` |
| Opacity | All widgets | `lv_obj_set_style_opa` |

> **Text alignment** is not extracted — FigML does not export horizontal
> text alignment in its XML output. LVGL's default (left) is used.

#### Font Sizes

figma2lvgl maps Figma font sizes to LVGL Montserrat fonts.
Supported sizes: `10, 12, 14, 16, 18, 20, 22, 24`.
Any other size falls back to `LV_FONT_DEFAULT`.

Enable only the sizes your design uses in `lv_conf.h`:
```c
#define LV_FONT_MONTSERRAT_12  1
#define LV_FONT_MONTSERRAT_14  1
```
> Each font size adds flash usage — only enable what you need.

---

### Example Figma Files

| Display | Link |
|---|---|
| ILI9486 320×480 | [Open in Figma](https://www.figma.com/design/JU5Og9SLLkJiLlspSwfRCb/ili9486?node-id=0-1&t=0rfYzdqqKZITkTkW-1) |
| 128×32 OLED | [Open in Figma](https://www.figma.com/design/uBkcRNjG82tD8hR1sb4wjW/Home-Lock-Gate-Node?node-id=0-1&t=cxgoN9O1GflqxDJP-1) |

---

## 🧠 Architecture Overview

```
Figma XML + PNG assets
        │
        ▼
    Parser          reads characters attr, styles, geometry
        │
        ▼
    Model           ParsedScreen / ParsedChild / ParsedStyle
        │
        ▼
    Generator       WidgetType enum + ChildSpec templates
        │
        ▼
  Generated Code  (ui_src/)
    ├── src/            ← screen .c files
    ├── include/        ← screen .h files  
    ├── priv_src/       ← image .c files + ui_style.c
    └── priv_include/   ← assets.h, ui_config.h, ui_defs.h, ui_style.h
```

`ui_config.h` is auto-generated on every run and sets `UI_MAX_CHILDREN`
to exactly the number your design requires.

---

## 💡 Example Integrations

See the `examples/` folder for ready-to-use project setups:

```
examples/
  espidf/
    ili9486/        ← ESP32 + ILI9486 320×480 display
```

More platform examples (STM32, Zephyr, bare-metal) coming soon.

---

## 🏁 Design Philosophy

| Principle | What it means |
|-----------|--------------|
| **Figma = layout + style** | All geometry, colors, fonts come from Figma — nothing hardcoded in C |
| **Naming = semantics** | Node name determines widget type; rename in Figma to change the generated widget |
| **Generator = metadata builder** | Emits static C structs; no dynamic parsing at runtime |
| **Output = portable C** | No dependencies beyond LVGL v9 |
| **Initial state = Figma** | Generated structs carry Figma text and styles as the design-time default; firmware updates from there |