
import sys
import os
import re
import xml.etree.ElementTree as ET
from figma_parser import parse_screen
from generator import generate_screen
from utils.utils import write_file

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 parser.py diagram.xml")
        sys.exit(1)

    xml_path = sys.argv[1]
    if not os.path.isfile(xml_path):
        print("File not found:", xml_path)
        sys.exit(1)

    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
    except Exception as e:
        print("Failed to parse XML:", e)
        sys.exit(1)

    # locate page -> children -> Frame nodes
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
        write_file(h_fname, h_text)
        write_file(c_fname, c_text)
        generated.append((c_fname, h_fname))

    print("\nGenerated files:")
    for c,h in generated:
        print(f" - {c}")
        print(f" - {h}")

if __name__ == "__main__":
    main()
