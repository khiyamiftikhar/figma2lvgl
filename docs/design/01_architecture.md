# figma2lvgl — Architecture

## High-Level Pipeline

```
Figma XML + PNG assets
        │
        ▼
┌───────────────────┐
│   CLI / main.py   │  Argument parsing, validation, orchestration
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│  figma_parser.py  │  XML tree walker → ParsedScreen / ParsedNode tree
└────────┬──────────┘
         │
         ▼
┌───────────────────────────────────┐
│   Three-pass code generator       │
│  ┌──────────────┐                 │
│  │ node_emitter │ struct fields   │
│  │ init_emitter │ _init() body    │
│  │setter_emitter│ setters + CBs   │
│  └──────────────┘                 │
└────────┬──────────────────────────┘
         │
         ▼
┌───────────────────┐
│  image_converter  │  PNG → LVGL C arrays via LVGLImage.py
└────────┬──────────┘
         │
         ▼
┌───────────────────┐
│   static_src/     │  ui_defs.h, ui_style.c/h — copied as-is
└────────┬──────────┘
         │
         ▼
        ui_src/
```

---

## Module Map

### `figma2lvgl/main.py` — Orchestrator

CLI entry point. Owns the full pipeline: parse → validate assets → convert images → copy static files → generate ui_config.h → generate screen files. Contains no parsing or generation logic — only orchestration.

### `core/figma_parser.py` — Tree Walker

Recursively walks the Figma XML tree. Produces a `ParsedScreen` containing a tree of `ParsedNode` objects that mirrors the Figma hierarchy. Handles structural frame promotion (invisible grouping frames are dropped, their children promoted to the parent level).

### `core/utils/figma_helpers.py` — Widget Type Detection

`detect_widget_type(node)` — 10-rule priority detection returning a `WidgetType` enum value or `None`. Includes content-based icon component detection (Vector children heuristic). See `03_figma_parsing.md` for the full rule table.

### `core/widget_type.py` — WidgetType Enum

Seven values: `LABEL`, `IMAGE`, `BAR`, `BUTTON`, `SLIDER`, `PANEL`, `DYNAMIC`, `STRUCTURAL`. `STRUCTURAL` is an internal sentinel — never appears in the output. Used by the parser, detection, emitters, and `ui_apply_style()`.

### `core/node_emitter.py` — Struct Emitter

Pass 1 of generation. Recursively emits the C struct field block and static initializer for a screen. Produces the `static struct { ... } s_home = { ... };` block that mirrors the Figma hierarchy.

### `core/init_emitter.py` — Init Emitter

Pass 2 of generation. BFS traversal of the ParsedNode tree. Emits a flat sequence of LVGL creation calls (parents before children). No C recursion — just sequential `lv_*_create()` calls in `_init()`.

### `core/setter_emitter.py` — Setter/Callback Emitter

Pass 3 of generation. Walks the tree and emits setter functions and `__attribute__((weak))` event callbacks for interactive/dynamic widgets.

### `core/generator.py` — Orchestrates the Three Passes

Calls the three emitters, assembles the results into C and H layout strings, returns `(c_filename, h_filename, h_text, c_text)`.

### `core/emit/layouts.py` — C/H File Templates

`_C_LAYOUT` and `_H_LAYOUT` — Python format strings for the full `.c` and `.h` file structure. Per-widget code blocks are substituted in by the generator.

### `core/config_writer.py` — ui_config.h Generator

Writes `priv_include/ui_config.h` containing `UI_MAX_STRING_LENGTH` and other constants. Auto-generated on every run.

### `tools/image_converter.py` — Image Pipeline

`convert_images()` — importable function that converts PNG assets to LVGL C arrays using `LVGLImage.py`. Called directly from `main.py` (no subprocess boundary).

### `static_src/` — Static Runtime Files

Three files copied verbatim into `ui_src/priv_include/` and `ui_src/priv_src/` on every run:
- `ui_defs.h` — `ui_child_type_t` enum and `ui_style_t` structs
- `ui_style.h` — declaration of `ui_apply_style()`
- `ui_style.c` — implementation of `ui_apply_style()` and font mapping

---

## Data Flow

```
XML <Frame>
    └─ parse_screen()
         └─ _parse_children() [recursive]
              ├─ detect_widget_type()  → WidgetType enum
              ├─ parse_style()         → ParsedStyle
              ├─ _find_button_label()  → text (for BUTTON only)
              └─ ParsedNode(type, id, x,y,w,h, style, text, children[])

ParsedScreen (tree of ParsedNodes)
    └─ generate_screen()
         ├─ Pass 1: emit_screen_struct_type()   → C struct block
         ├─ Pass 2: emit_init_body()             → flat init sequence
         └─ Pass 3: collect_setters_and_callbacks() → setters, callbacks
         └─ _C_LAYOUT / _H_LAYOUT               → .c text, .h text
```

---

## Key Design Decisions

**Screen-specific struct, not a generic array:**
Each Figma Frame generates its own typed C struct that mirrors the Figma hierarchy. `home.panel_top.time.lv_obj` — not `home.children[1].lv_obj`. The struct is file-static and never exposed in the `.h` file.

**BFS init ordering:**
`_init()` creates LVGL objects in breadth-first order — parents always before children. The generated code is a flat sequence of create calls, not nested function calls. No stack depth from nesting.

**Weak callbacks:**
BUTTON and SLIDER generate `__attribute__((weak))` default implementations. Firmware overrides in its own `.c` file with no registration needed.

**Static vs dynamic text:**
Button `label_text` → `const char *` (Flash). All other label text → `char text[UI_MAX_STRING_LENGTH]` (RAM) with a setter generated.

**Content-based icon detection:**
Figma icon components (component instances wrapping Vector paths) are detected from their internal structure, not their name. The designer's component naming convention is irrelevant.
