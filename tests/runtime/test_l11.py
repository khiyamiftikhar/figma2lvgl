"""L11: pressing each button and dragging each slider calls the right callbacks.

Design doc 12, section 6. A scripted pointer presses every button (and holds
buttons with _lp/_lpr) at its test point and drags every slider from its
minimum end to its maximum end. Each step must record exactly the expected
callbacks, in order, and nothing else; a slider's callback runs once per
change of its value. A widget that LVGL's own hit test
doesn't find at its test point is "covered" (or "not visible") and fails,
except in the events fixture, which asserts the detector itself.

LVGL 9.3's event order within one press, confirmed with the events fixture:
PRESSED at press, RELEASED at release, then CLICKED. LONG_PRESSED after
400 ms, LONG_PRESSED_REPEAT every 100 ms after that, and CLICKED on release
after a long press too (finding N25). Since v0.7.0 (Q15) a button with _lp
or _lpr registers its clicked callback on SHORT_CLICKED, so a hold calls
only the long-press callbacks; a tap still calls clicked. The pointer is
driven by a simulated 5 ms clock, so the repeat count is exact.
"""

import pytest

from figma2lvgl.core.widget_type import WidgetType as T
from tests.golden_cases import CASES, REPORT_ONLY
from tests.runtime.harness import keep_artifacts, run_case

# The events fixture's and the f_covering frame's own cases: one button
# under another must be reported covered by exactly that button; the button
# under a filled panel must not (panels aren't clickable, so LVGL's hit test
# looks through them).
EXPECTED_COVERED = {
    "events": [("events", "btn_under_button", "btn_over_button")],
    "features": [("f_covering", "btn_under_button", "btn_on_top")],
}


# Repeats in the harness's 1000 ms hold (harness.py), with LVGL 9.3's
# defaults (LONG_PRESSED after 400 ms, LONG_PRESSED_REPEAT every 100 ms) and
# the simulated 5 ms clock: measured, and the same on every run, because
# LVGL sees the press and the release only when its input timer reads the
# pointer, not at the exact millisecond the harness changes it.
LPR_REPEATS = 5


def _widgets(screen):
    def walk(nodes, prefix):
        for n in nodes:
            member = prefix + (n.id,)
            yield n, ".".join(member)
            yield from walk(n.children, member)
    return list(walk(screen.children, ()))


def _check_button(result, screen, node, name, failures):
    sym = node.symbols
    cb = lambda ev: sym[("cb", ev)]
    mods = set(node.event_modifiers)
    tap = ([cb("pressed")] if "press" in mods else []) \
        + ([cb("released")] if "release" in mods else []) + [cb("clicked")]
    got = result.step_callbacks.get((screen.snake, f"{name} tap"), [])
    if got != tap:
        failures.append(f"{screen.snake} {name} tap: callbacks {got}, expected {tap}")
    if "lp" in mods:
        want = [cb("long_pressed")]                       # no clicked (Q15)
        got = result.step_callbacks.get((screen.snake, f"{name} long press"), [])
        if got != want:
            failures.append(f"{screen.snake} {name} long press: callbacks {got}, expected {want}")
    if "lpr" in mods:
        got = result.step_callbacks.get((screen.snake, f"{name} long press"), [])
        want = [cb("long_pressed_repeat")] * LPR_REPEATS   # no clicked (Q15)
        if got != want:
            failures.append(f"{screen.snake} {name} long press: callbacks {got}, "
                            f"expected {LPR_REPEATS} x {cb('long_pressed_repeat')}")


def _check_slider(result, screen, node, name, failures):
    want = node.symbols[("cb", None)]
    got = result.step_callbacks.get((screen.snake, f"{name} drag"), [])
    lo, hi = sorted((node.slider_min, node.slider_max))
    value = result.values.get((screen.snake, name))
    if lo == hi:
        # An empty range (naming warns about it) can't move: no event.
        if got or value != lo:
            failures.append(f"{screen.snake} {name} drag: range {lo}..{hi} can't move, "
                            f"but callbacks {got}, value {value}")
        return
    # One VALUE_CHANGED callback per change of the value during the drag.
    seen = result.drag_values.get((screen.snake, f"{name} drag"), [])
    changes = sum(1 for a, b in zip(seen, seen[1:]) if a != b)
    if not got or any(c != want for c in got) or len(got) != changes:
        failures.append(f"{screen.snake} {name} drag: callbacks {got}, expected "
                        f"{want} once per value change ({changes}: {seen})")
    if value is None or value < hi - (hi - lo) * 0.15:
        failures.append(f"{screen.snake} {name} drag: value {value}, expected the top 15% of {lo}..{hi}")


def touch_failures(result, case) -> list:
    """Every way a touch run differs from doc 12, section 6, as messages."""
    failures = []
    covered = [(s, m, by) for s, m, by, _x, _y in result.covered]
    if covered != EXPECTED_COVERED.get(case, []):
        failures.append("covered widgets: " + "; ".join(
            f"{m} by {by} at ({x}, {y})" for _s, m, by, x, y in result.covered))
    if result.not_visible:
        failures.append(f"not visible: {result.not_visible}")
    skip = {(s, m) for s, m, _by in covered}

    for screen in result.model:
        for node, name in _widgets(screen):
            if (screen.snake, name) in skip:
                if result.step_callbacks.get((screen.snake, f"{name} tap")):
                    failures.append(f"{name} is covered but a callback ran")
                continue
            if node.widget_type == T.BUTTON:
                _check_button(result, screen, node, name, failures)
            elif node.widget_type == T.SLIDER:
                _check_slider(result, screen, node, name, failures)
    # Steps that aren't presses (init, load, settle) record no callbacks.
    for (s, st), cbs in result.step_callbacks.items():
        if not st.endswith((" tap", " long press", " drag")):
            failures.append(f"{s} {st}: unexpected callbacks {cbs}")
    return failures


@pytest.mark.parametrize("case", [c for c in CASES if c not in REPORT_ONLY])
def test_presses_call_the_right_callbacks(case, lvgl, tmp_path):
    result = run_case(case, lvgl, tmp_path, mode="touch")
    if result.returncode != 0 or not result.done:
        keep_artifacts(tmp_path, case)
    assert result.returncode == 0 and result.done, result.describe()
    failures = touch_failures(result, case)
    if failures:
        keep_artifacts(tmp_path, case)
    assert failures == [], "\n".join(failures)


def _unregister_plain_click(ui_src):
    c = ui_src / "src" / "ui_events.c"
    text = c.read_text(encoding="utf-8")
    line = "lv_obj_add_event_cb(s_events.btn_plain.lv_obj, ui_events_on_btn_plain_clicked,"
    assert line in text
    c.write_text(text.replace(line, "if (0) " + line, 1), encoding="utf-8")


def test_checker_catches_a_missing_callback(lvgl, tmp_path):
    """The checker's own test: an unregistered callback must be reported."""
    result = run_case("events", lvgl, tmp_path, mode="touch", patch=_unregister_plain_click)
    assert result.returncode == 0
    assert touch_failures(result, "events") == [
        "events btn_plain tap: callbacks [], expected ['ui_events_on_btn_plain_clicked']"]


def test_events_fixture_exercises_every_kind(lvgl, tmp_path):
    """The events fixture really covers what L11 claims (doc 12, section 6)."""
    result = run_case("events", lvgl, tmp_path, mode="touch")
    steps = {st for _s, st in result.step_callbacks}
    for step in ["btn_a tap", "btn_b tap", "btn_c long press", "btn_d long press",
                 "slider_h drag", "slider_v drag", "slider_rev drag", "slider_vrev drag",
                 "btn_under_panel tap"]:
        assert step in steps, step
    assert result.values[("events", "slider_vrev")] >= 35       # range -50..50, reversed


def _long_press_also_clicks(ui_src):
    """0.6.0's registration for btn_c (_lp): clicked on LV_EVENT_CLICKED."""
    c = ui_src / "src" / "ui_events.c"
    text = c.read_text(encoding="utf-8")
    old = ("lv_obj_add_event_cb(s_events.btn_c.lv_obj, ui_events_on_btn_c_clicked,\n"
           "                        LV_EVENT_SHORT_CLICKED, NULL);")
    assert old in text
    c.write_text(text.replace(old, old.replace("SHORT_CLICKED", "CLICKED"), 1), encoding="utf-8")


def test_checker_catches_a_long_press_that_also_clicks(lvgl, tmp_path):
    """Q15: a hold on an _lp button must not call clicked (N25)."""
    result = run_case("events", lvgl, tmp_path, mode="touch", patch=_long_press_also_clicks)
    failures = touch_failures(result, "events")
    assert failures == [
        "events btn_c long press: callbacks ['ui_events_on_btn_c_long_pressed', "
        "'ui_events_on_btn_c_clicked'], expected ['ui_events_on_btn_c_long_pressed']"]
