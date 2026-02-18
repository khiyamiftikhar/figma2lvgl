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
    └── ui_component/          ← Generated ESP-IDF component
        ├── generated/         ← Screen files (.c/.h)
        ├── runtime_v1_0_0/    ← Versioned runtime engine
        ├── CMakeLists.txt
        └── idf_component.yml

------------------------------------------------------------------------

## 🚀 Usage

``` bash
python main.py layout.xml
```

This generates:

    ui_component/
        generated/
        runtime_vX_Y_Z/
        CMakeLists.txt
        idf_component.yml

Then copy `ui_component/` into your ESP-IDF `components/` directory.

------------------------------------------------------------------------

## 🧩 Extending with New GUI Elements

To add a new UI element:

1.  Create a new template inside `core/templates/`
2.  Register it in `child_registry.py`
3.  Use naming tokens in Figma (e.g. `_button`)

No changes to generator core required.

------------------------------------------------------------------------

## 🔁 Runtime Versioning

Runtime engine is versioned:

    runtime_v1_0_0/
    runtime_v1_1_0/
    runtime_v2_0_0/

Generated CMake automatically references the correct version.

This ensures: - Safe upgrades - Backward compatibility - Deterministic
builds

------------------------------------------------------------------------

## 🏁 Design Philosophy

-   Figma = layout only
-   Naming = semantics
-   Generator = metadata builder
-   Runtime = LVGL execution engine
-   All UI updates = thread-safe
