"""Positions, visibility and clipping from the coordinate fixtures.

Design doc 10, section 3.5: a node's XML x/y is relative to its nearest
frame-like ancestor (Frame, Component, Instance); Groups are transparent. A
generated widget's position is its screen position minus its generated
parent's. These tests parse the two Figma exports and check every generated
widget's screen position against Figma's own report. The same fixtures run
under real LVGL in tests/compile/test_positions.py.
"""

import pytest

from figma2lvgl.core.figma_parser import parse_screen
from figma2lvgl.core.widget_type import WidgetType
from tests.coords_cases import FIXTURES, figma_positions, screen_frame


def screen_positions(screen):
    """Widget id -> (screen x, screen y, node), summing parent positions."""
    out = {}

    def walk(nodes, ox, oy):
        for n in nodes:
            out[n.id] = (ox + n.x, oy + n.y, n)
            walk(n.children, ox + n.x, oy + n.y)

    walk(screen.children, 0, 0)
    return out


@pytest.mark.parametrize("fixture", list(FIXTURES))
def test_positions_match_figma(fixture):
    positions = screen_positions(parse_screen(screen_frame(fixture)))
    figma = figma_positions(fixture)
    _xml, _rep, _name, generated, _not = FIXTURES[fixture]
    for name in sorted(generated):
        x, y, _ = positions[name]
        assert (x, y) == figma[name], name


@pytest.mark.parametrize("fixture", list(FIXTURES))
def test_generated_set(fixture):
    positions = screen_positions(parse_screen(screen_frame(fixture)))
    _xml, _rep, _name, generated, not_generated = FIXTURES[fixture]
    buttons = {k for k in positions if k.startswith("btn_")}
    assert buttons == generated
    assert not (buttons & not_generated)


def test_hidden_frame_and_its_children_skipped():  # N18
    positions = screen_positions(parse_screen(screen_frame("coords")))
    assert "hidden_box" not in positions and "btn_e" not in positions


def test_containers_without_fill_are_dropped():  # R12, N16, N19
    positions = screen_positions(parse_screen(screen_frame("coords")))
    panels = [k for k, (_x, _y, n) in positions.items() if n.widget_type is WidgetType.PANEL]
    assert panels == []


def test_group_panel_children_relative_to_group():  # N19
    positions = screen_positions(parse_screen(screen_frame("coords2")))
    grp = positions["panel_grp"][2]
    assert grp.widget_type is WidgetType.PANEL and (grp.x, grp.y) == (30, 250)
    kids = {c.id: (c.x, c.y) for c in grp.children}
    assert kids == {"btn_pg1": (0, 0), "btn_pg2": (60, 30)}


@pytest.mark.parametrize("name, clips", [
    ("card", True),        # filled frame, clipsContent="true"
    ("auto_box", True),    # auto-layout frame, clipsContent="true"
    ("panel_grp", False),  # groups never clip
])
def test_clipping(name, clips):  # N23
    positions = screen_positions(parse_screen(screen_frame("coords2")))
    assert positions[name][2].clips is clips


def test_frame_without_clips_content_does_not_clip():  # N23
    import xml.etree.ElementTree as ET
    frame = ET.fromstring(
        '<Frame name="s" type="FRAME"><children>'
        '<Frame name="box" type="FRAME" x="10" y="10" width="50" height="50">'
        '<fills><fill color="#ffffff"/></fills><children>'
        '<Rectangle name="btn_x" x="40" y="40" width="30" height="20" type="RECTANGLE"/>'
        '</children></Frame></children></Frame>')
    box = parse_screen(frame).children[0]
    assert box.widget_type is WidgetType.PANEL and box.clips is False


def test_rotated_layer_at_figma_origin(caplog):
    positions = screen_positions(parse_screen(screen_frame("coords2")))
    assert positions["btn_rot"][:2] == (60, 380)
    assert "rotated 30" in caplog.text


def test_layer_opacity_zero_kept():
    positions = screen_positions(parse_screen(screen_frame("coords")))
    ghost = positions["btn_ghost"][2]
    assert ghost.widget_type is WidgetType.BUTTON
    assert ghost.style.effects.opacity == 0
