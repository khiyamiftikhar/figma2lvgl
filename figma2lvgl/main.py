import sys
import os
import shutil
import subprocess
import argparse
import xml.etree.ElementTree as ET
from pathlib import Path

from figma2lvgl.core.figma_parser import parse_screen
from figma2lvgl.core.generator import generate_screen
from figma2lvgl.core.utils.utils import write_file
from figma2lvgl.core.cmake_generator import generate_cmake
from figma2lvgl.core.child_registry import CHILDREN


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
def confirm_overwrite(ui_src: Path) -> bool:
    """
    Returns True if it's safe to proceed (either folder is fresh,
    or the user explicitly said yes).
    """
    if not ui_src.exists():
        return True  # nothing there yet, no need to ask

    # Check if any of the 4 subfolders exist AND have files in them
    subfolders = ["src", "include", "priv_src", "priv_include"]
    has_content = any(
        (ui_src / sub).is_dir() and any((ui_src / sub).iterdir())
        for sub in subfolders
    )

    if not has_content:
        return True  # folders exist but are empty, safe to proceed

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
        required_assets.update(screen.get_required_assets(CHILDREN))

    missing = []
    for asset_id in required_assets:
        expected_file = os.path.join(images_dir, asset_id + ".png")
        if not os.path.exists(expected_file):
            missing.append(expected_file)

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


# -------------------------------------------------
# Run image converter script with dest folders
# -------------------------------------------------
def run_image_converter(images_dir, priv_src_dir, priv_include_dir):
    script = Path(__file__).parent / "tools" / "image_converter.py"

    if not script.exists():
        print(f"ERROR: image_converter.py not found at {script}")
        return False

    print("\nRunning image conversion...")

    result = subprocess.run(
        [
            sys.executable,       # same Python that's running this script
            str(script),
            str(images_dir),      # -i equivalent: where the PNGs live
            str(priv_src_dir),    # where .c files go
            str(priv_include_dir) # where .h files go
        ],
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        print("\n==========================================")
        print(" IMAGE CONVERSION FAILED")
        print("==========================================")
        print(result.stdout)
        print(result.stderr)
        print("==========================================\n")
        return False

    print("Image conversion completed successfully.")
    return True


# -------------------------------------------------
# Argument parsing
# -------------------------------------------------
def parse_args():
    parser = argparse.ArgumentParser(
        description="LVGL UI Generator — converts Figma XML + images to C/H source files.",
        formatter_class=argparse.RawTextHelpFormatter,
        epilog=(
            "Examples:\n"
            "  Windows:  python main.py -x E:/project/diagram.xml\n"
            "  Linux:    python main.py -x /home/user/project/diagram.xml\n"
            "  Full:     python main.py -x diagram.xml -i ./assets/images -d ./output\n"
        )
    )

    parser.add_argument(
        "-x", "--xml",
        required=True,
        metavar="PATH",
        help="(Required) Path to the diagram XML file."
    )

    parser.add_argument(
        "-i", "--images",
        metavar="PATH",
        default=None,
        help=(
            "Path to the folder containing PNG images.\n"
            "Default: same directory as the XML file."
        )
    )

    parser.add_argument(
        "-d", "--dest",
        metavar="PATH",
        default=None,
        help=(
            "Destination folder where ui_src/ will be created.\n"
            "Default: same directory as the XML file.\n"
            "         This is recommended — output stays next to your project."
        )
    )

    return parser.parse_args()

# -------------------------------------------------
# Copy static headers into priv_include
# -------------------------------------------------
def copy_static_headers(priv_inc: Path) -> bool:
    static_src = Path(__file__).parent / "static_src"

    if not static_src.is_dir():
        print(f"WARNING: static_src/ not found at {static_src}, skipping.")
        return True  # non-fatal, pipeline can still continue

    copied = 0
    for header in static_src.glob("*.h"):
        shutil.copy2(str(header), str(priv_inc / header.name))
        print(f"  Copied {header.name} -> priv_include/")
        copied += 1

    if copied == 0:
        print("WARNING: static_src/ exists but contains no .h files.")

    return True

# -------------------------------------------------
# Main
# -------------------------------------------------
def main():
    args = parse_args()

    # --- Resolve XML path ---
    xml_path = Path(args.xml).resolve()
    if not xml_path.is_file():
        print(f"ERROR: XML file not found: {xml_path}")
        sys.exit(1)

    # --- Resolve images dir (default: cwd) ---
    images_dir = Path(args.images).resolve() if args.images else xml_path.parent
    if not images_dir.is_dir():
        print(f"ERROR: Images directory not found: {images_dir}")
        sys.exit(1)
    
    

    # --- Resolve destination dir (default: folder containing the XML) ---
    dest_dir = Path(args.dest).resolve() if args.dest else xml_path.parent

    # --- Build output folder tree ---
    #
    #  <dest>/ui_src/
    #    src/           ← .c files generated from XML
    #    include/       ← .h files generated from XML
    #    priv_src/      ← .c files from image converter
    #    priv_include/  ← .h files from image converter
    #
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
        print(f"ERROR: Failed to parse XML: {e}")
        sys.exit(1)

    page_children = root.find("children")
    if page_children is None:
        print("ERROR: No <children> element found in XML.")
        sys.exit(1)

    frames = page_children.findall("Frame")
    if not frames:
        print("ERROR: No <Frame> nodes found in XML.")
        sys.exit(1)

    screens = [parse_screen(frame) for frame in frames]

    # --- Validate assets ---
    if not validate_assets(screens, images_dir):
        sys.exit(1)

    # --- Reset output folders ---
      # --- Confirm overwrite if ui_src already exists ---
    if not confirm_overwrite(ui_src):
        sys.exit(0)

    # --- Reset output folders ---
    print("Cleaning output folders...")
    for folder in [src_dir, include_dir, priv_src, priv_inc]:
        reset_directory(folder)

    # --- Run image converter ---
    if not run_image_converter(images_dir, priv_src, priv_inc):
        sys.exit(1)

    # --- Copy static headers ---
    print("\nCopying static headers...")
    if not copy_static_headers(priv_inc):
        sys.exit(1)

    # --- Generate UI source files ---
    print("\nGenerating UI files...\n")
    generated = []

    for screen in screens:
        c_fname, h_fname, h_text, c_text = generate_screen(screen)

        c_path = src_dir     / c_fname
        h_path = include_dir / h_fname

        write_file(str(h_path), h_text)
        write_file(str(c_path), c_text)
        generated.append((c_path, h_path))

    print("Generated files:")
    for c, h in generated:
        print(f"  - {c}")
        print(f"  - {h}")

    # --- Generate CMake ---
    cmake_text = generate_cmake()
    cmake_path = ui_src / "CMakeLists.txt"
    write_file(str(cmake_path), cmake_text)

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