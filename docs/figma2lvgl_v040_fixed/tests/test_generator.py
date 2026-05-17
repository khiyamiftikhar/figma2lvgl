# tests/test_generator.py  (v0.4.0)

from pathlib import Path
import xml.etree.ElementTree as ET
import pytest

from figma2lvgl.core.figma_parser import parse_screen
from figma2lvgl.core.generator import generate_screen

GOLDEN_DIR = Path(__file__).parent / "golden"

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


def _make():
    screen = parse_screen(ET.fromstring(TEST_XML))
    return generate_screen(screen)


class TestGolden:
    def test_c_matches_golden(self):
        _, _, _, c_text = _make()
        golden = (GOLDEN_DIR / "ui_testscreen.c").read_text()
        assert c_text == golden, "C output changed — run tests/regen_golden.py if intentional"

    def test_h_matches_golden(self):
        _, _, h_text, _ = _make()
        golden = (GOLDEN_DIR / "ui_testscreen.h").read_text()
        assert h_text == golden, "H output changed — run tests/regen_golden.py if intentional"


class TestStructure:
    def setup_method(self):
        self.c_fname, self.h_fname, self.h, self.c = _make()

    def test_filenames(self):
        assert self.c_fname == "ui_testscreen.c"
        assert self.h_fname == "ui_testscreen.h"

    def test_static_struct_present(self):
        assert "static struct {" in self.c
        assert "} s_testscreen" in self.c

    def test_nested_panel_in_struct(self):
        assert "} panel_top;" in self.c

    def test_nested_label_in_struct(self):
        assert "char        text[UI_MAX_STRING_LENGTH];" in self.c

    def test_button_const_label(self):
        assert "const char *label_text;" in self.c

    def test_bar_value_field(self):
        assert "int32_t value;" in self.c

    def test_bar_range_fields(self):
        assert "int32_t min;" in self.c
        assert "int32_t max;" in self.c

    def test_initializer_label_text(self):
        assert '.text = "16:30"' in self.c

    def test_initializer_button_label(self):
        assert '.label_text = "Ok"' in self.c

    def test_no_unresolved_vars(self):
        assert "${" not in self.c
        assert "${" not in self.h

    def test_callback_declared_not_defined(self):
        # Callbacks are declared in .h but NOT defined in .c — linker error if not implemented
        assert "__attribute__((weak))" not in self.c
        assert "ui_testscreen_on_btn_ok_clicked" in self.c   # registered in lv_obj_add_event_cb
        assert "ui_testscreen_on_btn_ok_clicked" in self.h   # declared in header

    def test_bar_anim_helper(self):
        assert "_bar_anim_exec_cb" in self.c

    def test_init_fn_present(self):
        assert "void ui_testscreen_init(void)" in self.c

    def test_load_fn_present(self):
        assert "void ui_testscreen_load(void)" in self.c

    def test_bar_init_uses_struct_range(self):
        assert "lv_bar_set_range(s_testscreen.battery_bar.lv_obj, s_testscreen.battery_bar.min, s_testscreen.battery_bar.max)" in self.c

    def test_panel_init_creates_obj(self):
        assert "s_testscreen.panel_top.lv_obj = lv_obj_create(s_testscreen.lv_screen);" in self.c

    def test_label_init_under_panel(self):
        assert "s_testscreen.panel_top.time.lv_obj = lv_label_create(s_testscreen.panel_top.lv_obj);" in self.c

    def test_button_init_creates_internal_label(self):
        assert "lv_label_create(s_testscreen.btn_ok.lv_obj)" in self.c

    def test_button_event_cb_registered(self):
        assert "LV_EVENT_CLICKED" in self.c

    def test_nested_setter_name(self):
        assert "ui_testscreen_panel_top_time_set_text" in self.c
        assert "ui_testscreen_panel_top_time_set_text" in self.h

    def test_button_setter_in_header(self):
        assert "ui_testscreen_btn_ok_set_label(const char *text)" in self.h

    def test_bar_setter_in_header(self):
        assert "ui_testscreen_battery_bar_set_value(int value, uint32_t duration_ms)" in self.h

    def test_callback_declared_in_header(self):
        assert "void ui_testscreen_on_btn_ok_clicked(lv_event_t *e);" in self.h

    def test_include_guard(self):
        assert "#ifndef UI_TESTSCREEN_H" in self.h
        assert "#define UI_TESTSCREEN_H" in self.h

    def test_lv_obj_set_size_on_button(self):
        assert "lv_obj_set_size(s_testscreen.btn_ok.lv_obj" in self.c

    def test_lv_label_set_text_in_init(self):
        # Label initial text applied during init
        assert "lv_label_set_text(s_testscreen.panel_top.time.lv_obj, s_testscreen.panel_top.time.text)" in self.c

    def test_ui_apply_style_called(self):
        assert "ui_apply_style(" in self.c
