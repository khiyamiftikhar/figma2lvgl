import sys
import os
import re
import xml.etree.ElementTree as ET
from core.figma_parser import parse_screen
from core.generator import generate_screen
from core.utils.utils import write_file
from core.cmake_generator import generate_cmake



OUTPUT_DIR = os.path.join("ui_component", "generated")

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 parser.py diagram.xml")
        sys.exit(1)

    xml_path = sys.argv[1]
    if not os.path.isfile(xml_path):
        print("File not found:", xml_path)
        sys.exit(1)

    # 🔥 Ensure output directory exists
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

    generated = []

    for frame in frames:
        screen = parse_screen(frame)
        c_fname, h_fname, h_text, c_text = generate_screen(screen)

        print(f"Generating {c_fname} and {h_fname} ...")

        # 🔥 Build full paths
        c_path = os.path.join(OUTPUT_DIR, c_fname)
        h_path = os.path.join(OUTPUT_DIR, h_fname)

        write_file(h_path, h_text)
        write_file(c_path, c_text)

        generated.append((c_path, h_path))

    print("\nGenerated files:")
    for c, h in generated:
        print(f" - {c}")
        print(f" - {h}")
        
    cmake_text = generate_cmake()
    cmake_path = os.path.join("", "ui_component", "CMakeLists.txt")
    write_file(cmake_path, cmake_text)


if __name__ == "__main__":
    main()
