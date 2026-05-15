import sys
import os
import shutil
import subprocess
import argparse
import logging
import xml.etree.ElementTree as ET
from pathlib import Path

from figma2lvgl.core.figma_parser import parse_screen
from figma2lvgl.core.generator import generate_screen
from figma2lvgl.core.utils.utils import write_file
from figma2lvgl.core.config_writer import write_ui_config
from figma2lvgl.tools.image_converter import convert_images

logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
logger = logging.getLogger(__name__)





#The image converion code downloaded and used from offical lvgl repo, has header inclusion
#which falss back to "lvgl/lvgl.h" which causes compile error for esp32. it relaces that
# block after generation
#
def fix_lvgl_includes(priv_src_dir):
    old_block = '''#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
#include <lvgl.h>
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif'''

    new_block = '''#if defined(LV_LVGL_H_INCLUDE_SIMPLE)
#include "lvgl.h"
#elif defined(LV_LVGL_H_INCLUDE_SYSTEM)
#include <lvgl.h>
#elif defined(LV_BUILD_TEST)
#include "../lvgl.h"
#elif defined(ESP_PLATFORM)
#include "lvgl.h"
#else
#include "lvgl/lvgl.h"
#endif'''

    for c_file in Path(priv_src_dir).glob("*.c"):
        with open(c_file, "r", encoding="utf-8") as f:
            content = f.read()

        if old_block in content:
            content = content.replace(old_block, new_block)

            with open(c_file, "w", encoding="utf-8") as f:
                f.write(content)

            print(f"  Patched LVGL include block in {c_file.name}")


# -------------------------------------------------
# Clean directory completely and recreate it
# -------------------------------------------------
# -------------------------------------------------
# Clean directory completely and recreate it
# -------------------------------------------------
def reset_directory(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path, exist_ok=True)


# -------------------------------------------------
# Check if ui_src already has content and ask user
# -------------------------------------------------
def confirm_overwrite(ui_src: Path, auto_yes: bool = False) -> bool:
    """
    Returns True if it's safe to proceed (folder is fresh, empty,
    or user confirmed / --yes flag was passed).
    """
    if not ui_src.exists():
        return True

    subfolders = ["src", "include", "priv_src", "priv_include"]
    has_content = any(
        (ui_src / sub).is_dir() and any((ui_src / sub).iterdir())
        for sub in subfolders
    )

    if not has_content:
        return True

    if auto_yes:
        logger.info("--yes flag set: overwriting existing output in %s", ui_src)
        return True

    print("\n==========================================")
    print(" WARNING: Output folder already has content")
    print("==========================================")
    print(f"  {ui_src}")
    print("\n  The following subfolders will be wiped and regenerated:")
    for sub in subfolders:
        folder = ui_src / sub
        if folder.is_dir():
            count = len(list(folder.iterdir()))
            print(f"    {sub}/  ({count} file(s))")
    print()

    while True:
        answer = input("  Overwrite? [y/n]: ").strip().lower()
        if answer in ("y", "yes"):
            return True
        if answer in ("n", "no"):
            print("\nAborted. No files were changed.\n")
            return False
        print("  Please enter y or n.")

# -------------------------------------------------
# Validate required assets exist
# -------------------------------------------------
def validate_assets(screens, images_dir):
    required_assets = set()
    for screen in screens:
        required_assets.update(screen.get_required_assets())   # v0.4.0: no registry arg

    missing = [
        os.path.join(images_dir, asset_id + ".png")
        for asset_id in required_assets
        if not os.path.exists(os.path.join(images_dir, asset_id + ".png"))
    ]

    if missing:
        print("\n==========================================")
        print(" ASSET VALIDATION FAILED")
        print("==========================================")
        print("The following image files are missing:\n")
        for m in missing:
            print("  -", m)
        print("\nPlease add the missing files and run again.")
        print("==========================================\n")
        return False

    return True


# run_image_converter removed — FIX-8: use convert_images() directly


def _verify_compile(src_dir: Path, include_dir: Path, priv_inc: Path):
    """
    Syntax-check generated C files using gcc -fsyntax-only.
    Does not link or run — just checks that the C is syntactically valid.
    Exits with code 1 if any file fails.

    Requires gcc on PATH. A stub lvgl.h is not required for syntax checking
    since we pass -DLVGL_COMPILE_CHECK to suppress #include resolution.
    In practice, most LVGL type errors are caught regardless.
    """
    import shutil
    if not shutil.which("gcc"):
        logger.warning("--verify-compile: gcc not found on PATH, skipping.")
        return

    c_files = sorted(src_dir.glob("*.c"))
    if not c_files:
        logger.warning("--verify-compile: no .c files found in %s", src_dir)
        return

    cmd = [
        "gcc", "-fsyntax-only",
        f"-I{include_dir}",
        f"-I{priv_inc}",
        "-DLVGL_COMPILE_CHECK",
    ] + [str(f) for f in c_files]

    logger.debug("Compile check: %s", " ".join(cmd))
    result = subprocess.run(cmd, capture_output=True, text=True)

    if result.returncode != 0:
        logger.error("Compile check FAILED:\n%s", result.stderr)
        sys.exit(1)

    logger.info("Compile check passed (%d file(s)).", len(c_files))
# -------------------------------------------------
# Argument parsing
# -------------------------------------------------
def parse_args():
    parser = argparse.ArgumentParser(
        description="LVGL UI Generator — converts Figma XML + images to C/H source files.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog=(
            "Examples:\n"
            "  Basic:      figma2lvgl -x layout.xml\n"
            "  Full:       figma2lvgl -x layout.xml -i ./assets -d ./output\n"
            "  CI:         figma2lvgl -x layout.xml --yes --lvgl-tool ./tools/LVGLImage.py\n"
            "  Air-gapped: figma2lvgl -x layout.xml -y --lvgl-tool /ci/tools/LVGLImage.py\n"
        )
    )

    parser.add_argument(
        "-x", "--xml",
        required=True, metavar="PATH",
        help="(Required) Path to the Figma XML file exported by FigML."
    )
    parser.add_argument(
        "-i", "--images",
        metavar="PATH", default=None,
        help="Folder containing PNG image assets.\nDefault: same directory as the XML file."
    )
    parser.add_argument(
        "-d", "--dest",
        metavar="PATH", default=None,
        help="Destination folder where ui_src/ will be created.\nDefault: same directory as the XML file."
    )
    parser.add_argument(
        "-y", "--yes",
        action="store_true", default=False,
        help=(
            "Skip all interactive prompts.\n"
            "Assumes yes to overwrite and auto-downloads LVGLImage.py if not cached.\n"
            "For air-gapped CI use --lvgl-tool instead of relying on download."
        )
    )
    parser.add_argument(
        "--lvgl-tool",
        metavar="PATH", default=None, type=Path,
        help=(
            "Path to LVGLImage.py. Skips cache lookup and download prompt.\n"
            "Use in CI to pin the tool version or in air-gapped environments."
        )
    )
    parser.add_argument(
        "-f", "--color-format",
        choices=["RGB565", "RGB888", "ARGB8888", "L8"],
        default="RGB565", metavar="FMT",
        help=(
            "Pixel encoding for PNG image asset conversion.\n"
            "Must match your display hardware color depth.\n"
            "Options: RGB565, RGB888, ARGB8888, L8.  Default: RGB565."
        )
    )
    parser.add_argument(
        "--patch-esp-includes",
        action="store_true", default=False,
        help=(
            "Patch LVGL include guards in generated image .c files for ESP-IDF.\n"
            "Prefer setting LV_LVGL_H_INCLUDE_SIMPLE=1 in your CMakeLists.txt instead."
        )
    )
    parser.add_argument(
        "-v", "--verbose",
        action="store_true", default=False,
        help="Enable debug-level logging."
    )
    parser.add_argument(
        "--verify-compile",
        action="store_true", default=False,
        help=(
            "After generation, syntax-check generated C files using gcc.\n"
            "Requires gcc on PATH. Checks syntax only — does not link or run.\n"
            "Useful for catching template or substitution bugs early."
        )
    )

    return parser.parse_args()

# -------------------------------------------------
# Copy static files into priv_include
# -------------------------------------------------
def copy_static_files(priv_src: Path, priv_inc: Path) -> bool:
    static_src = Path(__file__).parent / "static_src"

    if not static_src.is_dir():
        logger.warning("static_src/ not found at %s, skipping.", static_src)
        return True

    for header in static_src.glob("*.h"):
        shutil.copy2(str(header), str(priv_inc / header.name))
        logger.info("Copied %s -> priv_include/", header.name)

    for source in static_src.glob("*.c"):
        shutil.copy2(str(source), str(priv_src / source.name))
        logger.info("Copied %s -> priv_src/", source.name)

    return True

#-- Dependencies
def find_or_download_lvgl_tool(
    auto_yes: bool = False,
    tool_override: Path = None,
) -> Path:
    """
    Resolve LVGLImage.py path.
    --lvgl-tool bypasses cache and download entirely (best for CI).
    --yes auto-confirms the download prompt.
    """
    if tool_override is not None:
        if not tool_override.is_file():
            logger.error("--lvgl-tool path not found: %s", tool_override)
            return None
        logger.info("Using LVGLImage.py from --lvgl-tool: %s", tool_override)
        return tool_override

    import platformdirs
    cache_dir = Path(platformdirs.user_cache_dir("figma2lvgl"))
    cached = cache_dir / "LVGLImage.py"

    if cached.is_file():
        return cached

    print("\n  figma2lvgl requires LVGLImage.py from the LVGL project (MIT licensed).")
    print("  Source: https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py")
    print()

    if auto_yes:
        answer = "y"
        logger.info("--yes flag set: auto-downloading LVGLImage.py")
    else:
        answer = input("  Download and cache it now? [y/n]: ").strip().lower()

    if answer not in ("y", "yes"):
        print("  Aborted. Use --lvgl-tool <path> to provide it manually.")
        return None

    import urllib.request
    url = "https://raw.githubusercontent.com/lvgl/lvgl/master/scripts/LVGLImage.py"
    cache_dir.mkdir(parents=True, exist_ok=True)
    print("  Downloading...")
    urllib.request.urlretrieve(url, cached)
    logger.info("Saved to %s", cached)
    return cached

#############
# -------------------------------------------------
# Main
# -------------------------------------------------
def main():
    args = parse_args()

    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)

    # --- Resolve paths ---
    xml_path = Path(args.xml).resolve()
    if not xml_path.is_file():
        logger.error("XML file not found: %s", xml_path)
        sys.exit(1)

    images_dir = Path(args.images).resolve() if args.images else xml_path.parent
    if not images_dir.is_dir():
        logger.error("Images directory not found: %s", images_dir)
        sys.exit(1)

    dest_dir = Path(args.dest).resolve() if args.dest else xml_path.parent

    ui_src      = dest_dir  / "ui_src"
    src_dir     = ui_src    / "src"
    include_dir = ui_src    / "include"
    priv_src    = ui_src    / "priv_src"
    priv_inc    = ui_src    / "priv_include"

    print("\n==========================================")
    print(" LVGL UI Generator")
    print("==========================================")
    print(f"  XML file   : {xml_path}")
    print(f"  Images dir : {images_dir}")
    print(f"  Output dir : {ui_src}")
    print("==========================================\n")

    # --- Parse XML ---
    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
    except Exception as e:
        logger.error("Failed to parse XML: %s", e)
        sys.exit(1)

    page_children = root.find("children")
    if page_children is None:
        logger.error("No <children> element found in XML.")
        sys.exit(1)

    frames = page_children.findall("Frame")
    if not frames:
        logger.error("No <Frame> nodes found in XML.")
        sys.exit(1)

    screens = [parse_screen(frame) for frame in frames]

    # --- Validate assets ---
    if not validate_assets(screens, images_dir):
        sys.exit(1)

    # --- Resolve LVGLImage.py ---
    lvgl_tool = find_or_download_lvgl_tool(
        auto_yes=args.yes,
        tool_override=args.lvgl_tool,
    )
    if lvgl_tool is None:
        sys.exit(1)

    # --- Confirm overwrite ---
    if not confirm_overwrite(ui_src, auto_yes=args.yes):
        sys.exit(0)

    # --- Reset output folders ---
    logger.info("Cleaning output folders...")
    for folder in [src_dir, include_dir, priv_src, priv_inc]:
        reset_directory(folder)

    # --- Run image converter ---
    if not convert_images(
        images_dir, priv_src, priv_inc, lvgl_tool,
        color_format=args.color_format,
    ):
        sys.exit(1)

    # --- ESP include patching (opt-in only) ---
    if args.patch_esp_includes:
        fix_lvgl_includes(priv_src)
    # else: set LV_LVGL_H_INCLUDE_SIMPLE=1 in your CMakeLists.txt

    # --- Copy static files ---
    logger.info("Copying static files...")
    if not copy_static_files(priv_src, priv_inc):
        sys.exit(1)

    # --- Generate ui_config.h ---
    largest = write_ui_config(screens, priv_inc)
    logger.info("ui_config.h written (largest screen: %s)", largest)

    # --- Generate screen files ---
    logger.info("Generating UI files...")
    for screen in screens:
        c_fname, h_fname, h_text, c_text = generate_screen(screen)
        write_file(str(include_dir / h_fname), h_text)
        write_file(str(src_dir     / c_fname), c_text)
        logger.info("  %s  %s", c_fname, h_fname)

    # --- Optional compile check (--verify-compile) ---
    if args.verify_compile:
        _verify_compile(src_dir, include_dir, priv_inc)

    print("\n==========================================")
    print(" PIPELINE COMPLETED SUCCESSFULLY")
    print("==========================================")
    print(f"\n  ui_src/")
    print(f"    src/          ({len(list(src_dir.glob('*.c')))} .c files)")
    print(f"    include/      ({len(list(include_dir.glob('*.h')))} .h files)")
    print(f"    priv_src/     ({len(list(priv_src.glob('*.c')))} .c files)")
    print(f"    priv_include/ ({len(list(priv_inc.glob('*.h')))} .h files)")
    print()


if __name__ == "__main__":
    main()