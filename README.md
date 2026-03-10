# figma2lvgl — Figma to LVGL C Code Generator

A **code-generation tool** that converts **Figma UI layouts** into
**LVGL C source files** ready to drop into any embedded project.

- Works with **any LVGL v9 project** (ESP-IDF, Zephyr, bare-metal, etc.)
- Installable via **pip** — no manual script setup
- Fully cross-platform — Windows, Linux, macOS

---

## ✨ Key Features

- 📐 Figma XML → Deterministic C code
- 🧵 Thread-safe UI updates via worker queue
- 🧱 Static metadata-driven UI (`ui_screen_t`, `ui_child_t`)
- 📦 Generates self-contained `ui_src/` folder
- 🔁 Versioned runtime (safe upgrades)
- 🧩 Extensible via template system
- 🎯 Zero dynamic layout parsing at runtime

---

## 🚀 Installation
```bash
pip install figma2lvgl
```

### Prerequisite — LVGLImage.py

Image conversion requires `LVGLImage.py` from the official LVGL repository.
On first run, figma2lvgl will ask to download and cache it automatically.
You can also place it manually next to your XML file.

---

## 📖 Usage
```bash
figma2lvgl -x diagram.xml
```

All arguments:

| Argument | Description | Default |
|---|---|---|
| `-x` | Path to Figma XML file | **Required** |
| `-i` | Folder containing PNG images | Same directory as XML |
| `-d` | Destination for generated output | Same directory as XML |

Examples:
```bash
# Minimal — everything next to the XML
figma2lvgl -x /home/user/project/layout.xml

# Full control
figma2lvgl -x layout.xml -i assets/images -d build/output

# Windows
figma2lvgl -x E:\project\layout.xml -i E:\project\images -d E:\project\output
```

---

## 📁 Output Layout

Running the tool produces a `ui_src/` folder at the destination:
```
ui_src/
  src/              ← Generated screen .c files
  include/          ← Generated screen .h files
  priv_src/         ← Converted image .c files
  priv_include/     ← Image header (assets.h) + and all struct definition header (ui_defs.h)
```

Drop `ui_src/` into your project and add the source files to your build system manually.


## 🎨 Designing in Figma

figma2lvgl reads the **Figma node name** to identify each UI element.
Three element types are currently supported:

### Exporting XML from Figma

figma2lvgl reads XML exported via the **FigML — Figma XML Exporter Plugin**.

To export your design:
1. Right-click on your frame in Figma
2. Go to **Plugins → FigML - Figma XML Exporter Plugin → FigML**
3. Export and save the `.xml` file
4. Pass it to figma2lvgl using `-x`

![Figma XML Export](https://raw.githubusercontent.com/khiyamiftikhar/figma2lvgl/main/docs/figma-export.png)

### Text / Label
Any `Text` node is automatically mapped to an LVGL label.
Just create a text element in Figma — no special naming required.
```
Figma node type: Text
Figma name:      anything (e.g. "Time", "Welcome", "status_label")
Maps to:         LV_OBJ label
```

### Image
Any node whose name contains `icon` or `image` is mapped to an LVGL image.
The name must match the PNG filename placed in your images folder.
```
Figma node type: INSTANCE or FRAME
Figma name:      must contain "icon" or "image" (e.g. "icon_wifi", "image_logo")
Maps to:         LV_OBJ image
Asset required:  icon_wifi.png / image_logo.png in your images folder
```

### Bar
Any node whose name contains `bar` is mapped to an LVGL bar widget.
```
Figma node type: RECTANGLE
Figma name:      must contain "bar" (e.g. "bar", "progress_bar", "battery_bar")
Maps to:         LV_OBJ bar
```

### Naming Rules Summary

| Element | Figma Type | Name Requirement |
|---|---|---|
| Label | Text | any name |
| Image | any | must contain `icon` or `image` |
| Bar | Rectangle | must contain `bar` |

> **Note:** Names are case-insensitive. `Bar`, `BAR`, and `bar` all work.
> The Figma frame name becomes the screen name in generated code.
---
### Example Figma Files

| Display | Link |
|---|---|
| ILI9486 320x480 | [Open in Figma](https://www.figma.com/design/JU5Og9SLLkJiLlspSwfRCb/ili9486?node-id=0-1&t=0rfYzdqqKZITkTkW-1) |
| 128x32 OLED | [Open in Figma](https://www.figma.com/design/uBkcRNjG82tD8hR1sb4wjW/Home-Lock-Gate-Node?node-id=0-1&t=cxgoN9O1GflqxDJP-1) |

## 🧠 Architecture Overview
```
Figma XML
    ↓
Parser
    ↓
Model (Screen + Children)
    ↓
Templates
    ↓
Generated Code (ui_src/)
    ├── src/
    ├── include/
    ├── priv_src/
    └── priv_include/
```

---

## 💡 Example Integrations

See the `examples/` folder for ready-to-use project setups across different platforms:
```
examples/
  espidf/
    ili9486/        ← ESP32 + ILI9486 display
```

More platform examples (STM32, Zephyr, bare-metal) coming soon.

---

## 🏁 Design Philosophy

- **Figma** = layout only
- **Naming conventions** = semantics
- **Generator** = metadata builder
- **Output** = portable C, no runtime dependencies beyond LVGL
- **All UI updates** = thread-safe