# Changelog

## 0.3.0 — 2026-05-13

### Fixed
- **Label text never rendered** — `figma_parser.py` now reads text
  content from the FigML `characters` attribute (not `child.text`).
  Text is baked into `data.label.text` in the generated struct.
  `LABEL_INIT` applies it via `lv_label_set_text()` on first render.
  Label text lifecycle: Figma value is the design-time default;
  the generated setter provides full runtime control after that.
- **Control characters in label text** — `sanitize_c_string()` now
  escapes `\n`, `\r`, `\t`, and U+2028/U+2029 (FigML line separators).
  Previously, a label like "Time is\n15:00" produced invalid C.
- **Silent garbage from unrecognized Figma nodes** — structural nodes
  (grouping frames, component instances, decorative shapes) no longer
  silently become empty labels. Unrecognized nodes are now skipped with
  a `WARNING` that names the screen, the node, and the exact rename
  needed in Figma to generate a widget from it.
- **Image widgets ignored Figma dimensions** — `IMAGE_INIT` now calls
  `lv_obj_set_size(c->lv_obj, c->w, c->h)`. Previously, @2x PNG exports
  rendered at double the intended size.
- **`UI_MAX_CHILDREN` overflow** — removed the hardcoded `#define 16`.
  `config_writer.py` now computes the actual maximum child count from
  the parsed XML and writes it to `ui_config.h`. The struct array is
  always exactly the right size; no cap, no overflow, no user action.
- **`safe_substitute()` masking template bugs** — all template
  substitution now uses `substitute()`. A missing variable raises
  `KeyError` at generation time instead of leaving `${variable}` in
  generated C that fails at compile time.
- **`textAlignHorizontal` always returning None** — attribute does not
  exist in FigML exports; the read was dead code. Removed with a comment.
- **`UI_MAX_STRING_LENGTH` mismatch** — Python side was 64, C side 30.
  Both now use 30 (defined in `ui_config.h`).
- **Non-deterministic generated output** — `unique_types` was a `set()`;
  init case ordering varied between Python runs. Changed to an ordered
  list (first-appearance order). Same XML always produces the same C.

### Added
- **`WidgetType` enum** (`core/widget_type.py`) — replaces bare string
  type identifiers like `"UI_CHILD_LABEL"`. Typos now raise
  `AttributeError` at import time. `.c_enum_name()` emits the C literal.
- **`config_writer.py`** — writes `priv_include/ui_config.h` on every
  run. Contains `UI_MAX_CHILDREN`, `UI_MAX_STRING_LENGTH`,
  `UI_MAX_ID_LENGTH`, `UI_MAX_ICON_STATES`.
- **Naming patterns in `ChildSpec`** — `setter_name_pattern` and
  `callback_name_pattern` fields with `derive_setter_name()` and
  `derive_callback_name()` methods. `generator.py` no longer has
  per-type `if/elif` branches; adding a new widget type requires
  zero changes to the generator.
- **Per-screen bar range comment** — if a screen contains bar widgets,
  a `/* TODO: Bar range is hardcoded to 0-100 ... */` comment is emitted
  once above `_init()` listing each bar widget ID by name.
- **CLI flags**: `--yes` / `-y` (skip all prompts, auto-download
  `LVGLImage.py`), `--lvgl-tool PATH` (bypass cache and download —
  for CI and air-gapped builds), `--color-format` / `-f` (pixel
  encoding for PNG conversion: `RGB565`, `RGB888`, `ARGB8888`, `L8`),
  `--patch-esp-includes` (opt-in ESP-IDF include guard patching),
  `--verbose` / `-v` (debug logging), `--verify-compile` (post-generation
  `gcc -fsyntax-only` check on generated `.c` files).
- **Test suite** — 62 tests across `tests/test_parser.py` (38) and
  `tests/test_generator.py` (24). Includes golden-file tests for the
  generator, a realistic FigML XML fixture derived from a real export,
  and regression guards for the label fill routing fix and the
  unresolved template variable fix. `tests/regen_golden.py` helper
  for intentional generator changes.
- **All modules now actively used** — all imports resolve; no orphaned
  dead code remaining in the repo.

### Changed
- **Image converter** (`tools/image_converter.py`) rewritten as an
  importable `convert_images()` function. `main.py` calls it directly —
  no subprocess boundary. Color format is now a parameter, not a
  hardcoded constant.
- **Logging** — all diagnostic output in `main.py`,
  `figma_parser.py`, `figma_helpers.py`, and `image_converter.py`
  now uses Python's `logging` module. UX banners remain `print()`.
- **`normalize_id()`** now splits camelCase and PascalCase boundaries.
  `"BatteryBar"` → `"battery_bar"`, `"HTTPStatus"` → `"http_status"`.
- **`--patch-esp-includes` is now opt-in** (previously ran
  unconditionally). Recommended alternative: set
  `LV_LVGL_H_INCLUDE_SIMPLE=1` in `CMakeLists.txt`.
- **Design docs** — all 10 docs updated to reflect the post-fix
  codebase. Docs 05, 06, 08, 09 newly written.

### Removed
- Dead code: `core/model/`, `core/context.py`, `core/emit/c_file.py`,
  `core/emit/h_file.py`, `core/utils/code_buffer.py`,
  `core/cmake_generator.py`, `core/config_name.py`.
- `UI_CHILD_ICON` from `ui_child_type_t` — no spec, no template, no
  detection rule.
- `int_attr()` duplicate in `figma_helpers.py` — single definition
  in `utils.py`.

### Breaking Changes
- **`ui_defs.h` now includes `ui_config.h`** (auto-generated into
  `priv_include/` on every run). Any `.c` file that includes
  `ui_defs.h` now requires `priv_include/` on its include path —
  including `ui_style.c` in `priv_src/`.
  - ESP-IDF / Zephyr: handled automatically by component include dirs.
  - Bare-metal Makefile: add `-Iui_src/priv_include` to CFLAGS for
    all `ui_src/` source files.
- **Generated C output format changed** — struct initialisers, setter
  names, and init cases are regenerated. Existing hand-edited
  `ui_src/` output must be regenerated from the XML.

---

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