"""Tests for figma2lvgl/core/naming.py (design doc 10, rev 4).

Section numbers refer to doc 10. Every table in the doc that the module
decides on its own is a test here: mixed keywords (3.4), ranges (4.1),
the allocator (4.5) and the naming rows of the cases table (8).

The module isn't wired into the parser yet (v0.5.0 PR 1), so the last group
of tests checks it against 0.4.6 on the real designs: same widget types and
member names, except where doc 10 section 6 lists a change.
"""

import re
import xml.etree.ElementTree as ET
from pathlib import Path

import pytest

from figma2lvgl.core import naming as nm
from figma2lvgl.core.naming import (
    Allocator, Counters, Diagnostic, ImageUse, NamingError, OutputManifest,
    Rule, ScreenSymbols, WidgetSymbols,
)
from figma2lvgl.core.widget_type import WidgetType as T

ROOT = Path(__file__).resolve().parents[1]

FILL = '<fills><fill blendMode="NORMAL" color="#ff0000" /></fills>'


def node(name, tag="Rectangle", fill=False, children="", **attrs):
    """An XML node like the FigML exporter writes."""
    extra = "".join(f' {k}="{v}"' for k, v in attrs.items())
    kids = f"<children>{children}</children>" if children else ""
    return ET.fromstring(
        f'<{tag} name="{name}"{extra}>{kids}{FILL if fill else ""}</{tag}>')


def codes(diags):
    return [d.code for d in diags]


# ── Tokens (3.1) ───────────────────────────────────────────────────────────────

@pytest.mark.parametrize("raw, words", [
    ("BatteryBar", ["battery", "bar"]),
    ("icon-wifi", ["icon", "wifi"]),
    ("Panel Content", ["panel", "content"]),
    ("silicone_pad", ["silicone", "pad"]),
    ("HTTPStatus", ["http", "status"]),
    ("row2col", ["row2col"]),
    ("battery_bar_n10_100", ["battery", "bar", "n10", "100"]),
    ("الوقت", []),
    ("  btn  OK!", ["btn", "ok"]),
])
def test_tokenize(raw, words):
    assert [t.text for t in nm.tokenize(raw)] == words


def test_token_positions_point_into_raw_name():
    raw = "Brightness Slider_0_255"
    for t in nm.tokenize(raw):
        assert raw[t.start:t.end].lower() == t.text


# ── Visibility (3.2) ───────────────────────────────────────────────────────────

def test_hidden_layer_skips_whole_subtree():
    n = node("hidden_box", "Frame", fill=True, visible="false",
             children='<Rectangle name="btn_e">' + FILL + '</Rectangle>')
    info = nm.classify(n)
    assert info.widget_type is None and info.rule is Rule.HIDDEN


def test_visible_container_hidden_child_only_child_skipped():
    child = ET.fromstring('<Rectangle name="btn_x" visible="false">' + FILL + "</Rectangle>")
    parent = node("panel_box", "Frame", fill=True, children=ET.tostring(child, "unicode"))
    assert nm.classify(parent).widget_type is T.PANEL
    assert nm.classify(child).rule is Rule.HIDDEN


def test_layer_opacity_zero_still_a_widget():
    assert nm.classify(node("btn_ghost", opacity="0", fill=True)).widget_type is T.BUTTON


@pytest.mark.parametrize("paint, visual", [
    ('<fills><fill color="#fff" /></fills>', True),
    ('<fills><fill visible="false" color="#fff" /></fills>', False),
    ('<fills><fill opacity="0" color="#fff" /></fills>', False),
    ('<fills><fill opacity="0.5" color="#fff" /></fills>', True),
    ('<strokes><stroke color="#000" /></strokes>', True),
    ('<strokes><stroke visible="false" color="#000" /></strokes>', False),
    ('<strokes><stroke opacity="0" color="#000" /></strokes>', False),
    ("", False),
])
def test_visual_properties(paint, visual):
    n = ET.fromstring(f'<Frame name="x">{paint}</Frame>')
    assert nm.has_visual_properties(n) is visual


def test_radius_alone_is_not_visual():
    assert not nm.has_visual_properties(node("x", "Frame", cornerRadius="8"))


# ── Rules (3.4) ────────────────────────────────────────────────────────────────

KID = '<Rectangle name="btn_k">' + FILL + "</Rectangle>"


@pytest.mark.parametrize("n, rule, wtype", [
    (node("anything", "Text"), Rule.R1, T.LABEL),
    (node("btn_ok"), Rule.R2, T.BUTTON),
    (node("Btn OK"), Rule.R2, T.BUTTON),
    (node("BtnOk"), Rule.R2, T.BUTTON),
    (node("btn"), Rule.R2, T.BUTTON),
    (node("slider_vol"), Rule.R3, T.SLIDER),
    (node("volume-slider"), Rule.R3, T.SLIDER),
    (node("list_items", children=KID), Rule.R4, T.DYNAMIC),
    (node("BatteryBar"), Rule.R5, T.BAR),
    (node("wifi_icon"), Rule.R6, T.IMAGE),
    (node("shape", "Vector", fill=True), Rule.R7, None),
    (node("wifi", "wifi", type="INSTANCE", children='<Vector name="v" />'), Rule.R8, T.IMAGE),
    (node("card", "card", type="INSTANCE", children=KID), Rule.R9, None),
    (node("panel_slot"), Rule.R10, T.PANEL),
    (node("Content", "Frame", fill=True, children=KID), Rule.R11, T.PANEL),
    (node("Content", "Frame", children=KID), Rule.R12, T.STRUCTURAL),
    (node("Frame_12", "Frame", children=KID), Rule.R12, T.STRUCTURAL),
    (node("Group", "Group", children=KID), Rule.R12, T.STRUCTURAL),
    (node("toolbar_bg", fill=True), Rule.R13, T.PANEL),
    (node("box", "Frame", fill=True), Rule.R13, T.PANEL),
    (node("silicone_pad"), Rule.R14, None),
    (node("dot", "Ellipse", fill=True), Rule.R14, None),
])
def test_rules(n, rule, wtype):
    info = nm.classify(n)
    assert (info.rule, info.widget_type) == (rule, wtype)


def test_instance_classified_by_type_not_tag():
    # An instance's tag is its layer name; "Text" here must not mean R1.
    n = node("Text", "Text", type="INSTANCE", children=KID)
    assert nm.classify(n).rule is Rule.R9


def test_skips_carry_a_diagnostic():
    assert codes(nm.classify(node("silicone_pad")).diagnostics) == ["skipped_leaf"]
    n = node("card", "card", type="INSTANCE", children=KID)
    assert codes(nm.classify(n).diagnostics) == ["skipped_instance"]


# Doc 10 3.4 "Mixed keywords": type, rule, and the ignored keywords.
@pytest.mark.parametrize("name, wtype, rule, ignored", [
    ("btn_slider", T.BUTTON, Rule.R2, "slider"),
    ("button_icon", T.BUTTON, Rule.R2, "icon"),
    ("image_button", T.IMAGE, Rule.R6, "button"),
    ("bar_volume_slider", T.SLIDER, Rule.R3, "bar"),
    ("volume_slider_bar", T.BAR, Rule.R5, "slider"),
    ("list_icon", T.DYNAMIC, Rule.R4, "icon"),
    ("grid_bar", T.DYNAMIC, Rule.R4, "bar"),
    ("bar_icon", T.BAR, Rule.R5, "icon"),
    ("icon_bar", T.BAR, Rule.R5, "icon"),
    ("panel_btn", T.PANEL, Rule.R10, "btn"),
    ("slider_panel", T.SLIDER, Rule.R3, "panel"),
])
def test_mixed_keywords(name, wtype, rule, ignored):
    info = nm.classify(node(name))
    assert (info.widget_type, info.rule) == (wtype, rule)
    conflict = [d for d in info.diagnostics if d.code == "keyword_conflict"]
    assert len(conflict) == 1
    assert conflict[0].old == ignored and conflict[0].new == rule.value
    assert conflict[0].layers == (name,)


def test_no_conflict_for_same_type_keywords():
    assert "keyword_conflict" not in codes(nm.classify(node("icon_image")).diagnostics)


def test_panel_marker_does_not_override_hidden():
    n = node("panel_slot", visible="false")
    assert nm.classify(n).rule is Rule.HIDDEN


def test_rotation_ignored():
    info = nm.classify(node("btn_rot", fill=True, rotation="30.00000044529205"))
    assert info.widget_type is T.BUTTON
    assert "rotation_ignored" in codes(info.diagnostics)
    assert "rotation_ignored" not in codes(nm.classify(node("btn_x", rotation="0")).diagnostics)


# ── Modifiers (3.3, 4.1) ───────────────────────────────────────────────────────

@pytest.mark.parametrize("name, base, event", [
    ("btn_ok", "btn_ok", "clicked"),
    ("btn_ok_click", "btn_ok", "clicked"),
    ("btn_ok_press", "btn_ok", "pressed"),
    ("btn_ok_release", "btn_ok", "released"),
    ("btn_ok_lp", "btn_ok", "long_pressed"),
    ("btn_ok_lpr", "btn_ok", "long_pressed_repeat"),
    ("Btn OK Press", "Btn OK", "pressed"),
])
def test_event_suffix(name, base, event):
    info = nm.classify(node(name))
    assert (info.base, info.event) == (base, event)


def test_lvgl_event_constants():
    assert nm.classify(node("btn_ok_lp")).lvgl_event == "LV_EVENT_LONG_PRESSED"
    assert nm.classify(node("btn_ok")).lvgl_event == "LV_EVENT_CLICKED"


def test_event_suffix_never_leaves_only_the_keyword():  # N5
    info = nm.classify(node("btn_press"))
    assert (info.base, info.event) == ("btn_press", "clicked")
    assert "event_suffix_kept" in codes(info.diagnostics)


def test_event_suffix_only_on_buttons():  # N5
    info = nm.classify(node("time_click", "Text"))
    assert info.base == "time_click" and info.event is None


@pytest.mark.parametrize("name, wtype, rng, base", [
    ("brightness_slider_0_255", T.SLIDER, (0, 255), "brightness_slider"),   # N24
    ("slider_vol_0_255", T.SLIDER, (0, 255), "slider_vol"),
    ("battery_bar_0_100", T.BAR, (0, 100), "battery_bar"),                   # B4
    ("battery_bar_n10_100", T.BAR, (-10, 100), "battery_bar"),
    ("slider_rev_100_0", T.SLIDER, (100, 0), "slider_rev"),                  # reversed
    ("Volume Slider 0 255", T.SLIDER, (0, 255), "Volume Slider"),
    ("slider_vol", T.SLIDER, None, "slider_vol"),
])
def test_ranges(name, wtype, rng, base):
    info = nm.classify(node(name))
    assert (info.widget_type, info.value_range, info.base) == (wtype, rng, base)


def test_range_equal_warns():
    info = nm.classify(node("slider_x_5_5"))
    assert info.value_range == (5, 5)
    assert "range_degenerate" in codes(info.diagnostics)


def test_range_outside_int32_is_an_error():
    with pytest.raises(NamingError) as e:
        nm.classify(node("slider_x_0_9999999999"))
    assert e.value.diagnostic.code == "range_out_of_bounds"
    assert e.value.diagnostic.layers == ("slider_x_0_9999999999",)


def test_numbers_stay_in_names_of_other_types():  # 3.3 step 5
    info = nm.classify(node("panel_2_3", "Frame", fill=True))
    assert info.widget_type is T.PANEL and info.value_range is None
    assert info.base == "panel_2_3"


def test_button_keeps_trailing_numbers():
    info = nm.classify(node("btn_1_2"))
    assert info.base == "btn_1_2" and info.value_range is None


# ── Identifiers (4.2, 4.3) ─────────────────────────────────────────────────────

@pytest.mark.parametrize("base, prefix, expected, code", [
    ("time", "label", "time", None),
    ("BatteryBar", "bar", "batterybar", None),           # 0.4.6 form (rev 4)
    ("Brightness Slider", "slider", "brightness_slider", None),
    ("2nd row", "label", "label_2nd_row", "renamed_invalid"),   # N3
    ("default", "label", "label_default", "renamed_invalid"),
    ("class", "label", "label_class", "renamed_invalid"),
    ("new", "button", "button_new", "renamed_invalid"),
    ("lv_logo", "image", "image_lv_logo", "renamed_invalid"),   # C5
    ("LV_Logo", "image", "image_lv_logo", "renamed_invalid"),
    ("★", "label", "label_1", "renamed_invalid"),
    ("الوقت", "label", "label_1", "renamed_invalid"),
])
def test_to_identifier(base, prefix, expected, code):
    name, diags = nm.to_identifier(base, prefix, Counters())
    assert name == expected
    assert codes(diags) == ([code] if code else [])
    assert nm.is_valid_identifier(name)


def test_empty_name_counters_per_type():
    c = Counters()
    names = [nm.to_identifier(b, p, c)[0] for b, p in
             [("★", "label"), ("ok", "label"), ("★", "label"), ("★", "button")]]
    assert names == ["label_1", "ok", "label_2", "button_1"]


@pytest.mark.parametrize("field", sorted(nm.RESERVED_FIELDS))
def test_reserved_field_never_used_as_member(field):  # N12
    info = nm.classify(node(field, "Text"))
    name, diags = nm.member_candidate(info, Counters())
    if field.startswith("lv_"):
        # C5 applies first: the LVGL prefix gets the type prefix.
        assert (name, codes(diags)) == ("label_" + field, ["renamed_invalid"])
    else:
        assert (name, codes(diags)) == ("label_" + field, ["renamed_reserved"])


def test_fixed_names_never_end_with_underscore():
    # Member names are joined with "_" into function names; a trailing "_"
    # would make "__" (found by the hostile fixture, doc 10 rev 5).
    c = Counters()
    for base in ["default", "class", "int", "2nd", "lv_x", "★", ""] + sorted(nm.RESERVED_FIELDS):
        name, _ = nm.to_identifier(base, "label", c)
        assert not name.endswith("_") and nm.is_valid_identifier(name + "_set_text"), name


def test_c1_on_hostile_names():
    hostile = ["", "_", "__x", "_X", "a__b", "9", "int", "and", "xor_eq",
               "char8_t", "nullptr", "lv_x", "LV_X", "Ünïcødé", "a-b c",
               "café", "x" * 200, "typeof", "constinit", "co_await"]
    c = Counters()
    for base in hostile:
        name, _ = nm.to_identifier(base, "label", c)
        assert nm.is_valid_identifier(name), (base, name)
        assert not name.startswith("lv_")


def test_reserved_fields_match_emitted_struct_fields():
    """4.3: the reserved set equals the fields node_emitter actually emits."""
    from figma2lvgl.core.figma_parser import ParsedNode, ParsedScreen, ParsedStyle
    from figma2lvgl.core.node_emitter import emit_screen_struct_type, emit_struct_fields
    emitted = set()
    for wtype in T:
        if wtype is T.STRUCTURAL:
            continue
        n = ParsedNode(wtype, "w", "w", 0, 0, 1, 1, ParsedStyle())
        emitted |= set(re.findall(r"\b(\w+)(?:\[[^\]]*\])?;", emit_struct_fields(n)))
    screen = ParsedScreen("s")
    emitted |= set(re.findall(r"\b(\w+);", emit_screen_struct_type(screen)))
    emitted.discard("s_s")   # the struct variable, not a field
    assert emitted == set(nm.RESERVED_FIELDS)


# ── Allocator (4.5) ────────────────────────────────────────────────────────────

@pytest.mark.parametrize("cands, expected", [
    (["button", "button_2", "button"], ["button", "button_2", "button_3"]),
    (["button", "button", "button_2"], ["button", "button_3", "button_2"]),   # natural first
    (["title", "title", "title"], ["title", "title_2", "title_3"]),           # N15
    (["a", "b"], ["a", "b"]),
])
def test_allocator(cands, expected):
    names, diags = Allocator().claim_all([(c, f"L{i}") for i, c in enumerate(cands)])
    assert names == expected
    assert len(diags) == sum(a != b for a, b in zip(cands, expected))
    assert all(d.code == "suffixed_duplicate" for d in diags)


def test_allocator_diagnostic_names_both_layers():
    _, diags = Allocator().claim_all([("x", "first"), ("x", "second")])
    assert diags[0].layers == ("second", "first")
    assert (diags[0].old, diags[0].new) == ("x", "x_2")


def test_allocator_keeps_state_between_calls():
    a = Allocator()
    a.claim_all([("x", "L1")])
    names, _ = a.claim_all([("x", "L2")])
    assert names == ["x_2"]


def test_allocator_casefold():
    names, _ = Allocator(casefold=True).claim_all([("ui_Home.c", "a"), ("ui_home.c", "b")])
    assert names == ["ui_Home.c", "ui_home.c_2"]


# ── Screens (4.2, 5) ───────────────────────────────────────────────────────────

def test_screen_names_keep_0_4_6_form():  # Q9
    names, diags = nm.assign_screen_names(["HomeScreen", "Home Screen 2", "settings"])
    assert names == ["homescreen", "home_screen_2", "settings"] and diags == []


def test_screen_collision_is_an_error():  # N11
    with pytest.raises(NamingError) as e:
        nm.assign_screen_names(["Home", "home"])
    assert e.value.diagnostic.code == "screen_collision"
    assert e.value.diagnostic.layers == ("Home", "home")


@pytest.mark.parametrize("raws, expected", [
    (["style"], ["style_2"]),                       # N17
    (["defs", "config"], ["defs_2", "config_2"]),
    (["style", "style_2"], ["style_3", "style_2"]),
])
def test_screen_reserved_stems(raws, expected):
    names, diags = nm.assign_screen_names(raws)
    assert names == expected
    assert codes(diags).count("renamed_reserved") == len(raws) - raws.count("style_2")


def test_screen_name_validated():
    names, diags = nm.assign_screen_names(["2nd page", "default"])
    assert names == ["screen_2nd_page", "screen_default"]
    assert codes(diags) == ["renamed_invalid", "renamed_invalid"]


# ── Images (4.4) ───────────────────────────────────────────────────────────────

def test_asset_name_from_base_name():
    assert nm.asset_name("icon_wifi", Counters())[0] == "icon_wifi"
    assert nm.asset_name("ui_style", Counters())[0] == "image_ui_style"    # N17
    assert nm.asset_name("lv_logo", Counters())[0] == "image_lv_logo"     # C5


def use(layer, comp, size=(48, 48), asset="icon_wifi"):
    return ImageUse(asset, layer, comp, size)


def test_same_component_shared_silently():  # N13
    assert nm.check_asset_identity([use("a", "36:3"), use("b", "36:3")]) == []


def test_different_components_error():
    with pytest.raises(NamingError) as e:
        nm.check_asset_identity([use("a", "36:3"), use("b", "40:1")])
    assert e.value.diagnostic.code == "asset_conflict"
    assert e.value.diagnostic.layers == ("a", "b")


@pytest.mark.parametrize("sizes, mentions_size", [
    (((48, 48), (48, 48)), False),
    (((48, 48), (32, 32)), True),
])
def test_unverified_identity_warns_even_when_sizes_match(sizes, mentions_size):
    diags = nm.check_asset_identity([use("a", None, sizes[0]), use("b", None, sizes[1])])
    assert codes(diags) == ["asset_unverified"]
    assert diags[0].layers == ("a", "b")
    assert ("sizes differ" in diags[0].message) is mentions_size


def test_single_use_no_warning():
    assert nm.check_asset_identity([use("a", None)]) == []


# ── External symbols (4.6) ─────────────────────────────────────────────────────

def w(key, *path, ops=(), events=(), slider=False, get=False):
    return WidgetSymbols(key, tuple(path), key, ops, events, slider, get)


def test_setters_use_full_path():  # N4
    s = ScreenSymbols("home", (
        w("deep", "a", "b", "x", "label", ops=("set_text",)),
        w("short", "b", "x", "label", ops=("set_text",)),
    ))
    names, diags = nm.assign_symbols([s], Allocator())
    assert names[("deep", "set_text")] == "ui_home_a_b_x_label_set_text"
    assert names[("short", "set_text")] == "ui_home_b_x_label_set_text"
    assert diags == []


def test_init_and_load():
    names, _ = nm.assign_symbols([ScreenSymbols("home", ())], Allocator())
    assert names[("home", "init")] == "ui_home_init"
    assert names[("home", "load")] == "ui_home_load"


def test_unique_members_keep_short_callbacks():  # C7
    s = ScreenSymbols("home", (
        w("ok", "panel_a", "btn_ok", events=("clicked",)),
        w("vol", "slider_vol", slider=True),
        w("lst", "list_items", get=True),
    ))
    names, _ = nm.assign_symbols([s], Allocator())
    assert names[("ok", "cb", "clicked")] == "ui_home_on_btn_ok_clicked"
    assert names[("vol", "cb", None)] == "ui_home_on_slider_vol"
    assert names[("lst", "get")] == "ui_home_get_list_items"


def test_same_member_in_two_panels_qualified():
    s = ScreenSymbols("home", (
        w("a", "panel_a", "btn_ok", events=("clicked",)),
        w("b", "panel_b", "btn_ok", events=("clicked",)),
    ))
    names, diags = nm.assign_symbols([s], Allocator())
    assert names[("a", "cb", "clicked")] == "ui_home_on_panel_a_btn_ok_clicked"
    assert names[("b", "cb", "clicked")] == "ui_home_on_panel_b_btn_ok_clicked"
    assert diags == []


def test_two_level_qualification():
    s = ScreenSymbols("home", (
        w("a", "x", "panel_a", "btn_ok", events=("clicked",)),
        w("b", "y", "panel_a", "btn_ok", events=("clicked",)),
    ))
    names, _ = nm.assign_symbols([s], Allocator())
    assert names[("a", "cb", "clicked")] == "ui_home_on_x_panel_a_btn_ok_clicked"
    assert names[("b", "cb", "clicked")] == "ui_home_on_y_panel_a_btn_ok_clicked"


def test_different_events_need_no_qualification():
    s = ScreenSymbols("home", (
        w("a", "panel_a", "btn_ok", events=("clicked",)),
        w("b", "panel_b", "btn_ok", events=("pressed",)),
    ))
    names, _ = nm.assign_symbols([s], Allocator())
    assert names[("a", "cb", "clicked")] == "ui_home_on_btn_ok_clicked"
    assert names[("b", "cb", "pressed")] == "ui_home_on_btn_ok_pressed"


def test_flattened_paths_get_a_suffix():
    s = ScreenSymbols("home", (
        w("first", "panel_a", "btn_ok", ops=("set_label",)),
        w("second", "panel", "a_btn_ok", ops=("set_label",)),
    ))
    names, diags = nm.assign_symbols([s], Allocator(code="flattened_collision"))
    assert names[("first", "set_label")] == "ui_home_panel_a_btn_ok_set_label"
    assert names[("second", "set_label")] == "ui_home_panel_a_btn_ok_set_label_2"
    assert codes(diags) == ["flattened_collision"]
    assert diags[0].layers == ("second", "first")


def test_symbols_across_screens_natural_first():
    # Screen "home" widget "main_btn" and screen "home_main" widget "btn"
    # flatten alike; the second gets the suffix, in XML order.
    a = ScreenSymbols("home", (w("x", "main_btn", ops=("set_label",)),))
    b = ScreenSymbols("home_main", (w("y", "btn", ops=("set_label",)),))
    names, diags = nm.assign_symbols([a, b], Allocator())
    assert names[("x", "set_label")] == "ui_home_main_btn_set_label"
    assert names[("y", "set_label")] == "ui_home_main_btn_set_label_2"
    assert len(diags) == 1


# ── Whole-design check (5) ─────────────────────────────────────────────────────

def test_check_namespaces():
    m = OutputManifest(
        symbols=[("ui_home_init", "home"), ("ui_home_init", "Home")],
        macros=[("UI_HOME_H", "home"), ("UI_STYLE_H", "bundled")],
        files=[("src/ui_Home.c", "Home"), ("src/ui_home.c", "home")],
    )
    diags = nm.check_namespaces(m)
    assert [d.old for d in diags] == ["ui_home_init", "src/ui_home.c"]
    assert all(d.code == "internal_collision" for d in diags)


def test_check_namespaces_clean():
    m = OutputManifest(symbols=[("a", "x")], macros=[("A", "x")], files=[("a.c", "x")])
    assert nm.check_namespaces(m) == []


# ── Compatibility with 0.4.6 on the real designs (C7) ─────────────────────────

REAL = [
    ROOT / "tests" / "fixtures" / "testscreen.xml",
    ROOT / "examples" / "espidf" / "ili9486" / "design" / "ili9486.xml",
    ROOT / "tests" / "fixtures" / "real" / "selection.xml",
]


def _old_member(name):
    """0.4.6's member name, before duplicate renaming (figma_parser)."""
    from figma2lvgl.core.utils.figma_helpers import parse_widget_name
    from figma2lvgl.core.utils.utils import normalize_id
    base, _ = parse_widget_name(name)
    return normalize_id(re.sub(r"_(n?\d+)_(n?\d+)$", "", base))


def _screens(path):
    root = ET.parse(path).getroot()
    if root.tag == "Frame":
        return [root]
    return [f for f in root.find("children") if f.tag == "Frame"]


@pytest.mark.parametrize("path", REAL, ids=[p.stem for p in REAL])
def test_real_designs_classify_and_name_like_0_4_6(path):
    """Every node of the real designs gets 0.4.6's type and member name.

    Doc 10 section 6 lists where v0.5.0 differs; none of those cases occur
    in these designs, so any difference here is a bug in naming.py.
    """
    from figma2lvgl.core.utils.figma_helpers import detect_widget_type
    checked = 0

    def walk(parent):
        nonlocal checked
        for child in nm._children(parent):
            old = detect_widget_type(child)
            info = nm.classify(child)
            assert info.widget_type == old, child.attrib.get("name")
            if info.widget_type not in (None, T.STRUCTURAL):
                new, _ = nm.member_candidate(info, Counters())
                assert new == _old_member(child.attrib.get("name", "")), child.attrib.get("name")
                checked += 1
            if info.widget_type in (T.PANEL, T.STRUCTURAL):
                walk(child)

    for screen in _screens(path):
        walk(screen)
    assert checked > 0
