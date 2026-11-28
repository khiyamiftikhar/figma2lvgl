"""Screen lifecycle (design doc 11, rev 6), on every test design.

v0.7.0 PR 1: the cases run on the code as it is (0.6.0's lifecycle) and
fail where doc 11 says it is wrong. Each such failure is listed in
EXPECTED_ON_0_6 with the specific check it must fail on: a failure that
doesn't match, a crash, a sanitizer report, an LVGL assert or a timeout
fails the test, and so does an expected failure that doesn't happen. PR 2
(the lifecycle) empties the table, PR 4 (images) removes the last entry.
"""

import re
from pathlib import Path

import pytest

from tests.golden_cases import CASES
from tests.runtime import lifecycle as L

# case -> what the failing check must say, on 0.6.0's code.
NO_DEINIT = r"no ui_\w+_deinit\(\)"
EXPECTED_ON_0_6 = {
    "init_twice": r"second init built a new screen \(pointers changed\)$",
    "setters_before_init": r"expected .* \(value set before init\)$",
    "deinit_active": NO_DEINIT,
    "deinit_inactive": NO_DEINIT,
    "full_cycle": NO_DEINIT,
    "cycles_100": NO_DEINIT,
    "setters_after_deinit": NO_DEINIT,
    "slider_readback": NO_DEINIT,
    "bar_target": NO_DEINIT,
    "image_matrix": NO_DEINIT,
    # 0.6.0: setters don't store into the struct, button label_text is a
    # pointer, and design text is cut at 29 bytes.
    "text_exact": (r"(after a text longer than its buffer is|the text field is a pointer"
                   r"|design text after init is)"),
    "length64": r"buffer is \d+ bytes with UI_MAX_STRING_LENGTH=64$",
    "debug": NO_DEINIT,
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
        pytest.xfail(f"0.6.0: {out.failure}")
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
