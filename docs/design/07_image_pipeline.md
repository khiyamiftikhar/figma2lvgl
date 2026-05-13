# figma2lvgl — Image Pipeline

The image pipeline converts PNG assets from the designer's images folder into LVGL-compatible C arrays and a companion `assets.h` header. It runs before screen file generation and is entirely separate from XML parsing and code generation.

---

## Pipeline Overview

```
images/*.png
      │
      ▼
convert_images()             (imported function — no subprocess)
      │
      ├─ for each PNG:
      │    └─ LVGLImage.py --ofmt C --cf <color_format> -o _lvgl_staging/ <png>
      │         └─ produces <name>.c and <name>.h in staging dir
      │
      ├─ move .c files → ui_src/priv_src/
      ├─ move .h files → ui_src/priv_include/
      └─ generate assets.h → ui_src/priv_include/

main.py (after convert_images returns)
      ├─ fix_lvgl_includes(priv_src/)     ← only if --patch-esp-includes
      └─ copy_static_files()              ← ui_defs.h, ui_style.c/h
```

---

## `LVGLImage.py` — Dependency Management

`LVGLImage.py` is the official LVGL image conversion script (MIT licensed), sourced from the LVGL repository. It is not bundled with figma2lvgl. Resolution order in `find_or_download_lvgl_tool()`:

1. If `--lvgl-tool <path>` was passed: use that path directly. No cache lookup, no download. **Recommended for CI.**
2. Check platform cache: `~/.cache/figma2lvgl/LVGLImage.py` (Linux), `~/Library/Caches/figma2lvgl/LVGLImage.py` (macOS), `%LOCALAPPDATA%\figma2lvgl\LVGLImage.py` (Windows)
3. If cached: return cached path
4. If not cached: prompt to download from the LVGL GitHub repo (auto-confirmed with `--yes`)

**For CI / air-gapped environments:** use `--lvgl-tool ./path/to/LVGLImage.py` to skip the cache and download entirely:
```bash
figma2lvgl -x layout.xml --yes --lvgl-tool ./tools/LVGLImage.py
```

---

## `convert_images()` — Conversion Function

`convert_images(assets_dir, src_dir, inc_dir, lvgl_tool, color_format)` is an importable Python function in `figma2lvgl/tools/image_converter.py`. `main.py` calls it directly — there is no subprocess boundary.

**Parameters:**

| Parameter | Type | Description |
|-----------|------|-------------|
| `assets_dir` | `Path` | Folder containing PNG files |
| `src_dir` | `Path` | Destination for generated `.c` files (`priv_src/`) |
| `inc_dir` | `Path` | Destination for generated `.h` files and `assets.h` (`priv_include/`) |
| `lvgl_tool` | `Path` | Path to `LVGLImage.py` |
| `color_format` | `str` | Pixel encoding format. Default: `"RGB565"`. Configurable via `--color-format`. |

**Returns:** `True` on success, `False` on any failure.

**Steps:**
1. Validates `assets_dir` and `lvgl_tool` exist
2. Creates `priv_src/`, `priv_include/`, and a temporary `_lvgl_staging/` directory
3. For each `.png` in `assets_dir` (sorted):
   - Calls `LVGLImage.py --ofmt C --cf <color_format> -o _lvgl_staging/ <png>`
   - Logs an error and returns `False` if conversion fails
4. Moves `.c` files from staging → `priv_src/`
5. Moves `.h` files from staging → `priv_include/`
6. Deletes the (now empty) staging directory
7. Writes `assets.h`

If no PNG files are found, logs a warning and continues (a screen with no images is valid).

---

## Color Format

The pixel encoding format must match the target display hardware. Controlled via the `--color-format` / `-f` CLI flag:

```bash
figma2lvgl -x layout.xml -f ARGB8888
```

| Format | Bits per pixel | Use case |
|--------|---------------|----------|
| `RGB565` | 16 | Most common TFT displays (default) |
| `RGB888` | 24 | Higher quality TFTs |
| `ARGB8888` | 32 | Displays with alpha channel |
| `L8` | 8 | Greyscale / e-ink |

> This is **not** the same as Figma fill colors. Figma fill colors (hex values like `#4CAF50`) are read from XML and baked into C structs independently — they are not affected by this setting. The color format only affects PNG image asset pixel encoding.

---

## `assets.h` Generation

`_write_assets_header(names, inc_dir)` writes `ui_src/priv_include/assets.h`:

```c
#pragma once

#include "lvgl.h"

LV_IMG_DECLARE(icon_wifi);
// ... one per converted image
```

Every generated screen `.c` file includes `assets.h`, allowing setter functions to reference image descriptors by name (e.g. `c->data.image.src = &icon_wifi`).

---

## LVGL Include Patching (Opt-In)

`LVGLImage.py` generates image `.c` files with a multi-branch include guard whose fallback is `#include "lvgl/lvgl.h"` — which causes a compile error on ESP-IDF.

**Recommended fix for ESP-IDF:** add one line to `CMakeLists.txt`:
```cmake
target_compile_definitions(${COMPONENT_LIB} PUBLIC LV_LVGL_H_INCLUDE_SIMPLE=1)
```
This makes `LVGLImage.py` generate `#include "lvgl.h"` directly — no post-processing needed.

**Fallback (opt-in only):** pass `--patch-esp-includes` to run `fix_lvgl_includes()` after conversion. This function string-replaces the include guard in every generated `.c` file to add an `ESP_PLATFORM` branch. It is fragile — if LVGL changes their include guard format between versions, the patch silently does nothing. Use the CMake flag instead wherever possible.

```bash
figma2lvgl -x layout.xml --patch-esp-includes
```

---

## Asset Validation (pre-pipeline)

Before any conversion, `main.py` validates that all required PNG files exist:

1. Calls `screen.get_required_assets(CHILDREN)` for each screen
2. Collects child IDs where `ChildSpec.requires_asset == True` (currently `WidgetType.IMAGE` only)
3. Checks `<images_dir>/<id>.png` exists for each
4. If any are missing: prints all missing paths and exits — no output written

The pipeline is all-or-nothing: one missing image aborts everything.

---

## Output Files

| File | Location | Description |
|------|----------|-------------|
| `<name>.c` | `priv_src/` | LVGL image descriptor C array |
| `<name>.h` | `priv_include/` | Image descriptor header |
| `assets.h` | `priv_include/` | Master image declarations, included by screen `.c` files |
