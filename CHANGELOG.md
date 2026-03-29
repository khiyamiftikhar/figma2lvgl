# Changelog

## 0.2.0 — 2026-03-29

### Added
- **Style foundation** — new `ui_style_t` struct with sub-structs `ui_style_box_t`, `ui_style_text_t`,
  and `ui_style_effects_t` added to `ui_defs.h`. Every widget now carries style data automatically.
- **Style extraction in parser** — `figma_parser.py` now reads fill color, text color, font size,
  corner radius, border color/width, and opacity from Figma XML per node type.
- **Style code generation** — `generator.py` emits `.style` initializer blocks in the generated
  screen structs. Nodes with no style emit `.style = { .box = {0}, .text = {0}, .effects = {0} }`.
- **Runtime style application** — new `ui_style.h` / `ui_style.c` added to the runtime library,
  exposing `ui_apply_style()`. Called once per widget at the end of the init loop — same call,
  same signature for every widget type. All future widgets get styling for free.
- **Font mapping** — `ui_get_font()` maps Figma font sizes to LVGL `lv_font_montserrat_*` pointers,
  guarded by `#if LV_FONT_MONTSERRAT_XX` so only enabled fonts compile.

### Fixed
- Image converter generated include block falling through to `#include "lvgl/lvgl.h"` on ESP32,
  causing a build error. Fixed by adding `LV_LVGL_H_INCLUDE_SIMPLE` guard.

---

## 0.1.3 — 2026-03-10
- Added author, classifiers, keywords and project URLs to pyproject.toml

## 0.1.2 — 2026-03-10
- Added Figma to XML conversion details in the README.md
- Added Figma XML export screenshot and example Figma file links

## 0.1.1 — 2026-03-08
- README updated: removed CMakeLists reference, added Figma design guide with element naming rules

## 0.1.0 — 2026-03-08
- Initial release