import sys
import os
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


# -------------------------------------------------
# Clean previously generated UI files
# -------------------------------------------------
def clean_generated_files(output_dir):
    if not os.path.exists(output_dir):
        return

    for filename in os.listdir(output_dir):
        if filename.startswith("ui_") and (
            filename.endswith(".c") or filename.endswith(".h")
        ):
            os.remove(os.path.join(output_dir, filename))


# -------------------------------------------------
# Validate required assets exist
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
        print("\nERROR: Missing asset files:")
        for m in missing:
            print("   ", m)
        raise RuntimeError("Asset validation failed.")


# -------------------------------------------------
# Run external script safely
# -------------------------------------------------
def run_script(script_path, description):
    print(f"\nRunning {description}...")

    result = subprocess.run(
        ["python", script_path],
        capture_output=True,
        text=True
    )

    if result.returncode != 0:
        print(result.stdout)
        print(result.stderr)
        raise RuntimeError(f"{description} failed.")

    print(f"{description} completed.")


# -------------------------------------------------
# Main
# -------------------------------------------------
def main():
    if len(sys.argv) < 2:
        print("Usage: python3 parser.py diagram.xml")
        sys.exit(1)

    xml_path = sys.argv[1]
    if not os.path.isfile(xml_path):
        print("File not found:", xml_path)
        sys.exit(1)

    os.makedirs(OUTPUT_DIR, exist_ok=True)

    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
    except Exception as e:
        print("Failed to parse XML:", e)
        sys.exit(1)

    page_children = root.find("children")
    if page_children is None:
        print("No <children> found at top level in XML.")
        sys.exit(1)

    frames = [n for n in page_children.findall("Frame")]
    if not frames:
        print("No <Frame> nodes found in XML.")
        sys.exit(1)

    # -------------------------------------------------
    # Parse screens
    # -------------------------------------------------
    screens = [parse_screen(frame) for frame in frames]

    # -------------------------------------------------
    # Validate required assets
    # -------------------------------------------------
    validate_assets(screens)

    # -------------------------------------------------
    # Run image conversion
    # -------------------------------------------------
    run_script(IMAGE_CONVERTER_SCRIPT, "image conversion script")

    # -------------------------------------------------
    # Clean old generated UI files
    # -------------------------------------------------
    clean_generated_files(OUTPUT_DIR)

    # -------------------------------------------------
    # Generate UI source/header files
    # -------------------------------------------------
    generated = []

    for screen in screens:
        c_fname, h_fname, h_text, c_text = generate_screen(screen)

        print(f"Generating {c_fname} and {h_fname} ...")

        c_path = os.path.join(OUTPUT_DIR, c_fname)
        h_path = os.path.join(OUTPUT_DIR, h_fname)

        write_file(h_path, h_text)
        write_file(c_path, c_text)

        generated.append((c_path, h_path))

    print("\nGenerated files:")
    for c, h in generated:
        print(f" - {c}")
        print(f" - {h}")

    # -------------------------------------------------
    # Generate CMake file
    # -------------------------------------------------
    cmake_text = generate_cmake()
    cmake_path = os.path.join("ui_component", "CMakeLists.txt")
    write_file(cmake_path, cmake_text)

    print("\nPipeline completed successfully.")


if __name__ == "__main__":
    main()