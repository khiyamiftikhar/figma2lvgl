"""Screen lifecycle (design doc 11, rev 6), on every test design.

v0.7.0 PR 1 ran the cases on 0.6.0's code, where each failed at a listed
check; PR 2 built the lifecycle and emptied that list except for what a
later PR builds (image hide, PR 4). A listed failure must happen at its
specific check: a failure that doesn't match, a crash, a sanitizer report,
an LVGL assert or a timeout fails the test, and so does an expected
failure that doesn't happen.
"""

import re
from pathlib import Path

import pytest

from tests.golden_cases import CASES
from tests.runtime import lifecycle as L

# case -> what the failing check must say on the code as it is. v0.7.0 PR 2
# built the lifecycle; image hide() comes with PR 4 (Q16).
EXPECTED_ON_0_6 = {
    "image_matrix": r"no ui_\w+_hide\(\)",
}

_built: dict = {}


def _exe(design: str, variant: str, lvgl: Path, tmp_path_factory) -> Path:
    key = (design, variant)
    if key not in _built:
        work = tmp_path_factory.mktemp(f"lifecycle_{design}_{variant}")
        _built[key] = L.build_design(design, lvgl, work, variant)
    return _built[key]


def judge(out: L.Outcome, expected: dict) -> None:
    """Pass, skip, xfail (only for the listed specific failure) or fail."""
    if out.done:
        if not out.ok:
            pytest.skip("; ".join(f"{s}: {why}" for s, why in out.skipped) or "nothing to run")
        if out.case in expected:
            pytest.fail(f"expected failure didn't happen: {out.design}/{out.case} passed; "
                        f"remove '{out.case}' from EXPECTED_ON_0_6")
        return
    pattern = expected.get(out.case)
    if pattern and out.clean_failure and re.search(pattern, out.failure):
        pytest.xfail(f"not built yet: {out.failure}")
    pytest.fail(out.describe())


@pytest.mark.parametrize("case", L.CASE_NAMES)
@pytest.mark.parametrize("design", list(CASES))
def test_lifecycle(design, case, lvgl, tmp_path_factory):
    exe = _exe(design, L.CASE_VARIANT.get(case, "base"), lvgl, tmp_path_factory)
    judge(L.run(design, case, exe), EXPECTED_ON_0_6)


def test_no_log_calls_without_debug(tmp_path):
    """Doc 11 rule 5: without UI_LIFECYCLE_DEBUG, no log call in the
    generated code; every LV_LOG_ is inside #ifdef UI_LIFECYCLE_DEBUG."""
    from tests.golden_cases import generate
    for design in CASES:
        ui_src = generate(design, tmp_path / design)
        for c in sorted((ui_src / "src").glob("*.c")):
            depth = 0
            for n, line in enumerate(c.read_text(encoding="utf-8").splitlines(), 1):
                stripped = line.strip()
                if stripped.startswith("#ifdef UI_LIFECYCLE_DEBUG"):
                    depth += 1
                elif stripped.startswith("#endif") and depth:
                    depth -= 1
                elif "LV_LOG_" in line and not depth:
                    pytest.fail(f"{c.name}:{n}: log call outside #ifdef UI_LIFECYCLE_DEBUG")


# The checker's own tests: a crash, a sanitizer report or a timeout is never
# an expected failure, even in a case expected to fail.

def _patch_init(text: str):
    def patch(ui_src):
        c = ui_src / "src" / "ui_testscreen.c"
        src = c.read_text(encoding="utf-8")
        marker = "void ui_testscreen_init(void)\n{\n"
        assert marker in src
        c.write_text(src.replace(marker, marker + text, 1), encoding="utf-8")
    return patch


def test_a_crash_is_not_an_expected_failure(lvgl, tmp_path):
    exe = L.build_design("golden", lvgl, tmp_path,
                         patch=_patch_init("    { volatile char b[4]; volatile int i = 4; b[i] = 1; (void)b; }\n"))
    out = L.run("golden", "init_twice", exe)
    assert not out.clean_failure and "runtime error" in out.stderr, out.describe()
    with pytest.raises(pytest.fail.Exception):
        judge(out, EXPECTED_ON_0_6)


def test_a_timeout_is_not_an_expected_failure(lvgl, tmp_path, monkeypatch):
    monkeypatch.setattr(L, "TIMEOUT_S", 5)
    exe = L.build_design("golden", lvgl, tmp_path,
                         patch=_patch_init("    for (;;) {}\n"))
    out = L.run("golden", "init_twice", exe)
    assert out.timed_out and not out.clean_failure
    with pytest.raises(pytest.fail.Exception, match="timed out"):
        judge(out, EXPECTED_ON_0_6)


def test_an_lvgl_assert_ends_the_program(lvgl, tmp_path):
    """tests/lvgl/lv_conf.h: an LVGL assert prints a marker and aborts."""
    exe = L.build_design("golden", lvgl, tmp_path,
                         patch=_patch_init("    lv_obj_set_pos(NULL, 0, 0);\n"))
    out = L.run("golden", "init_twice", exe)
    assert "ASSERT " in out.stdout and not out.timed_out and not out.clean_failure, out.describe()


# Each check catches the bug it is for: the generated code broken on purpose,
# one way per mechanism (design doc 11 rev 5 test requirements).

def _sub(rel: str, old: str, new: str):
    def patch(ui_src):
        f = ui_src / rel
        text = f.read_text(encoding="utf-8")
        assert old in text, (rel, old)
        f.write_text(text.replace(old, new, 1), encoding="utf-8")
    return patch


MUTANTS = [
    ("golden", "init_twice", _sub("src/ui_testscreen.c",
        "    if (s_testscreen.lv_screen)\n        return;\n    s_testscreen.lv_screen = lv_obj_create(NULL);",
        "    s_testscreen.lv_screen = lv_obj_create(NULL);"),
     r"second init built a new screen"),
    ("golden", "deinit_active", _sub("src/ui_testscreen.c",
        "    if (lv_screen_active() == s_testscreen.lv_screen) {",
        "    if (0) {"),
     r"deinit of the active screen changed"),
    ("golden", "deinit_inactive", _sub("src/ui_testscreen.c",
        "    s_testscreen.battery_bar.lv_obj = NULL;\n", ""),
     r"a pointer isn't NULL after deinit"),
    ("golden", "full_cycle", _sub("src/ui_testscreen.c",
        "    ui_copy_text(s_testscreen.panel_top.time.text, sizeof s_testscreen.panel_top.time.text, text);\n", ""),
     r"panel_top.time is .* \(value kept across deinit and init\)$"),
    ("events", "slider_readback", _sub("src/ui_events.c",
        "    s_events.slider_h.value = lv_slider_get_value(s_events.slider_h.lv_obj);\n", ""),
     r"slider_h value after deinit and init \(dragged\) is"),
    ("golden", "bar_target", _sub("src/ui_testscreen.c",
        "    s_testscreen.battery_bar.value = value;   /* the target: what a rebuild shows */\n", ""),
     r"battery_bar value after deinit mid-animation \(target\) is"),
    ("golden", "setters_before_init", _sub("src/ui_testscreen.c",
        "    ui_copy_text(s_testscreen.btn_ok.label_text, sizeof s_testscreen.btn_ok.label_text, text);\n", ""),
     r"btn_ok label is .* \(value set before init\)$"),
    ("golden", "text_exact", _sub("priv_src/ui_style.c",
        "            n = start;", "            (void)start;"),
     r"(cut UTF-8 character|invalid UTF-8|longer than its buffer)"),
]


@pytest.mark.parametrize("design,case,patch,expected", MUTANTS,
                         ids=[f"{d}-{c}" for d, c, _p, _e in MUTANTS])
def test_each_check_catches_its_bug(design, case, patch, expected, lvgl, tmp_path):
    exe = L.build_design(design, lvgl, tmp_path, L.CASE_VARIANT.get(case, "base"), patch=patch)
    out = L.run(design, case, exe)
    assert out.clean_failure and re.search(expected, out.failure), out.describe()
