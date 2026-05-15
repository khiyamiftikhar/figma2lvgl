# figma2lvgl — CLI and Execution Flow

The CLI entry point is `main()` in `figma2lvgl/main.py`. It owns the full pipeline from argument parsing to the final summary. All parsing and generation logic lives in other modules — `main.py` only orchestrates and handles errors.

---

## CLI Arguments

Parsed by `argparse`. Run `figma2lvgl --help` for the full help text.

| Flag | Required | Default | Description |
|------|----------|---------|-------------|
| `-x` / `--xml` | Yes | — | Path to Figma XML file exported by FigML |
| `-i` / `--images` | No | Same dir as XML | Folder containing PNG image assets |
| `-d` / `--dest` | No | Same dir as XML | Destination folder where `ui_src/` is created |
| `-y` / `--yes` | No | `False` | Skip all interactive prompts; auto-downloads LVGLImage.py if not cached |
| `--lvgl-tool` | No | `None` | Direct path to LVGLImage.py — skips cache lookup and download entirely |
| `-f` / `--color-format` | No | `RGB565` | Pixel encoding for PNG→LVGL conversion: `RGB565`, `RGB888`, `ARGB8888`, `L8` |
| `--patch-esp-includes` | No | `False` | Patch LVGL include guards in generated image `.c` files for ESP-IDF (opt-in; prefer `LV_LVGL_H_INCLUDE_SIMPLE=1` in CMake) |
| `-v` / `--verbose` | No | `False` | Enable DEBUG-level logging |

**CI usage patterns:**

```bash
# Standard interactive use
figma2lvgl -x layout.xml

# CI with network access (auto-downloads LVGLImage.py if not cached)
figma2lvgl -x layout.xml --yes

# CI air-gapped or pinned version
figma2lvgl -x layout.xml --yes --lvgl-tool ./tools/LVGLImage.py

# Higher-end TFT display
figma2lvgl -x layout.xml -f ARGB8888

# ESP-IDF without cmake flag
figma2lvgl -x layout.xml --patch-esp-includes
```

---

## Output Folder Structure

```
<dest>/
  ui_src/
    src/            ← generated screen .c files
    include/        ← generated screen .h files
    priv_src/       ← image .c files + ui_style.c
    priv_include/   ← assets.h, ui_config.h, ui_defs.h, ui_style.h
```

---

## `main()` — Step-by-Step

### Step 1 — Verbose flag

```python
if args.verbose:
    logging.getLogger().setLevel(logging.DEBUG)
```

Enables debug log output for the rest of the pipeline.

### Step 2 — Resolve paths

All three paths (XML, images dir, dest dir) are resolved to absolute paths. Errors here:

| Condition | Error message |
|-----------|--------------|
| XML file not found | `logger.error("XML file not found: ...")` + `sys.exit(1)` |
| Images directory not found | `logger.error("Images directory not found: ...")` + `sys.exit(1)` |

### Step 3 — Print header

UX banner printed with resolved paths (always, regardless of log level).

### Step 4 — Parse XML

```python
tree = ET.parse(xml_path)
root = tree.getroot()
page_children = root.find("children")
frames = page_children.findall("Frame")
screens = [parse_screen(frame) for frame in frames]
```

Errors:

| Condition | Error |
|-----------|-------|
| Malformed XML | `logger.error("Failed to parse XML: ...")` + `sys.exit(1)` |
| No `<children>` element | `logger.error("No <children> element found in XML.")` + `sys.exit(1)` |
| No `<Frame>` nodes | `logger.error("No <Frame> nodes found in XML.")` + `sys.exit(1)` |

Unrecognized child nodes (no type keyword in name) are skipped with a `WARNING` log that includes the screen name — no error, pipeline continues.

### Step 5 — Asset validation

Calls `validate_assets(screens, images_dir)`. Checks `<images_dir>/<child_id>.png` exists for every child where `ChildSpec.requires_asset == True`. If any are missing, prints the missing paths and exits. No output has been written yet.

### Step 6 — Resolve LVGLImage.py

Calls `find_or_download_lvgl_tool(auto_yes=args.yes, tool_override=args.lvgl_tool)`:

- `--lvgl-tool` provided → validates and returns that path; exits if not found
- Cached → returns cached path immediately
- Not cached + `--yes` → downloads automatically
- Not cached + interactive → prompts; exits on `n`

### Step 7 — Confirm overwrite

Calls `confirm_overwrite(ui_src, auto_yes=args.yes)`:

- `ui_src/` doesn't exist → proceed silently
- `ui_src/` exists but all subfolders empty → proceed silently
- `ui_src/` has content + `--yes` → proceed, logs a message
- `ui_src/` has content + interactive → prints subfolder file counts, prompts; exits on `n`

### Step 8 — Reset output directories

Deletes and recreates `src/`, `include/`, `priv_src/`, `priv_include/`. Unconditional after step 7 confirms.

### Step 9 — Image conversion

```python
convert_images(images_dir, priv_src, priv_inc, lvgl_tool,
               color_format=args.color_format)
```

Direct function call — no subprocess. Uses `args.color_format` (default `"RGB565"`). Exits on failure.

### Step 10 — ESP include patching (opt-in)

```python
if args.patch_esp_includes:
    fix_lvgl_includes(priv_src)
```

Only runs when `--patch-esp-includes` is passed. Off by default. For ESP-IDF users the recommended path is `LV_LVGL_H_INCLUDE_SIMPLE=1` in `CMakeLists.txt`.

### Step 11 — Copy static files

`copy_static_files(priv_src, priv_inc)` copies `static_src/*.h` → `priv_include/` and `static_src/*.c` → `priv_src/`. Files: `ui_defs.h`, `ui_style.h`, `ui_style.c`.

### Step 12 — Generate `ui_config.h`

```python
write_ui_config(screens, priv_inc)
```

Writes `priv_include/ui_config.h` with `UI_MAX_STRING_LENGTH`, `UI_MAX_ID_LENGTH`, and `UI_MAX_ICON_STATES`. There is no `UI_MAX_CHILDREN` — the per-screen struct is always exactly the right size for the design, with no generic array involved.

### Step 13 — Generate screen files

For each `ParsedScreen`:
```python
c_fname, h_fname, h_text, c_text = generate_screen(screen)
write_file(str(include_dir / h_fname), h_text)
write_file(str(src_dir     / c_fname), c_text)
```

### Step 14 — Summary

UX banner with file counts per subfolder and total screens generated.

---

## Logging vs Print

The pipeline uses two output mechanisms:

| Type | Used for |
|------|----------|
| `logger.error/warning/info/debug` | All diagnostic messages — errors, warnings, progress |
| `print()` | UX banners (`===...===`), the initial path summary, the final summary, interactive prompts |

This means `--verbose` enables more `logger.*` output but does not affect the UX banners. `2>/dev/null` suppresses only `logger.*` output; banners still print.

---

## Error Exit Points Summary

| Step | Condition | Message |
|------|-----------|---------|
| 2 | XML file not found | `logger.error` + exit 1 |
| 2 | Images directory not found | `logger.error` + exit 1 |
| 4 | Malformed XML | `logger.error` + exit 1 |
| 4 | No `<children>` in XML | `logger.error` + exit 1 |
| 4 | No `<Frame>` nodes | `logger.error` + exit 1 |
| 5 | Missing PNG assets | Print banner + exit 1 |
| 6 | `--lvgl-tool` path not found | `logger.error` + exit 1 |
| 6 | User declined download | Print message + return `None` → exit 1 |
| 7 | User declined overwrite | Print "Aborted" + exit 0 |
| 9 | Image conversion failure | `logger.error` from `convert_images()` → exit 1 |
