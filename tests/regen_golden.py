#!/usr/bin/env python3
"""
Regenerate golden test files after an intentional generator change.

Usage:
    python tests/regen_golden.py

When to run:
    - After intentionally changing the output format of generator.py
    - After changing any emitter (node_emitter, init_emitter, setter_emitter)

After running, inspect the diff (git diff tests/golden/) to confirm
the changes are what you intended, then commit both the code and golden files.
"""

import sys
import xml.etree.ElementTree as ET
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent.parent))

from figma2lvgl.core.figma_parser import parse_screen
from figma2lvgl.core.generator import generate_screen

GOLDEN_DIR = Path(__file__).parent / "golden"

# Must stay in sync with TEST_XML in test_generator.py
TEST_XML = """
<Frame name="TestScreen" width="320" height="480">
  <children>
    <Frame name="panel_top" x="0" y="0" width="320" height="80">
      <fills><fill blendMode="NORMAL" color="#222222" /></fills>
      <children>
        <Text name="time" x="10" y="20" width="100" height="30"
              fontSize="14" characters="16:30">
          <fills><fill blendMode="NORMAL" color="#ffffff" /></fills>
        </Text>
      </children>
    </Frame>
    <Frame name="btn_ok" x="100" y="380" width="120" height="44"
           cornerRadius="8" type="FRAME">
      <fills><fill blendMode="NORMAL" color="#2196F3" /></fills>
      <children>
        <Text name="label" x="0" y="0" width="120" height="44"
              characters="Ok" type="TEXT">
          <fills><fill blendMode="NORMAL" color="#ffffff" /></fills>
        </Text>
      </children>
    </Frame>
    <Rectangle name="battery_bar" x="10" y="300" width="200" height="20">
      <fills><fill blendMode="NORMAL" color="#4caf50" /></fills>
    </Rectangle>
  </children>
</Frame>
"""


def main():
    screen = parse_screen(ET.fromstring(TEST_XML))
    c_fname, h_fname, h_text, c_text = generate_screen(screen)

    GOLDEN_DIR.mkdir(exist_ok=True)
    (GOLDEN_DIR / c_fname).write_text(c_text)
    (GOLDEN_DIR / h_fname).write_text(h_text)

    print("Regenerated golden files:")
    print(f"  {GOLDEN_DIR / c_fname}")
    print(f"  {GOLDEN_DIR / h_fname}")
    print()
    print("Review the diff before committing:")
    print("  git diff tests/golden/")


if __name__ == "__main__":
    main()
