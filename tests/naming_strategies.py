"""Hypothesis strategies: random Figma designs for the naming property tests.

Random layer trees (depth <= 5, <= 30 nodes per screen, 1-3 screens) whose
names are drawn from a pool meant to hit every rule of design doc 10:
type keywords, event suffixes, range numbers (including one outside int32),
digits, non-ASCII, C and C++ keywords, generated field names, reserved file
stems, Figma's automatic names, and exact duplicates. A second strategy
builds flattened-path clashes on purpose, which random names rarely hit.
"""

import xml.etree.ElementTree as ET

from hypothesis import strategies as st

WORDS = [
    "title", "ok", "home", "a", "b", "x", "row", "time", "value", "2nd", "row2",
    "default", "class", "int", "new", "and", "lv", "obj", "style", "text",
    "src", "min", "max", "label", "screen", "ui", "defs", "config", "assets",
    "Frame", "Group", "★", "é", "الوقت", "HTTP", "camelCase",
]
TYPE_WORDS = ["btn", "button", "slider", "list", "grid", "bar", "icon", "image", "panel"]
EVENT_WORDS = ["click", "press", "release", "lp", "lpr"]
NUMBERS = ["0", "1", "12", "100", "255", "n10", "9999999999"]
SEPARATORS = ["_", " ", "-", "", "__", "  "]
# Exact names, drawn often so siblings and screens collide.
FIXED_NAMES = [
    "title", "title", "title_2", "btn_ok", "btn_ok_press", "style", "text",
    "lv_obj", "default", "icon_wifi", "panel_a", "panel_a_b", "", "★",
    "slider_vol_0_255", "battery_bar_n10_90", "list_items", "Frame 12", "Group",
    "slider_big_0_9999999999",
]
SCREEN_NAMES = [
    "home", "Home", "settings", "style", "style_2", "defs", "2nd page",
    "default", "HomeScreen", "a", "★", "",
]


def _case(word, how):
    return {"lower": word.lower(), "title": word.title(), "upper": word.upper(),
            "keep": word}[how]


@st.composite
def layer_names(draw):
    if draw(st.integers(0, 3)) == 0:
        return draw(st.sampled_from(FIXED_NAMES))
    parts = draw(st.lists(
        st.sampled_from(WORDS + TYPE_WORDS + TYPE_WORDS + EVENT_WORDS + NUMBERS),
        min_size=1, max_size=4))
    sep = draw(st.sampled_from(SEPARATORS))
    how = draw(st.sampled_from(["lower", "lower", "title", "upper", "keep"]))
    return sep.join(_case(p, how) for p in parts)


def _fill(color="#336699", visible=True):
    fills = ET.Element("fills")
    f = ET.SubElement(fills, "fill", color=color)
    if not visible:
        f.set("visible", "false")
    return fills


@st.composite
def leaves(draw):
    kind = draw(st.sampled_from(["Text", "Rectangle", "Rectangle", "Rectangle", "Ellipse", "Vector"]))
    el = ET.Element(kind, name=draw(layer_names()), type=kind.upper(),
                    x=str(draw(st.integers(0, 280))), y=str(draw(st.integers(0, 440))),
                    width=str(draw(st.integers(1, 120))), height=str(draw(st.integers(1, 60))))
    if kind == "Text":
        el.set("characters", draw(st.sampled_from(["t", "12:00", 'q"uote', "back\\slash", "ü"])))
        el.set("fontSize", "12")
    if draw(st.booleans()):
        el.append(_fill(visible=draw(st.integers(0, 5)) > 0))
    if draw(st.integers(0, 9)) == 0:
        el.set("visible", "false")
    if draw(st.integers(0, 9)) == 0:
        el.set("rotation", "30")
    return el


def _container(kids, name, kind, filled, clips, hidden, x, y):
    el = ET.Element(kind, name=name, type=kind.upper(), x=str(x), y=str(y),
                    width="160", height="120")
    if clips and kind == "Frame":
        el.set("clipsContent", "true")
    if hidden:
        el.set("visible", "false")
    children = ET.SubElement(el, "children")
    for k in kids:
        children.append(k)
    if filled and kind == "Frame":
        el.append(_fill("#eeeeee"))
    return el


@st.composite
def containers(draw, children):
    kids = draw(st.lists(children, min_size=1, max_size=4))
    return _container(
        kids, draw(layer_names()), draw(st.sampled_from(["Frame", "Frame", "Group"])),
        draw(st.booleans()), draw(st.booleans()), draw(st.integers(0, 12)) == 0,
        draw(st.integers(0, 200)), draw(st.integers(0, 300)))


@st.composite
def flattened_clash(draw):
    """Panels 'p_q' > 'r' and 'p' > 'q_r': both setters join to p_q_r."""
    p, q, r = (draw(st.sampled_from(["a", "b", "x", "row", "top"])) for _ in range(3))
    text = lambda n: ET.Element("Text", name=n, type="TEXT", characters="t",
                                x="2", y="2", width="40", height="12")
    return [
        _container([text(r)], f"{p}_{q}", "Frame", True, True, False, 0, 0),
        _container([text(f"{q}_{r}")], p, "Frame", True, True, False, 0, 140),
    ]


@st.composite
def deep_chain(draw):
    """Filled frames nested 4 deep around a label: full setter paths (N4)."""
    leaf = ET.Element("Text", name=draw(layer_names()), type="TEXT", characters="t",
                      x="1", y="1", width="20", height="10")
    el = leaf
    for _ in range(4):
        el = _container([el], draw(layer_names()), "Frame", True, False, False, 2, 2)
    return [el]


@st.composite
def same_button_in_two_panels(draw):
    """The same button name under two panels: qualified callbacks (4.6)."""
    btn = draw(st.sampled_from(["btn_ok", "btn_go_press", "button_save"]))
    rect = lambda: ET.Element("Rectangle", name=btn, type="RECTANGLE",
                              x="2", y="2", width="40", height="20")
    outer = draw(st.booleans())      # one more level: needs two-level qualification
    a = _container([rect()], "panel_c", "Frame", True, False, False, 0, 0)
    b = _container([rect()], "panel_c" if outer else "panel_d", "Frame", True, False, False, 0, 0)
    if outer:
        a = _container([a], "x", "Frame", True, False, False, 0, 0)
        b = _container([b], "y", "Frame", True, False, False, 0, 200)
    return [a, b]


def trees():
    """One screen's top-level children (at most 30 nodes plus the shapes)."""
    node = st.recursive(leaves(), lambda inner: containers(inner), max_leaves=30)
    shaped = st.one_of(flattened_clash(), deep_chain(), same_button_in_two_panels())
    return st.one_of(
        st.lists(node, min_size=1, max_size=6),
        st.tuples(st.lists(node, max_size=3), shaped).map(lambda t: t[0] + t[1]),
    )


def _depth(el, d=0):
    kids = el.find("children")
    return d if kids is None or not len(kids) else max(_depth(k, d + 1) for k in kids)


@st.composite
def designs(draw):
    """A page element with 1-3 screen frames."""
    page = ET.Element("page", name="Page_1", type="PAGE")
    frames = ET.SubElement(page, "children")
    for _ in range(draw(st.integers(1, 3))):
        frame = ET.SubElement(frames, "Frame", name=draw(st.sampled_from(SCREEN_NAMES)),
                              type="FRAME", x="0", y="0", width="320", height="480")
        kids = ET.SubElement(frame, "children")
        for child in draw(trees().filter(lambda t: all(_depth(c) <= 4 for c in t))):
            kids.append(child)
        frame.append(_fill("#ffffff"))
    return page
