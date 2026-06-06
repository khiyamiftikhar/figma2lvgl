# tests/test_parser.py  (v0.4.0)

import xml.etree.ElementTree as ET
import logging
import pytest

from figma2lvgl.core.figma_parser import parse_screen, ParsedNode
from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.utils.utils import sanitize_c_string, normalize_id

# ── Fixtures ──────────────────────────────────────────────────────────────────

FLAT_XML = """
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

NESTED_XML = """
<Frame name="HomeScreen" width="320" height="480">
  <children>
    <Frame name="panel_top" x="0" y="0" width="320" height="80">
      <fills><fill blendMode="NORMAL" color="#222222" /></fills>
      <children>
        <Text name="time" x="10" y="20" width="100" height="30"
              fontSize="14" characters="16:30">
          <fills><fill blendMode="NORMAL" color="#ffffff" /></fills>
        </Text>
        <icon_wifi name="icon_wifi" x="270" y="16" width="32" height="32"
                   type="INSTANCE">
          <fills><fill visible="false" blendMode="NORMAL" color="#ffffff" /></fills>
        </icon_wifi>
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
    <Rectangle name="brightness_slider" x="40" y="200" width="240" height="20"
               cornerRadius="10">
      <fills><fill blendMode="NORMAL" color="#78909C" /></fills>
    </Rectangle>
    <Frame name="list_devices" x="0" y="300" width="320" height="150">
      <children>
        <Text name="item" x="0" y="0" width="320" height="40"
              characters="Device 1" type="TEXT"/>
      </children>
    </Frame>
  </children>
</Frame>
"""

REALISTIC_XML = """
<Frame id="6:3" name="ili9486_home" maskType="ALPHA"
       x="0" y="0" width="320" height="480" type="FRAME">
  <children>
    <Text id="6:4" name="Time" maskType="ALPHA"
          x="94" y="79" width="133" height="34" fontSize="12"
          fontWeight="400" characters="Time is 15:00" type="TEXT"
          textAlignVertical="TOP">
      <fills><fill blendMode="NORMAL" color="#000000" /></fills>
    </Text>
    <Rectangle id="21:4" name="bar" x="33" y="311" width="241" height="35">
      <fills><fill blendMode="NORMAL" color="#e56060" /></fills>
    </Rectangle>
    <icon_wifi id="12:9" name="icon_wifi" x="129" y="143" width="48" height="48"
               type="INSTANCE">
      <fills><fill visible="false" blendMode="NORMAL" color="#ffffff" /></fills>
    </icon_wifi>
  </children>
</Frame>
"""


def _p(xml): return parse_screen(ET.fromstring(xml))


# ── Flat screen tests ─────────────────────────────────────────────────────────

class TestFlatScreen:
    def test_child_count(self):
        assert len(_p(FLAT_XML).children) == 2

    def test_label_type(self):
        assert _p(FLAT_XML).children[0].widget_type == WidgetType.LABEL

    def test_bar_type(self):
        assert _p(FLAT_XML).children[1].widget_type == WidgetType.BAR

    def test_label_text(self):
        assert _p(FLAT_XML).children[0].text_content == "Hello"

    def test_label_geometry(self):
        c = _p(FLAT_XML).children[0]
        assert (c.x, c.y, c.w, c.h) == (10, 20, 100, 30)

    def test_bar_has_no_text(self):
        assert _p(FLAT_XML).children[1].text_content == ""


# ── Nested screen tests ───────────────────────────────────────────────────────

class TestNestedScreen:
    def _screen(self):
        return _p(NESTED_XML)

    def test_direct_child_count(self):
        # panel_top, btn_ok, brightness_slider, list_devices
        assert len(self._screen().children) == 4

    def test_panel_type(self):
        panel = self._screen().children[0]
        assert panel.widget_type == WidgetType.PANEL
        assert panel.id == "panel_top"

    def test_panel_has_children(self):
        panel = self._screen().children[0]
        assert len(panel.children) == 2

    def test_panel_child_time_is_label(self):
        time = self._screen().children[0].children[0]
        assert time.widget_type == WidgetType.LABEL
        assert time.text_content == "16:30"

    def test_panel_child_icon_is_image(self):
        icon = self._screen().children[0].children[1]
        assert icon.widget_type == WidgetType.IMAGE

    def test_button_type(self):
        btn = self._screen().children[1]
        assert btn.widget_type == WidgetType.BUTTON
        assert btn.id == "btn_ok"

    def test_button_label_from_text_child(self):
        btn = self._screen().children[1]
        assert btn.text_content == "Ok"

    def test_button_has_no_parsed_children(self):
        # Button children are LVGL internal detail, not separate ParsedNodes
        btn = self._screen().children[1]
        assert len(btn.children) == 0

    def test_slider_type(self):
        slider = self._screen().children[2]
        assert slider.widget_type == WidgetType.SLIDER

    def test_slider_default_range(self):
        slider = self._screen().children[2]
        assert slider.slider_min == 0
        assert slider.slider_max == 100

    def test_bar_range_from_name(self):
        # battery_bar has no range in name → defaults 0/100
        # (panel_top[0] is a label, children[1] is btn_ok, [2] is slider, [3] is list)
        # Use a dedicated XML for this
        xml = """
<Frame name="S" width="320" height="480">
  <children>
    <Rectangle name="temp_bar_n20_50" x="0" y="0" width="200" height="20">
      <fills><fill blendMode="NORMAL" color="#ff0000" /></fills>
    </Rectangle>
  </children>
</Frame>"""
        import xml.etree.ElementTree as ET2
        screen = parse_screen(ET2.fromstring(xml))
        bar = screen.children[0]
        assert bar.widget_type == WidgetType.BAR
        assert bar.bar_min == -20
        assert bar.bar_max == 50

    def test_dynamic_container(self):
        lst = self._screen().children[3]
        assert lst.widget_type == WidgetType.DYNAMIC
        assert lst.id == "list_devices"

    def test_dynamic_has_no_children(self):
        lst = self._screen().children[3]
        assert len(lst.children) == 0

    def test_label_fill_routes_to_text_color(self):
        # FIX-6 regression guard: Text fill must route to text.color, not bg_color
        time_node = self._screen().children[0].children[0]
        assert time_node.style.text.color == 0xFFFFFF
        assert time_node.style.box.bg_color is None


# ── Realistic FigML tests ─────────────────────────────────────────────────────

class TestRealisticXML:
    def _screen(self):
        return _p(REALISTIC_XML)

    def test_child_count(self):
        # Time (LABEL), bar (BAR), icon_wifi (IMAGE)
        assert len(self._screen().children) == 3

    def test_label_text(self):
        assert self._screen().children[0].text_content == "Time is 15:00"

    def test_bar_type(self):
        assert self._screen().children[1].widget_type == WidgetType.BAR

    def test_image_type(self):
        assert self._screen().children[2].widget_type == WidgetType.IMAGE

    def test_label_fill_routes_correctly(self):
        lbl = self._screen().children[0]
        assert lbl.style.text.color == 0x000000
        assert lbl.style.box.bg_color is None


# ── Widget type detection tests ───────────────────────────────────────────────

class TestDetection:
    def _node(self, xml_str):
        return ET.fromstring(xml_str)

    def test_text_is_label(self):
        from figma2lvgl.core.utils.figma_helpers import detect_widget_type
        node = self._node('<Text name="foo" />')
        assert detect_widget_type(node) == WidgetType.LABEL

    def test_btn_prefix_is_button(self):
        from figma2lvgl.core.utils.figma_helpers import detect_widget_type
        node = self._node('<Frame name="btn_ok" />')
        assert detect_widget_type(node) == WidgetType.BUTTON

    def test_slider_suffix(self):
        from figma2lvgl.core.utils.figma_helpers import detect_widget_type
        node = self._node('<Rectangle name="brightness_slider" />')
        assert detect_widget_type(node) == WidgetType.SLIDER

    def test_list_prefix_is_dynamic(self):
        from figma2lvgl.core.utils.figma_helpers import detect_widget_type
        node = self._node('<Frame name="list_devices"><children><Text name="x"/></children></Frame>')
        assert detect_widget_type(node) == WidgetType.DYNAMIC

    def test_bar_keyword(self):
        from figma2lvgl.core.utils.figma_helpers import detect_widget_type
        node = self._node('<Rectangle name="battery_bar" />')
        assert detect_widget_type(node) == WidgetType.BAR


# ── Sanitize tests ────────────────────────────────────────────────────────────

class TestSanitize:
    def test_newline_escaped(self):
        assert "\\n" in sanitize_c_string("Time\n15:00", 30)
        assert "\n" not in sanitize_c_string("Time\n15:00", 30)

    def test_u2028_escaped(self):
        assert "\\n" in sanitize_c_string("Time\u202815:00", 30)

    def test_truncation(self):
        assert len(sanitize_c_string("A" * 40, 30)) == 29


# ── normalize_id tests ────────────────────────────────────────────────────────

class TestNormalizeId:
    def test_camel_case(self):   assert normalize_id("BatteryBar")   == "battery_bar"
    def test_spaces(self):       assert normalize_id("Progress Bar") == "progress_bar"
    def test_hyphens(self):      assert normalize_id("icon-wifi")    == "icon_wifi"
    def test_already_snake(self):assert normalize_id("battery_bar")  == "battery_bar"

# ── BFS traversal test ────────────────────────────────────────────────────────

class TestBFS:
    def test_bfs_order(self):
        screen = _p(NESTED_XML)
        bfs = screen.all_nodes_bfs()
        ids = [n.id for n, _, _ in bfs]
        # panel_top must appear before its children (time, icon_wifi)
        assert ids.index("panel_top") < ids.index("time")
        assert ids.index("panel_top") < ids.index("icon_wifi")
