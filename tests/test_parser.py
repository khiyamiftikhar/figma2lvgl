# tests/test_parser.py
#
# Parser tests covering:
#   - Synthetic XML (minimal, fast, readable)
#   - Realistic FigML XML (derived from ili9486.xml — includes all noise
#     attributes FigML actually exports: id, maskType, type, blendMode, etc.)
#
# Both fixtures are needed:
#   - Synthetic tests isolate logic clearly
#   - Realistic tests catch regressions where the parser works on clean XML
#     but breaks on real FigML noise attributes

import xml.etree.ElementTree as ET
import logging
import pytest

from figma2lvgl.core.figma_parser import parse_screen
from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.utils.utils import sanitize_c_string, normalize_id


# ── Fixtures ─────────────────────────────────────────────────────────────────

SIMPLE_XML = """
<Frame name="TestScreen" width="320" height="480">
  <children>
    <Text name="greeting" x="10" y="20" width="100" height="30"
          fontSize="16" characters="Hello">
      <fills><fill blendMode="NORMAL" color="#ffffff" /></fills>
    </Text>
    <Rectangle name="battery_bar" x="10" y="100" width="200" height="20">
      <fills><fill blendMode="NORMAL" color="#4caf50" /></fills>
    </Rectangle>
  </children>
</Frame>
"""

# Derived from ili9486.xml — includes real FigML noise attributes.
# Contains a Wifi_off INSTANCE node with no type keyword to verify skip+warn.
REALISTIC_XML = """
<Frame id="6:3" name="ili9486_home" maskType="ALPHA"
       x="0" y="0" width="320" height="480"
       clipsContent="true" type="FRAME">
  <children>
    <Text id="6:4" name="Time" maskType="ALPHA" strokeAlign="OUTSIDE"
          x="94" y="79" width="133" height="34" fontSize="12"
          fontWeight="400" characters="Time is 15:00" type="TEXT"
          textAlignVertical="TOP">
      <fills>
        <fill blendMode="NORMAL" color="#000000" />
      </fills>
      <fontName family="Inter" style="Regular" />
    </Text>
    <Rectangle id="21:4" name="bar" maskType="ALPHA"
               x="33" y="311" width="241" height="35" type="RECTANGLE">
      <fills>
        <fill blendMode="NORMAL" color="#e56060" />
      </fills>
    </Rectangle>
    <icon_wifi id="12:9" name="icon_wifi" maskType="ALPHA"
               x="129" y="143" width="48" height="48"
               clipsContent="true" type="INSTANCE">
      <fills>
        <fill visible="false" blendMode="NORMAL" color="#ffffff" />
      </fills>
    </icon_wifi>
    <Wifi_off id="36:5" name="Wifi_off" maskType="ALPHA"
              x="129" y="143" width="48" height="48"
              clipsContent="true" type="INSTANCE">
      <fills>
        <fill visible="false" blendMode="NORMAL" color="#ffffff" />
      </fills>
    </Wifi_off>
  </children>
</Frame>
"""

NEWLINE_XML = """
<Frame name="S">
  <children>
    <Text name="t" x="0" y="0" width="100" height="30"
          characters="Time is&#10;15:00">
      <fills><fill blendMode="NORMAL" color="#000000" /></fills>
    </Text>
  </children>
</Frame>
"""

U2028_XML = """
<Frame name="S">
  <children>
    <Text name="t" x="0" y="0" width="100" height="30"
          characters="Time is&#x2028;15:00">
      <fills><fill blendMode="NORMAL" color="#000000" /></fills>
    </Text>
  </children>
</Frame>
"""


def _p(xml):
    return parse_screen(ET.fromstring(xml))


# ── Synthetic XML tests ───────────────────────────────────────────────────────

class TestSimpleXML:

    def test_child_count(self):
        assert len(_p(SIMPLE_XML).children) == 2

    def test_screen_name(self):
        assert _p(SIMPLE_XML).name == "TestScreen"

    def test_screen_snake(self):
        # to_snake_case strips non-alnum and lowercases.
        # It does NOT split camelCase boundaries — that's normalize_id()'s job
        # (which is used for child IDs). Screen names from real FigML exports
        # are already snake_case (e.g. "ili9486_home"), so this is fine.
        assert _p(SIMPLE_XML).snake == "testscreen"

    def test_label_type(self):
        assert _p(SIMPLE_XML).children[0].type == WidgetType.LABEL

    def test_bar_type(self):
        assert _p(SIMPLE_XML).children[1].type == WidgetType.BAR

    def test_label_text_content(self):
        assert _p(SIMPLE_XML).children[0].text_content == "Hello"

    def test_bar_text_content_empty(self):
        # Non-label nodes should have empty text_content
        assert _p(SIMPLE_XML).children[1].text_content == ""

    def test_label_geometry(self):
        c = _p(SIMPLE_XML).children[0]
        assert (c.x, c.y, c.w, c.h) == (10, 20, 100, 30)

    def test_bar_geometry(self):
        c = _p(SIMPLE_XML).children[1]
        assert (c.x, c.y, c.w, c.h) == (10, 100, 200, 20)

    def test_label_id_normalized(self):
        assert _p(SIMPLE_XML).children[0].id == "greeting"

    def test_bar_id_normalized(self):
        assert _p(SIMPLE_XML).children[1].id == "battery_bar"

    def test_duplicate_id_raises(self):
        xml = """<Frame name="S"><children>
            <Text name="lbl" x="0" y="0" width="10" height="10"
                  characters="A"/>
            <Text name="lbl" x="0" y="0" width="10" height="10"
                  characters="B"/>
        </children></Frame>"""
        with pytest.raises(ValueError, match="Duplicate child id"):
            _p(xml)

    def test_empty_children(self):
        xml = '<Frame name="Empty"><children></children></Frame>'
        assert len(_p(xml).children) == 0

    def test_no_children_element(self):
        xml = '<Frame name="Bare"></Frame>'
        assert len(_p(xml).children) == 0


# ── Realistic FigML XML tests ─────────────────────────────────────────────────

class TestRealisticXML:

    def test_child_count_skips_wifi_off(self):
        # Wifi_off has no type keyword → skipped → 3 children, not 4
        assert len(_p(REALISTIC_XML).children) == 3

    def test_label_type(self):
        assert _p(REALISTIC_XML).children[0].type == WidgetType.LABEL

    def test_bar_type(self):
        assert _p(REALISTIC_XML).children[1].type == WidgetType.BAR

    def test_image_type(self):
        assert _p(REALISTIC_XML).children[2].type == WidgetType.IMAGE

    def test_label_text_content(self):
        assert _p(REALISTIC_XML).children[0].text_content == "Time is 15:00"

    def test_label_id(self):
        assert _p(REALISTIC_XML).children[0].id == "time"

    def test_bar_id(self):
        assert _p(REALISTIC_XML).children[1].id == "bar"

    def test_image_id(self):
        assert _p(REALISTIC_XML).children[2].id == "icon_wifi"

    def test_unknown_node_skipped_with_warning(self, caplog):
        """Wifi_off has no type keyword — must skip with a warning naming the screen."""
        with caplog.at_level(logging.WARNING):
            screen = _p(REALISTIC_XML)
        assert len(screen.children) == 3
        assert "Wifi_off" in caplog.text
        assert "ili9486_home" in caplog.text   # frame name in warning message

    def test_label_fill_routes_to_text_color(self):
        """
        FIX-6 regression guard: black fill (#000000) on a Text node must
        route to ParsedStyleText.color, NOT ParsedStyleBox.bg_color.
        If the WidgetType enum comparison in parse_style() is wrong,
        this fails — every label renders with wrong color.
        """
        c = _p(REALISTIC_XML).children[0]
        assert c.style.text.color == 0x000000
        assert c.style.box.bg_color is None

    def test_image_fill_visible_false_ignored(self):
        """icon_wifi has visible=false fill — bg_color should be None."""
        c = _p(REALISTIC_XML).children[2]
        assert c.style.box.bg_color is None

    def test_bar_fill_color(self):
        c = _p(REALISTIC_XML).children[1]
        assert c.style.box.bg_color == 0xe56060


# ── Text content / sanitization tests ────────────────────────────────────────

class TestTextContent:

    def test_newline_escaped(self):
        """FIX-1: literal \\n in characters attr must be escaped to \\\\n in C."""
        c = _p(NEWLINE_XML).children[0]
        assert "\\n" in c.text_content
        assert "\n" not in c.text_content

    def test_unicode_line_separator_escaped(self):
        """FIX-1: U+2028 (FigML line separator) must be escaped to \\\\n."""
        c = _p(U2028_XML).children[0]
        assert "\\n" in c.text_content
        assert "\u2028" not in c.text_content

    def test_sanitize_backslash(self):
        assert sanitize_c_string("path\\to\\file", 30) == "path\\\\to\\\\file"

    def test_sanitize_quote(self):
        assert sanitize_c_string('say "hi"', 30) == 'say \\"hi\\"'

    def test_sanitize_tab(self):
        assert sanitize_c_string("col1\tcol2", 30) == "col1\\tcol2"

    def test_sanitize_truncation(self):
        result = sanitize_c_string("A" * 35, 30)
        assert len(result) == 29   # maxlen - 1


# ── normalize_id tests ────────────────────────────────────────────────────────

class TestNormalizeId:

    def test_camel_case(self):
        assert normalize_id("BatteryBar") == "battery_bar"

    def test_pascal_with_acronym(self):
        assert normalize_id("HTTPStatus") == "http_status"

    def test_spaces(self):
        assert normalize_id("Progress Bar") == "progress_bar"

    def test_hyphens(self):
        assert normalize_id("icon-wifi") == "icon_wifi"

    def test_parens(self):
        assert normalize_id("Time (Label)") == "time_label"

    def test_already_snake(self):
        assert normalize_id("battery_bar") == "battery_bar"
