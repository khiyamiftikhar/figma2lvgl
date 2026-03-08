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
  priv_include/     ← Image headers + assets.h + ui_defs.h
  CMakeLists.txt    ← Ready to include in your build system
```

Drop `ui_src/` into your project as a component or library.

---

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

## 🧩 Extending with New GUI Elements

To add a new UI element:

1. Create a new template inside `core/templates/`
2. Register it in `child_registry.py`
3. Use naming tokens in Figma (e.g. `_button`)

No changes to the generator core required.

---

## 💡 Examples

See the `examples/` folder for complete project setups:
```
examples/
  espidf/
    ili9486/        ← ESP-IDF project with ILI9486 display
```

---

## 🏁 Design Philosophy

- **Figma** = layout only
- **Naming conventions** = semantics
- **Generator** = metadata builder
- **Output** = portable C, no runtime dependencies beyond LVGL
- **All UI updates** = thread-safe