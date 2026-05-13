#!/usr/bin/env python3
"""
Regenerate golden test files after an intentional generator change.

Usage:
    python tests/regen_golden.py

When to run:
    - After intentionally changing the output format of generator.py
    - After changing a template in core/templates/
    - After changing emit/layouts.py

After running, inspect the diff (git diff tests/golden/) to confirm
the changes are what you intended, then commit both the code and golden files.
"""

import sys
from pathlib import Path

# Ensure repo root is on path
sys.path.insert(0, str(Path(__file__).parent.parent))

from figma2lvgl.core.figma_parser import ParsedScreen, ParsedChild, ParsedStyle
from figma2lvgl.core.generator import generate_screen
from figma2lvgl.core.widget_type import WidgetType

GOLDEN_DIR = Path(__file__).parent / "golden"


def make_test_screen() -> ParsedScreen:
    screen = ParsedScreen("TestScreen")

    label_style = ParsedStyle()
    label_style.text.color = 0xFFFFFF
    label_style.text.size  = 16
    screen.children.append(ParsedChild(
        type=WidgetType.LABEL,
        id="greeting",
        x=10, y=20, w=100, h=30,
        style=label_style,
        text_content="Hello",
    ))

    bar_style = ParsedStyle()
    bar_style.box.bg_color = 0x4CAF50
    screen.children.append(ParsedChild(
        type=WidgetType.BAR,
        id="battery_bar",
        x=10, y=100, w=200, h=20,
        style=bar_style,
    ))

    return screen


def main():
    screen = make_test_screen()
    c_fname, h_fname, h_text, c_text = generate_screen(screen)

    GOLDEN_DIR.mkdir(exist_ok=True)

    c_path = GOLDEN_DIR / c_fname
    h_path = GOLDEN_DIR / h_fname

    c_path.write_text(c_text)
    h_path.write_text(h_text)

    print(f"Regenerated golden files:")
    print(f"  {c_path}")
    print(f"  {h_path}")
    print()
    print("Review the diff before committing:")
    print("  git diff tests/golden/")


if __name__ == "__main__":
    main()
