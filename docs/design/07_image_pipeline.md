# figma2lvgl — Image Pipeline

The image pipeline converts PNG assets from the designer's images folder into LVGL-compatible C arrays and a companion `assets.h` header. It is entirely separate from XML parsing and code generation, and runs as a subprocess before the main generation step.

---

## Pipeline Overview

```
images/*.png
      │
      ▼
image_converter.py          (called as subprocess by main.py)
      │
      ├─ for each PNG:
      │    └─ LVGLImage.py --ofmt C --cf RGB565 -o _lvgl_staging/ <png>
      │         └─ produces <name>.c and <name>.h in staging dir
      │
      ├─ move .c files → ui_src/priv_src/
      ├─ move .h files → ui_src/priv_include/
      └─ generate assets.h → ui_src/priv_include/
      
main.py (after image_converter returns)
      └─ fix_lvgl_includes(priv_src/)
           └─ patches ESP32 include guard in each .c file
```

---

## `LVGLImage.py` — Dependency Management

`LVGLImage.py` is the official LVGL image conversion script, sourced from the LVGL repository. It is **not bundled** with figma2lvgl to avoid licensing and versioning issues. Instead, it is fetched on first run and cached locally.

### Resolution Order

`find_or_download_lvgl_tool()` in `main.py`:

1. Checks the platform-specific user cache directory for `figma2lvgl/LVGLImage.py`
   - Linux: `~/.cache/figma2lvgl/LVGLImage.py`
   - macOS: `~/Library/Caches/figma2lvgl/LVGLImage.py`
   - Windows: `%LOCALAPPDATA%\figma2lvgl\LVGLImage.py`
2. If found, returns the cached path immediately
3. If not found, prints the source URL and asks the user:
   ```
   figma2lvgl requires LVGLImage.py from the LVGL project (MIT licensed).
   Source: https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py

   Download and cache it now? [y/n]:
   ```
4. On `y`, downloads from `https://raw.githubusercontent.com/lvgl/lvgl/master/scripts/LVGLImage.py` and saves to cache
5. On `n`, aborts with instructions to place it manually

The cached path is passed to `image_converter.py` as `sys.argv[4]`. `image_converter.py` never searches for the file itself — it trusts the path it receives.

---

## `image_converter.py` — Conversion Script

Called as a subprocess:
```bash
python image_converter.py <images_dir> <priv_src_dir> <priv_include_dir> <lvgl_tool_path>
```

Steps:

1. Validates `images_dir` and `lvgl_tool` exist
2. Creates `priv_src_dir`, `priv_include_dir`, and a temporary `_lvgl_staging/` directory
3. For each `.png` in `images_dir`:
   - Runs `LVGLImage.py --ofmt C --cf RGB565 -o _lvgl_staging/ <png>`
   - Conversion format is hardcoded to `RGB565`
4. Moves all `.c` files from staging → `priv_src_dir`
5. Moves all `.h` files from staging → `priv_include_dir`
6. Deletes the (now empty) staging directory
7. Calls `generate_assets_header()` → writes `assets.h`

If no PNG files are found in `images_dir`, a warning is printed but processing continues (a screen with no images is valid).

---

## `assets.h` Generation

`generate_assets_header(names, inc_dir)` writes `ui_src/priv_include/assets.h`:

```c
#pragma once

#include "lvgl.h"

LV_IMG_DECLARE(icon_wifi);
LV_IMG_DECLARE(battery_bar);
// ... one per converted image
```

`names` is the list of PNG stem names (filename without extension) collected during conversion. The generated `assets.h` is included by every generated screen `.c` file so setter functions can reference image descriptors by name.

---

## LVGL Include Patching

`LVGLImage.py` generates image `.c` files with a multi-branch include guard:

```c
#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
#include <lvgl.h>
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"    ← causes compile error on ESP32
#endif
```

On ESP-IDF the correct include is `"lvgl.h"` (not `"lvgl/lvgl.h"`). After image conversion, `fix_lvgl_includes(priv_src_dir)` in `main.py` searches every `.c` file in `priv_src/` and replaces the block above with an extended version that adds an `ESP_PLATFORM` branch:

```c
#elif defined(ESP_PLATFORM)
#include "lvgl.h"
```

This patch is applied unconditionally on every run. It only modifies files where the exact old block is found.

---

## Asset Validation (pre-pipeline)

Before any conversion runs, `main.py` validates that all required PNG files are present:

1. Calls `screen.get_required_assets(CHILDREN)` for each parsed screen
2. Collects all child IDs where `ChildSpec.requires_asset == True` (currently only `UI_CHILD_IMAGE`)
3. For each collected ID, checks that `<images_dir>/<id>.png` exists
4. If any are missing, prints all missing paths and exits — no output is written

This means the pipeline is all-or-nothing: if one image is missing, nothing is generated.

---

## Color Format

The conversion color format is hardcoded to `RGB565` in `image_converter.py`:

```python
COLOR_FORMAT = "RGB565"
```

This matches the most common embedded display color depth. To change it (e.g. for a 32-bit display), modify this constant.

---

## Output Files

| File | Location | Description |
|------|----------|-------------|
| `<name>.c` | `priv_src/` | LVGL image descriptor C array |
| `<name>.h` | `priv_include/` | Image descriptor header (included by `assets.h`) |
| `assets.h` | `priv_include/` | Master image declarations header, included by screen `.c` files |
