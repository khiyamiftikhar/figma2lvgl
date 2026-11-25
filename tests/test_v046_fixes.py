"""Regression tests for the v0.4.6 fixes that need no compiler.

The compile checks (tests/compile) cover B1 and B2 in real C and C++ too.
"""

import re
import xml.etree.ElementTree as ET
from pathlib import Path

from figma2lvgl.core.figma_parser import parse_screen
from figma2lvgl.core.generator import generate_screen

ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "tests" / "fixtures"


def _generate(xml_name: str, frame_index: int = 0):
    root = ET.parse(FIXTURES / xml_name).getroot()
    frame = root.find("children").findall("Frame")[frame_index]
    screen = parse_screen(frame)
    return screen, generate_screen(screen)


def test_b1_button_callback_keeps_screen_name_starting_with_s():
    # v0.4.5 used lstrip("s_"), which turned "settings" into "ettings".
    _, (_, _, h_text, c_text) = _generate("probe_b1_settings.xml")
    assert "ui_settings_on_btn_ok_clicked" in h_text
    assert "lv_obj_add_event_cb(" in c_text
    assert "ui_settings_on_btn_ok_clicked" in c_text
    assert "ui_ettings_" not in c_text


def test_b2_empty_style_in_declaration_order():
    # ui_style_t declares box, text, effects; C++ needs that order.
    _, (_, _, _, c_text) = _generate("real/selection.xml", frame_index=0)
    empty = re.findall(r"\.style = \{[^\n]*\}", c_text)
    assert empty, "fixture should produce at least one empty style"
    for init in empty:
        assert init.strip() == ".style = { .box = {0}, .text = {0}, .effects = {0} }"


def test_b3_requires_python_310():
    # tomllib is 3.11+, and these tests run on 3.10 too.
    text = (ROOT / "pyproject.toml").read_text(encoding="utf-8")
    assert re.search(r'^requires-python\s*=\s*">=3\.10"\s*$', text, re.M)


def test_readme_and_help_list_the_same_flags():
    # Release criterion A6: every CLI flag is documented, nothing extra.
    from figma2lvgl.main import parse_args
    import argparse

    parser_flags = set()
    real_parse = argparse.ArgumentParser.parse_args

    def capture(self, args=None, namespace=None):
        for action in self._actions:
            parser_flags.update(o for o in action.option_strings if o.startswith("--"))
        return argparse.Namespace()

    argparse.ArgumentParser.parse_args = capture
    try:
        parse_args([])
    finally:
        argparse.ArgumentParser.parse_args = real_parse
    parser_flags.discard("--help")

    readme = (ROOT / "README.md").read_text(encoding="utf-8")
    table = readme[readme.index("### All Arguments"):readme.index("### Examples")]
    readme_flags = set(re.findall(r"`(?:-\w / )?(--[a-z][a-z-]*)", table))
    assert readme_flags == parser_flags
