"""The classification fixture (tests/fixtures/naming_classify.xml).

One node per rule of design doc 10 (R1-R14), the mixed-keyword table, the
modifier cases and visibility, parsed by the real parser. The same file is
a compile case and a full-output golden case.
"""

import xml.etree.ElementTree as ET
from pathlib import Path

import pytest

from figma2lvgl.core.figma_parser import parse_screen
from figma2lvgl.core.widget_type import WidgetType as T

XML = Path(__file__).resolve().parent / "fixtures" / "naming_classify.xml"

# member id -> type, for every node that must be generated (top level)
EXPECTED = {
    "caption": T.LABEL,                 # R1
    "btn_ok": T.BUTTON,                 # R2
    "btn_next": T.BUTTON,               # R2, space separator (0.4.6: skipped)
    "btnback": T.BUTTON,                # R2, camelCase; name keeps 0.4.6 form
    "btn_press": T.BUTTON,              # N5: suffix kept
    "btn_save": T.BUTTON,               # explicit _lp suffix
    "slider_vol": T.SLIDER,             # R3
    "volume_slider": T.SLIDER,          # R3, hyphen
    "brightness_slider": T.SLIDER,      # N24 (0.4.6: skipped)
    "slider_rev": T.SLIDER,             # reversed range
    "slider_flat": T.SLIDER,            # equal range, warning
    "list_items": T.DYNAMIC,            # R4
    "battery_bar": T.BAR,               # R5, range stripped (B4)
    "batterybar": T.BAR,                # R5, camelCase
    "icon_wifi": T.IMAGE,               # R6
    "wifi_off": T.IMAGE,                # R8
    "panel_slot": T.PANEL,              # R10, empty placeholder
    "header": T.PANEL,                  # R11
    "btn_inner": T.BUTTON,              # promoted out of R12 "Content"
    "toolbar_bg": T.PANEL,              # R13 (0.4.6: BAR, N2)
    "btn_slider": T.BUTTON,
    "button_icon": T.BUTTON,
    "bar_volume_slider": T.SLIDER,
    "volume_slider_bar": T.BAR,
    "list_icon": T.DYNAMIC,
    "grid_bar": T.DYNAMIC,
    "bar_icon": T.BAR,
    "icon_bar": T.BAR,
    "panel_btn": T.PANEL,
    "slider_panel": T.SLIDER,
    "panel_2_3": T.PANEL,               # numbers stay in non-slider names
    "time_click": T.LABEL,              # N5: label keeps its suffix
    "box_with_hidden_child": T.PANEL,
}
SKIPPED = {"decoration", "card_inst", "content", "silicone_pad", "dot",
           "btn_hidden", "ghost_fill", "radius_only"}


@pytest.fixture(scope="module")
def screen():
    root = ET.parse(XML).getroot()
    return parse_screen(root.find("children")[0])


def test_types(screen):
    assert {n.id: n.widget_type for n in screen.children} == EXPECTED


def test_skipped(screen):
    ids = {n.id for n in screen.children}
    assert not (ids & SKIPPED)


def nodes(screen):
    return {n.id: n for n in screen.children}


def test_ranges(screen):
    n = nodes(screen)
    assert (n["brightness_slider"].slider_min, n["brightness_slider"].slider_max) == (0, 255)
    assert (n["slider_rev"].slider_min, n["slider_rev"].slider_max) == (100, 0)
    assert (n["slider_flat"].slider_min, n["slider_flat"].slider_max) == (5, 5)
    assert (n["battery_bar"].bar_min, n["battery_bar"].bar_max) == (-10, 90)


def test_events(screen):
    n = nodes(screen)
    assert n["btn_save"].event_modifiers == ["lp"]
    assert n["btn_press"].event_modifiers == []     # default click


def test_hidden_child_of_visible_container(screen):
    box = nodes(screen)["box_with_hidden_child"]
    assert [c.id for c in box.children] == ["btn_shown"]


def test_promoted_child_position(screen):
    # btn_inner at (10, 10) inside "Content" (no fill) at (4, 172).
    inner = nodes(screen)["btn_inner"]
    assert (inner.x, inner.y) == (14, 182)


def test_diagnostics_logged(caplog):
    root = ET.parse(XML).getroot()
    parse_screen(root.find("children")[0])
    text = caplog.text
    for expected in ["'btn_press': 'press' kept", "range 5..5 is empty",
                     "skipping component instance 'card_inst'",
                     "Ellipse layers aren't drawn as boxes",
                     "'volume_slider_bar' matches R5 (BAR); ignoring keyword(s) slider"]:
        assert expected in text, expected


def test_range_outside_int32_stops_the_cli(tmp_path):
    """A naming error ends the run with a message naming the layer, before
    anything is written (doc 10, 4.1)."""
    import subprocess
    import sys
    xml = tmp_path / "bad.xml"
    xml.write_text(
        '<page name="p"><children><Frame name="s" width="320" height="480">'
        '<children><Rectangle name="slider_x_0_9999999999" width="10" height="10"/>'
        '</children></Frame></children></page>', encoding="utf-8")
    out = tmp_path / "out"
    result = subprocess.run(
        [sys.executable, "-m", "figma2lvgl.main", "-x", str(xml), "-d", str(out), "-y"],
        cwd=Path(__file__).resolve().parents[1], capture_output=True, text=True)
    assert result.returncode == 1
    assert "slider_x_0_9999999999" in result.stdout + result.stderr
    assert "Traceback" not in result.stderr
    assert not (out / "ui_src").exists()
