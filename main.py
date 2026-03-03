import sys
import os
import shutil
import subprocess
import xml.etree.ElementTree as ET

from core.figma_parser import parse_screen
from core.generator import generate_screen
from core.utils.utils import write_file
from core.cmake_generator import generate_cmake
from core.child_registry import CHILDREN


OUTPUT_DIR = os.path.join("ui_component", "src_generated")
IMAGES_DIR = os.path.join("ui_component", "assets", "images")
IMAGE_CONVERTER_SCRIPT = os.path.join("tools", "image_converter.py")

# If your image converter outputs generated .c/.h files,
# define its output directory here:
IMAGE_OUTPUT_DIR = os.path.join("ui_component", "assets_generated")


# -------------------------------------------------
# Clean directory completely and recreate it
# -------------------------------------------------
def reset_directory(path):
    if os.path.exists(path):
        shutil.rmtree(path)
    os.makedirs(path, exist_ok=True)


# -------------------------------------------------
# Validate required assets exist (NO exceptions)
# -------------------------------------------------
def validate_assets(screens):
    required_assets = set()

    for screen in screens:
        required_assets.update(screen.get_required_assets(CHILDREN))

    missing = []

    for asset_id in required_assets:
        expected_file = os.path.join(IMAGES_DIR, asset_id + ".png")
        if not os.path.exists(expected_file):
            missing.append(expected_file)

    if missing:
        print("\n==========================================")
        print(" ASSET VALIDATION FAILED")
        print("==========================================")
        print("The following image files are missing:\n")

        for m in missing:
            print("  -", m)

        print("\nPlease add the missing files and run the generator again.")
        print("==========================================\n")
        return False

    return True


# -------------------------------------------------
# Run external script safely (NO traceback)
# -------------------------------------------------
def run_script(script_path, description):
    print(f"\nRunning {description}...")

    result = subprocess.run(
        ["python", script_path],
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        print("\n==========================================")
        print(f" {description.upper()} FAILED")
        print("==========================================")
        print(result.stdout)
        print(result.stderr)
        print("==========================================\n")
        return False

    print(f"{description} completed successfully.")
    return True


# -------------------------------------------------
# Main
# -------------------------------------------------
def main():
    if len(sys.argv) < 2:
        print("Usage: python parser.py diagram.xml")
        sys.exit(1)

    xml_path = sys.argv[1]

    if not os.path.isfile(xml_path):
        print("File not found:", xml_path)
        sys.exit(1)

    # -------------------------------------------------
    # Parse XML
    # -------------------------------------------------
    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
    except Exception as e:
        print("Failed to parse XML:", e)
        sys.exit(1)

    page_children = root.find("children")
    if page_children is None:
        print("No <children> found in XML.")
        sys.exit(1)

    frames = [n for n in page_children.findall("Frame")]
    if not frames:
        print("No <Frame> nodes found in XML.")
        sys.exit(1)

    screens = [parse_screen(frame) for frame in frames]

    # -------------------------------------------------
    # Validate assets
    # -------------------------------------------------
    if not validate_assets(screens):
        sys.exit(1)

    # -------------------------------------------------
    # Reset generated folders BEFORE generation
    # -------------------------------------------------
    print("\nCleaning generated folders...")
    reset_directory(OUTPUT_DIR)
    reset_directory(IMAGE_OUTPUT_DIR)

    # -------------------------------------------------
    # Run image conversion
    # -------------------------------------------------
    if not run_script(IMAGE_CONVERTER_SCRIPT, "image conversion script"):
        sys.exit(1)

    # -------------------------------------------------
    # Generate UI source/header files
    # -------------------------------------------------
    print("\nGenerating UI files...\n")

    generated = []

    for screen in screens:
        c_fname, h_fname, h_text, c_text = generate_screen(screen)

        c_path = os.path.join(OUTPUT_DIR, c_fname)
        h_path = os.path.join(OUTPUT_DIR, h_fname)

        write_file(h_path, h_text)
        write_file(c_path, c_text)

        generated.append((c_path, h_path))

    print("Generated files:")
    for c, h in generated:
        print(f"  - {c}")
        print(f"  - {h}")

    # -------------------------------------------------
    # Generate CMake
    # -------------------------------------------------
    cmake_text = generate_cmake()
    cmake_path = os.path.join("ui_component", "CMakeLists.txt")
    write_file(cmake_path, cmake_text)

    print("\n==========================================")
    print(" PIPELINE COMPLETED SUCCESSFULLY")
    print("==========================================\n")


if __name__ == "__main__":
    main()