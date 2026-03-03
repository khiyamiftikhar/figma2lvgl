# Figma → LVGL UI Code Generator (ESP-IDF Ready)

A **code-generation tool** that converts **Figma UI layouts** into a
complete **ESP-IDF component** containing:

-   Static LVGL screen structures
-   Thread-safe update system
-   Versioned runtime engine
-   Auto-generated CMake configuration

Designed specifically for **embedded systems (ESP32 + LVGL v9)**.

------------------------------------------------------------------------

## ✨ Key Features

-   📐 Figma XML → Deterministic C code
-   🧵 Thread-safe UI updates via worker queue
-   🧱 Static metadata-driven UI (`ui_screen_t`, `ui_child_t`)
-   📦 Generates full ESP-IDF component
-   🔁 Versioned runtime (safe upgrades)
-   🧩 Extensible via template system
-   🎯 Zero dynamic layout parsing at runtime

------------------------------------------------------------------------

## 🧠 Architecture Overview

    Figma XML
        ↓
    Parser
        ↓
    Model (Screen + Children)
        ↓
    Templates
        ↓
    Generated Code
        ↓
    ESP-IDF Component (ui_component/)
            ├── generated/
            ├── runtime_vX_Y_Z/
            ├── CMakeLists.txt
            └── idf_component.yml

------------------------------------------------------------------------

## 📁 Repository Layout

    figma-lvgl-generator/
    │
    ├── main.py                ← Entry point
    ├── core/                  ← Generator engine (private)
    │   ├── model/
    │   ├── emit/
    │   ├── templates/
    │   ├── utils/
    │   ├── generator.py
    │   ├── figma_parser.py
    │   ├── cmake_generator.py
    │   └── config.py
    │
    └── ui_component/               ← Generated ESP-IDF component
        ├── src_generated/          ← Screen files (.c/.h)
        ├── runtime/                ← Runtime engine
        |── assets/images           ← Images png files referred in xml
        |── assets/src_generated    ← Source file generated for converted images
        ├── CMakeLists.txt
        └── idf_component.yml

------------------------------------------------------------------------

## 🚀 Usage

``` bash
python main.py layout.xml
```

This generates:

    ui_component/
        src_generated/                  #ui C files
        assets/src_generated            #Converted Image source 
        CMakeLists.txt        

Then copy `ui_component/` into your ESP-IDF `components/` directory.

------------------------------------------------------------------------

## 🧩 Extending with New GUI Elements

To add a new UI element:

1.  Create a new template inside `core/templates/`
2.  Register it in `child_registry.py`
3.  Use naming tokens in Figma (e.g. `_button`)

No changes to generator core required.

------------------------------------------------------------------------



## 🏁 Design Philosophy

-   Figma = layout only
-   Naming = semantics
-   Generator = metadata builder
-   Runtime = LVGL execution engine
-   All UI updates = thread-safe
