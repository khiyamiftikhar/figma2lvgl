# Changelog

## [0.4.4] — 2026-05-15

### Changed
- **Callback model: weak linking removed, linker-error model adopted** —
  `__attribute__((weak))` stub definitions are no longer emitted in generated
  `.c` files. Callbacks are declared in `.h` and registered in `_init()` but
  have no default body. If not implemented, the linker reports an undefined
  reference. This works correctly on all compilers including MSVC (Visual
  Studio simulator). Implement a no-op body if you don't need the callback:
  `void ui_home_on_btn_ok_clicked(lv_event_t *e) { (void)e; }`

### Docs
- **README**: full rewrite to reflect v0.4.x architecture — typed per-screen
  struct, named event callbacks, bar/slider range encoding, button event
  suffixes, all widget types documented, linker-error callback model explained.
  Removed all v0.3.0 references (`ui_child_t`, `UI_MAX_CHILDREN`, `ChildSpec`,
  `ParsedChild`, generic array access).

---

## [0.4.3] — 2026-05-15

### Fixed
- **Styles never applied at runtime** — `emit_node_initializer()` never
  called `render_style_init()`, leaving every `.style` field
  zero-initialized. Since `ui_apply_style()` gates every property on
  `has_*` flags, all styles silently did nothing at runtime despite
  `ui_apply_style()` being called correctly in `_init()`.

  Fix: `.style` is now always the first field emitted in every node's
  initializer block — empty style → `.style = {0}`, non-empty →
  populated sub-structs with `has_*` flags set.

- **PANEL children with only a style block were silently dropped** from
  the initializer — the `if body.strip()` guard in
  `emit_screen_initializer()` treated a style-only body as empty.
  Guard removed; every node always has at least `.style = {0}`.

### Docs
- `05_code_generation.md`: corrected `emit_node_initializer` description
  from "emits only fields with non-default values" to "always emits
  `.style` first, then widget-specific fields"

---

## [0.4.2] — 2026-05-15

### Fixed
- **Callback name corruption for screens starting with 's'** — `_callback_name_for()`
  used `lstrip("s_")` which strips individual characters, not the prefix string.
  Screen names like `settings` or `splash` produced broken callback names
  (`ettings`, `plash`). Changed to `removeprefix("s_")`.

- **`UI_MAX_CHILDREN` phantom macro** — `config_writer.py` computed a max-children
  value and `main.py` logged and printed it as `UI_MAX_CHILDREN`, but this macro
  was never written to `ui_config.h`. Removed the dead computation and all
  references to the non-existent macro.

### Implemented
- **BAR range from Figma name** — bar widgets now support the same range encoding
  as sliders: `battery_bar_0_100` → range 0–100, `temp_bar_n20_50` → range −20–50.
  Adds `bar_min`/`bar_max` to `ParsedNode`, `int32_t min/max` to the generated BAR
  struct, and `lv_bar_set_range` now uses struct fields instead of the previous
  hardcoded `0, 100` with a `/* TODO */` comment.

### Removed
- **8 dead v0.3.0 files** that were never imported by the v0.4.0 pipeline:
  `core/generic_child.py`, `core/child_registry.py`, `core/utils/template_loader.py`,
  `core/templates/label_templates.py`, `core/templates/image_templates.py`,
  `core/templates/bar_templates.py`, `core/templates/screen_template.py`,
  `core/emit/layouts.py`. These contained the old `ui_child_t *c = &children[index]`
  pattern that contradicted the v0.4.0 architecture in docs and confused anyone
  reading the source tree.

### Tests
- `regen_golden.py` rewritten to use the current `parse_screen()` API
  (previously referenced deleted `ParsedChild` class and would not run)
- Added `test_bar_range_fields` and `test_bar_init_uses_struct_range` to
  `test_generator.py`
- Added `test_bar_range_from_name` to `test_parser.py`
- Golden files regenerated to reflect bar struct changes
- All 67 tests pass

### Docs
- `01_architecture.md`: corrected module map — `_C_LAYOUT`/`_H_LAYOUT` are defined
  in `core/generator.py`, not `core/emit/layouts.py` (which no longer exists)

## 0.4.1 — 2026-05-15

## Changed
- Added support for parsing **Figma event modifiers** from widget names.
- Introduced `parse_widget_name()` to separate widget IDs from modifiers.
- Added `EVENT_SUFFIX_MAP` and `BUTTON_DEFAULT_EVENT` for standardized event handling.
- Extended `ParsedNode` to store `event_modifiers`.
- Generator now strips event modifiers and slider range values before forming struct IDs.
- Buttons now always register `LV_EVENT_CLICKED` and can optionally register additional callbacks for explicitly defined modifiers.
- Weak callback generation now follows the naming pattern:
  `on_<widget_id>_<event>()`

## Fixed
- Fixed button text styling behavior in generated LVGL code.
- Previously, text styles were applied to the button container and relied on LVGL inheritance.
- Text styles are now applied directly to the internal label object.
- Button containers now only receive box-related styling (background, border, radius).
- Button text styles are now extracted from the Figma `Text` child and merged into `style.text`.
- This makes button styling behavior explicit and predictable.

## Documentation
- Added documentation for widget name parsing (base names vs modifiers).
- Documented that modifiers are stripped before `normalize_id()`.
- Documented slider range stripping behavior.
- Rewrote button architecture docs to reflect two-target styling:
  - box styles → button container
  - text styles → internal label
- Added full event suffix mapping tables with generated struct fields and callback examples.
- Updated code generation docs with new button initialization flow.
- Updated runtime styling docs to clarify that text styling applies to `LABEL` only.
- Updated Figma guide with new button behavior, callback naming, and event examples.
- Documented why `LV_EVENT_ALL` is intentionally not used.
- Updated tests and regenerated golden UI outputs.

---

## 0.4.0 — 2026-05-14

### Changed — Architecture (Breaking)

- **Per-screen hierarchical struct replaces generic flat array** — each Figma
  `<Frame>` now generates its own typed, file-static C struct that mirrors the
  Figma node hierarchy exactly. The firmware developer accesses widgets by name
  (`s_home.panel_top.time.lv_obj`) instead of by index
  (`children[2]`). The struct is never exposed in the `.h` file — only the
  generated API functions are public.
- **`ui_child_t` and `ui_screen_t` removed from `ui_defs.h`** — these generic
  structs are replaced by the per-screen generated structs. `ui_defs.h` now
  contains only `ui_child_type_t`, `ui_style_t`, and its sub-structs.
- **`UI_MAX_CHILDREN` removed from `ui_config.h`** — no longer needed since
  the screen struct is always exactly the right size for the design.
- **Parser is now a recursive tree walker** — replaces the flat direct-children
  walk from v0.3.0. The full Figma hierarchy is parsed to arbitrary depth.
  `ParsedNode` replaces `ParsedChild`; each node carries a `children` list.
- **Generator is now a three-pass emitter** — struct emitter (recursive),
  init emitter (BFS flat sequence), setter emitter (setters + callbacks).
  String templates (`core/templates/`) are no longer used for code generation.
- **API function names encode the hierarchy** — `ui_home_panel_top_time_set_text()`
  tells the developer exactly where in the Figma design the widget lives.
  No need to open Figma or read the struct to understand the path.

### Added

- **Button widget** (`btn_*` / `button_*` naming) — generates
  `lv_button_create()` with an internal label. Button label text is read from
  the first `<Text>` child inside the Figma button frame. Falls back to
  deriving label from the node name if no text child is present.
- **Slider widget** (`slider_*` / `*_slider` naming) — generates
  `lv_slider_create()` with configurable range. Range encoded in name:
  `brightness_slider_0_255` → min=0, max=255.
- **Panel / container** — any named Figma frame with visual properties
  (fill, border, or radius) becomes an `lv_obj_create()` container with
  scrollbars disabled and clickable flag off. Children of the panel are
  parsed recursively and nested in the generated struct.
- **Dynamic container escape hatch** (`list_*` / `grid_*` naming) — generator
  stops recursion at these nodes. Only the container `lv_obj_t*` is generated.
  Firmware fills it at runtime via `ui_{screen}_get_{id}()`.
- **Weak event callbacks** — BUTTON and SLIDER nodes generate
  `__attribute__((weak))` callbacks. Firmware overrides in its own `.c` file
  with no registration needed.
- **Structural frame promotion** — Figma frames with no fill, no border, no
  radius, and an auto-generated name (e.g. "Frame 12") are silently dropped.
  Their children are promoted to the parent level. Transparent to both
  designer and firmware developer.
- **Static vs dynamic text** — button `label_text` is `const char *` (Flash).
  All other label text is `char text[UI_MAX_STRING_LENGTH]` (RAM) with a
  setter generated.
- **Nesting depth limits** — warning at depth > 5, hard stop at depth > 7.
  Generator emits actionable log messages with the screen name and node name.
- **`ui_style.c` extended** — `ui_apply_style()` now handles
  `UI_CHILD_BUTTON`, `UI_CHILD_SLIDER`, `UI_CHILD_PANEL`. Slider indicator
  receives fill color on `LV_PART_INDICATOR` (same pattern as bar).
- **64 tests** across updated `test_parser.py` (37) and `test_generator.py`
  (27) — covers tree walker, button label extraction, panel nesting, dynamic
  container stop, BFS ordering, hierarchical struct and API name correctness.

### Removed

- `core/templates/label_templates.py`, `bar_templates.py`,
  `image_templates.py` — replaced by the three emitter modules.
- `core/utils/template_loader.py` — no longer needed.
- `core/child_registry.py`, `core/generic_child.py` — `ChildSpec` registry
  pattern replaced by direct emitter logic per `WidgetType`.
- `core/emit/layouts.py` — replaced by inline layout strings in `generator.py`.
- `validate_assets()` no longer takes a `child_registry` argument — IMAGE
  nodes are discovered via the parser tree directly.

### Known Limitations (deferred to v1.0.0)

Screen navigation, font family support, gradients, shadows, scrolling
containers, and Auto Layout / flex are not supported in this release.
See `DESIGN_SPEC_v040.md` for the full scope boundary.

---

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