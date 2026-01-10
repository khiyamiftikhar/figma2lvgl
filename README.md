# Figma → LVGL UI Code Generator (Embedded)

A **code-generation tool** that converts **Figma UI layouts** into **static, thread-safe LVGL (v9) C code**, designed specifically for **embedded systems** (ESP32, ESP-IDF, etc.).

---
## ✨ Key Features
- 📐 Figma → C code (no runtime layout parsing)
- 🧵 Thread-safe UI updates via job queue
- 🧱 Static UI structures (`ui_screen_t`, `ui_child_t`)
- 🧩 Extensible via templates (no generator rewrites)
- 🧠 Semantics inferred from names, not Figma internals
- 🎯 Optimized for LVGL on microcontrollers

---
## 🧠 Design Philosophy
1. Figma is only for layout (position, size, hierarchy)
2. Semantics come from names (`icon`, `bar`, etc.)
3. All LVGL calls are centralized and thread-safe
4. One child = one template file

---
## 📁 Repository Layout
```
figma-lvgl-generator/
├── main.py
├── figma_parser.py
├── generator.py
├── child_registry.py
├── generic_child.py
├── model/
├── templates/
├── utils/
└── README.md
```

---
## 🚀 How to Use
```bash
python main.py layout.xml
```

Generates one `.c` and one `.h` per screen (Frame).

---
## ➕ Extending: Add a New Child
1. Create a template file in `templates/` (job + callback + setter + init)
2. Register it in `child_registry.py`
3. Use naming tokens in Figma (e.g. `_button`)

No generator changes required.

---
## 🏁 Summary
Design visually in Figma.  
Generate deterministic, static LVGL code.  
Control behavior safely from C.
