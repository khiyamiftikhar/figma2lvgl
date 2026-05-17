import subprocess
import sys
import shutil
import logging
from pathlib import Path

logger = logging.getLogger(__name__)

DEFAULT_COLOR_FORMAT = "RGB565"


def convert_images(
    assets_dir: Path,
    src_dir: Path,
    inc_dir: Path,
    lvgl_tool: Path,
    color_format: str = DEFAULT_COLOR_FORMAT,
) -> bool:
    """
    Convert all PNGs in assets_dir to LVGL C arrays using LVGLImage.py.
    Writes .c files to src_dir, .h files to inc_dir, generates assets.h.
    Returns True on success, False on any failure.
    """
    if not assets_dir.is_dir():
        logger.error("Images directory not found: %s", assets_dir)
        return False

    if not lvgl_tool.is_file():
        logger.error("LVGLImage.py not found at: %s", lvgl_tool)
        return False

    src_dir.mkdir(parents=True, exist_ok=True)
    inc_dir.mkdir(parents=True, exist_ok=True)

    staging_dir = src_dir.parent / "_lvgl_staging"
    staging_dir.mkdir(parents=True, exist_ok=True)

    image_names = []
    png_files = sorted(assets_dir.glob("*.png"))

    if not png_files:
        logger.warning("No PNG files found in %s", assets_dir)

    for png in png_files:
        logger.info("Converting %s ...", png.name)
        cmd = [
            sys.executable,
            str(lvgl_tool),
            "--ofmt", "C",
            "--cf", color_format,
            "-o", str(staging_dir),
            str(png),
        ]
        result = subprocess.run(cmd, capture_output=True, text=True)
        if result.returncode != 0:
            logger.error(
                "LVGLImage.py failed on %s:\n%s", png.name, result.stderr
            )
            return False
        image_names.append(png.stem)

    for file in staging_dir.iterdir():
        dest = src_dir if file.suffix == ".c" else inc_dir
        dest_path = dest / file.name
        shutil.move(str(file), str(dest_path))
        if file.suffix == ".c":
            _patch_lvgl_includes(dest_path)
    staging_dir.rmdir()

    _write_assets_header(image_names, inc_dir)
    logger.info("Image conversion done — %d image(s) converted.", len(image_names))
    return True


def _patch_lvgl_includes(c_file: Path):
    """
    Insert an ESP_PLATFORM branch into the LVGLImage.py include block.

    Adds one new #elif immediately before the #else fallback, leaving every
    other branch exactly as LVGLImage.py emitted it. Only ESP_PLATFORM is
    affected — all other platforms take the same path they always did.

    Before (LVGLImage.py original):
        #if defined(LV_LVGL_H_INCLUDE_SIMPLE)
        #include "lvgl.h"
        #elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
        #include <lvgl.h>
        #elif defined(LV_BUILD_TEST)
        #include "../lvgl.h"
        #else
        #include "lvgl/lvgl.h"   ← fires on ESP-IDF, wrong path
        #endif

    After:
        #if defined(LV_LVGL_H_INCLUDE_SIMPLE)
        #include "lvgl.h"
        #elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
        #include <lvgl.h>
        #elif defined(LV_BUILD_TEST)
        #include "../lvgl.h"
        #elif defined(ESP_PLATFORM)
        #include "lvgl.h"        ← ESP gets its own explicit lane
        #else
        #include "lvgl/lvgl.h"   ← fallback unchanged for everyone else
        #endif
    """
    OLD = (
        '#else\n'
        '#include "lvgl/lvgl.h"\n'
        '#endif'
    )
    NEW = (
        '#elif defined(ESP_PLATFORM)\n'
        '#include "lvgl.h"\n'
        '#else\n'
        '#include "lvgl/lvgl.h"\n'
        '#endif'
    )
    text = c_file.read_text(encoding="utf-8")
    if OLD in text:
        c_file.write_text(text.replace(OLD, NEW, 1), encoding="utf-8")
        logger.debug("Patched LVGL include block in %s", c_file.name)
    else:
        logger.debug("Include block not found in %s — skipping patch", c_file.name)


def _write_assets_header(names, inc_dir: Path):
    header = inc_dir / "assets.h"
    with open(header, "w") as f:
        f.write("#pragma once\n\n")
        f.write('#include "lvgl.h"\n\n')
        for name in names:
            f.write(f"LV_IMG_DECLARE({name});\n")
    logger.info("Generated assets.h with %d image(s).", len(names))


# Keep CLI entry point for standalone use / debugging
def main():
    if len(sys.argv) < 5:
        print(
            "Usage: image_converter.py <images_dir> <priv_src_dir> "
            "<priv_include_dir> <lvgl_tool_path> [color_format]"
        )
        sys.exit(1)

    ok = convert_images(
        assets_dir   = Path(sys.argv[1]).resolve(),
        src_dir      = Path(sys.argv[2]).resolve(),
        inc_dir      = Path(sys.argv[3]).resolve(),
        lvgl_tool    = Path(sys.argv[4]).resolve(),
        color_format = sys.argv[5] if len(sys.argv) > 5 else DEFAULT_COLOR_FORMAT,
    )
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
