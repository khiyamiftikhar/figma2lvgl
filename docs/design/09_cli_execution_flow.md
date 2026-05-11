# figma2lvgl — CLI and Execution Flow

The CLI entry point is `main()` in `figma2lvgl/main.py`. It owns the full pipeline from argument parsing to the final success report. No parsing or generation logic lives here — it only orchestrates.

---

## CLI Arguments

Parsed by `argparse`:

| Flag | Long form | Required | Default | Description |
|------|-----------|----------|---------|-------------|
| `-x` | `--xml` | Yes | — | Path to the Figma XML file |
| `-i` | `--images` | No | Same directory as XML | Folder containing PNG images |
| `-d` | `--dest` | No | Same directory as XML | Destination for `ui_src/` output |

All paths are resolved to absolute paths immediately after parsing.

---

## Output Folder Structure

Built from the resolved destination directory:

```
<dest>/
  ui_src/
    src/            ← generated screen .c files
    include/        ← generated screen .h files
    priv_src/       ← image .c files + ui_style.c
    priv_include/   ← assets.h, ui_defs.h, ui_style.h
```

---

## `main()` — Step-by-Step

### Step 1 — Validate Inputs

```
xml_path    = Path(args.xml).resolve()
images_dir  = Path(args.images).resolve()  or  xml_path.parent
dest_dir    = Path(args.dest).resolve()    or  xml_path.parent
```

- Exits with error if `xml_path` is not a file
- Exits with error if `images_dir` is not a directory

Prints a summary header showing all three resolved paths before proceeding.

---

### Step 2 — Parse XML

```python
tree = ET.parse(xml_path)
root = tree.getroot()
```

- Exits with error if XML is malformed
- Exits with error if `<children>` element is missing from the root
- Exits with error if no `<Frame>` nodes are found inside `<children>`

For each `<Frame>` found, calls `parse_screen(frame)` → list of `ParsedScreen` objects.

---

### Step 3 — Asset Validation

For each parsed screen, collects all child IDs where `ChildSpec.requires_asset == True`. Checks that `<images_dir>/<id>.png` exists for each.

If any assets are missing:
```
==========================================
 ASSET VALIDATION FAILED
==========================================
The following image files are missing:

  - /path/to/images/icon_wifi.png
  - /path/to/images/battery_image.png

Please add the missing files and run again.
==========================================
```
Exits — nothing has been written yet.

---

### Step 4 — Resolve LVGLImage.py

Calls `find_or_download_lvgl_tool()`. Returns the path to a cached `LVGLImage.py`, or downloads it after asking the user. Exits if the user declines and the file is not cached.

---

### Step 5 — Overwrite Guard

Calls `confirm_overwrite(ui_src)`.

- If `ui_src/` does not exist → proceeds silently
- If `ui_src/` exists but all subfolders are empty → proceeds silently
- If `ui_src/` has content in any subfolder → prints a warning listing each subfolder and its file count, then prompts:

```
==========================================
 WARNING: Output folder already has content
==========================================
  /path/to/project/ui_src

  The following subfolders will be wiped and regenerated:
    src/  (2 file(s))
    include/  (2 file(s))
    priv_src/  (5 file(s))
    priv_include/  (4 file(s))

  Overwrite? [y/n]:
```

On `n`, exits without writing anything.

---

### Step 6 — Reset Output Directories

Deletes and recreates all four output subdirectories:
- `ui_src/src/`
- `ui_src/include/`
- `ui_src/priv_src/`
- `ui_src/priv_include/`

This is unconditional after the user confirms. Previous contents are permanently deleted.

---

### Step 7 — Image Conversion

Calls `run_image_converter(images_dir, priv_src, priv_inc, lvgl_tool)`.

Runs `image_converter.py` as a subprocess:
```bash
python image_converter.py <images_dir> <priv_src> <priv_include> <lvgl_tool>
```

- On failure (non-zero returncode), prints full stdout/stderr and exits
- On success, proceeds

---

### Step 8 — LVGL Include Patching

Calls `fix_lvgl_includes(priv_src_dir)`.

Iterates every `.c` file in `priv_src/`. For each file that contains the standard LVGL include guard block, replaces it with the ESP32-compatible version (adds `ESP_PLATFORM` branch).

Only files that actually contain the old block are modified. Prints a line for each patched file.

---

### Step 9 — Copy Static Files

Calls `copy_static_files(priv_src, priv_inc)`.

Copies from `figma2lvgl/static_src/`:
- `*.h` files → `priv_include/`
- `*.c` files → `priv_src/`

Files copied: `ui_defs.h`, `ui_style.h`, `ui_style.c`.

---

### Step 10 — Generate Screen Files

For each `ParsedScreen`:
1. Calls `generate_screen(screen)` → `(c_fname, h_fname, h_text, c_text)`
2. Writes `h_text` → `ui_src/include/<h_fname>`
3. Writes `c_text` → `ui_src/src/<c_fname>`

---

### Step 11 — Summary Report

```
==========================================
 PIPELINE COMPLETED SUCCESSFULLY
==========================================

  ui_src/
    src/          (2 .c files)
    include/      (2 .h files)
    priv_src/     (6 .c files)
    priv_include/ (5 .h files)
```

---

## Error Exit Points Summary

| Point | Condition | Message |
|-------|-----------|---------|
| After arg parsing | XML file not found | `ERROR: XML file not found:` |
| After arg parsing | Images directory not found | `ERROR: Images directory not found:` |
| XML parse | Malformed XML | `ERROR: Failed to parse XML:` |
| XML parse | No `<children>` element | `ERROR: No <children> element found in XML.` |
| XML parse | No `<Frame>` nodes | `ERROR: No <Frame> nodes found in XML.` |
| Asset validation | Missing PNG files | `ASSET VALIDATION FAILED` + list |
| LVGLImage resolution | User declined download | Aborted message |
| Overwrite guard | User answered `n` | `Aborted. No files were changed.` |
| Image conversion | Non-zero subprocess exit | `IMAGE CONVERSION FAILED` + stderr |

---

## CMake Generation (Dormant)

`cmake_generator.py` and `generate_cmake()` exist but are commented out in `main.py`:

```python
# cmake_text = generate_cmake()
# cmake_path = ui_src / "CMakeLists.txt"
# write_file(str(cmake_path), cmake_text)
```

When enabled, it would generate a `CMakeLists.txt` in `ui_src/` using `idf_component_register()` — intended for ESP-IDF projects. The generated paths in `cmake_generator.py` reference `src_generated/` and `runtime/` which no longer match the current output layout (`src/`, `priv_src/`), so it would need updating before being re-enabled.
