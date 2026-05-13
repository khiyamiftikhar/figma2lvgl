# tests/test_generator.py
#
# Golden-file tests for the code generator.
# Strategy: build a minimal ParsedScreen programmatically (no XML dependency),
# generate C/H text, compare against checked-in golden files.
#
# If a golden test fails:
#   1. Inspect the diff to understand what changed
#   2. If the change is intentional, regenerate:
#        python tests/regen_golden.py
#   3. Commit the new golden files alongside the code change

from pathlib import Path
import pytest

from figma2lvgl.core.figma_parser import ParsedScreen, ParsedChild, ParsedStyle
from figma2lvgl.core.figma_parser import ParsedStyleText, ParsedStyleBox
from figma2lvgl.core.generator import generate_screen
from figma2lvgl.core.widget_type import WidgetType

GOLDEN_DIR = Path(__file__).parent / "golden"


# ── Test screen factory ───────────────────────────────────────────────────────

def _make_test_screen() -> ParsedScreen:
    """
    Minimal deterministic screen for golden-file testing.
    Matches the screen used when generating tests/golden/*.
    Two children: one LABEL, one BAR.
    """
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


# ── Golden file tests ─────────────────────────────────────────────────────────

class TestGeneratorGolden:

    def setup_method(self):
        screen = _make_test_screen()
        self.c_fname, self.h_fname, self.h_text, self.c_text = generate_screen(screen)

    def test_c_filename(self):
        assert self.c_fname == "ui_testscreen.c"

    def test_h_filename(self):
        assert self.h_fname == "ui_testscreen.h"

    def test_c_output_matches_golden(self):
        golden = (GOLDEN_DIR / "ui_testscreen.c").read_text()
        assert self.c_text == golden, (
            "Generator C output changed. If intentional, regenerate:\n"
            "  python tests/regen_golden.py\n"
            "Then commit the updated golden file."
        )

    def test_h_output_matches_golden(self):
        golden = (GOLDEN_DIR / "ui_testscreen.h").read_text()
        assert self.h_text == golden, (
            "Generator H output changed. If intentional, regenerate:\n"
            "  python tests/regen_golden.py\n"
            "Then commit the updated golden file."
        )


# ── Structural correctness tests (not golden-dependent) ───────────────────────

class TestGeneratorStructure:

    def setup_method(self):
        screen = _make_test_screen()
        self.c_fname, self.h_fname, self.h_text, self.c_text = generate_screen(screen)

    # FIX-1 Problem D: initial text applied in _init()
    def test_label_init_applies_text(self):
        assert "lv_label_set_text(c->lv_obj, c->data.label.text)" in self.c_text

    # FIX-1: text content baked into struct
    def test_label_text_in_struct(self):
        assert '.text = "Hello"' in self.c_text

    # FIX-4: image size would be tested here if screen had an image child
    # Covered by test_image_init_has_size below

    # FIX-5: substitute() — no ${...} literals in output
    def test_no_unresolved_template_variables(self):
        assert "${" not in self.c_text
        assert "${" not in self.h_text

    # FIX-6: enum value in generated C
    def test_ui_child_label_in_struct(self):
        assert ".type = UI_CHILD_LABEL" in self.c_text

    def test_ui_child_bar_in_struct(self):
        assert ".type = UI_CHILD_BAR" in self.c_text

    # FIX-7: naming from ChildSpec patterns
    def test_label_setter_name(self):
        assert "ui_testscreen_set_greeting" in self.c_text

    def test_bar_setter_name(self):
        assert "ui_testscreen_set_battery_bar" in self.c_text

    def test_bar_callback_name(self):
        assert "ui_testscreen_bar_job_cb" in self.c_text

    def test_label_setter_in_header(self):
        assert "ui_testscreen_set_greeting(const char *text)" in self.h_text

    def test_bar_setter_in_header(self):
        assert "ui_testscreen_set_battery_bar(int value, uint32_t duration_ms)" in self.h_text

    # FIX-9: per-screen bar range comment
    def test_bar_range_comment(self):
        assert "TODO: Bar range" in self.c_text
        assert "battery_bar" in self.c_text   # bar ID listed in comment

    # Include guard
    def test_include_guard(self):
        assert "#ifndef UI_TESTSCREEN_H" in self.h_text
        assert "#define UI_TESTSCREEN_H" in self.h_text

    # Style rendering
    def test_label_text_color_in_struct(self):
        assert ".has_color = true" in self.c_text
        assert ".color = 0xFFFFFF" in self.c_text

    def test_bar_bg_color_in_struct(self):
        assert ".has_bg = true" in self.c_text
        assert ".bg = 0x4CAF50" in self.c_text

    # Screen init and load functions
    def test_init_function_present(self):
        assert "void ui_testscreen_init(void)" in self.c_text

    def test_load_function_present(self):
        assert "void ui_testscreen_load(void)" in self.c_text


class TestImageInit:
    """FIX-4: IMAGE_INIT must set size, not just position."""

    def setup_method(self):
        from figma2lvgl.core.utils.figma_helpers import map_tag_to_child_type
        import xml.etree.ElementTree as ET

        # Build a screen with an image child
        screen = ParsedScreen("ImgScreen")
        screen.children.append(ParsedChild(
            type=WidgetType.IMAGE,
            id="icon_wifi",
            x=10, y=10, w=48, h=48,
        ))
        self.c_text = generate_screen(screen)[3]

    def test_image_init_has_set_pos(self):
        assert "lv_obj_set_pos(c->lv_obj, c->x, c->y)" in self.c_text

    def test_image_init_has_set_size(self):
        assert "lv_obj_set_size(c->lv_obj, c->w, c->h)" in self.c_text


class TestScreenWithNoBar:
    """FIX-9: bars_comment must be absent when screen has no bars."""

    def test_no_bar_comment_when_no_bars(self):
        screen = ParsedScreen("LabelOnly")
        screen.children.append(ParsedChild(
            type=WidgetType.LABEL,
            id="title",
            x=0, y=0, w=100, h=30,
            text_content="Hi",
        ))
        _, _, _, c_text = generate_screen(screen)
        assert "TODO: Bar range" not in c_text


class TestNoUnresolvedTemplateVars:
    """FIX-5: substitute() must raise KeyError on missing vars, not produce ${...} in output."""

    def test_no_dollar_brace_in_c_output(self):
        """If a ${variable} appears in generated C, it would cause a C compile error."""
        screen = _make_test_screen()
        _, _, h_text, c_text = generate_screen(screen)
        assert "${" not in c_text, (
            "Unresolved template variable in .c output — "
            "check that all ${...} vars are provided to Template.substitute()"
        )
        assert "${" not in h_text, (
            "Unresolved template variable in .h output"
        )
