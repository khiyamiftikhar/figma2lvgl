"""L9: every design runs under the sanitizers with every setter checked.

Design doc 12, section 4. One C program per design (tests/runtime/
harness.py): every screen in XML order, init, load, every setter with edge
values, each result checked through LVGL's public API, timers, lv_deinit.
Built with AddressSanitizer, UndefinedBehaviorSanitizer and LeakSanitizer.
"""

import pytest

from tests.golden_cases import CASES
from tests.runtime.harness import keep_artifacts, run_case


@pytest.mark.parametrize("case", list(CASES))
def test_design_runs_clean(case, lvgl, tmp_path):
    result = run_case(case, lvgl, tmp_path)
    ok = (result.returncode == 0 and result.done and not result.failures
          and result.setters_called == result.expected_setters)
    if not ok:
        keep_artifacts(tmp_path, case)
    assert result.returncode == 0 and not result.failures, result.describe()
    assert result.done, result.describe()
    assert result.not_reached == [], result.describe()
    assert result.setters_called == result.expected_setters, (
        f"{result.setters_called} of {result.expected_setters} setters called")
    assert result.callbacks == []          # nothing pressed in L9


# The harness's own tests: generated code broken on purpose must fail, and
# the message must name the screen and step (doc 12, section 4, Failure).

def _break_label_setter(ui_src):
    c = ui_src / "src" / "ui_testscreen.c"
    text = c.read_text(encoding="utf-8")
    assert "lv_label_set_text(s_testscreen.panel_top.time.lv_obj, text);" in text
    c.write_text(text.replace("lv_label_set_text(s_testscreen.panel_top.time.lv_obj, text);",
                              "(void)text;", 1), encoding="utf-8")


def _overflow_label_setter(ui_src):
    c = ui_src / "src" / "ui_testscreen.c"
    text = c.read_text(encoding="utf-8")
    c.write_text(text.replace(
        "lv_label_set_text(s_testscreen.panel_top.time.lv_obj, text);",
        "{ static char buf[8]; strcpy(buf, text); lv_label_set_text(s_testscreen.panel_top.time.lv_obj, buf); }",
        1).replace('#include "ui_testscreen.h"', '#include <string.h>\n#include "ui_testscreen.h"', 1),
        encoding="utf-8")


def test_harness_catches_a_setter_that_does_nothing(lvgl, tmp_path):
    result = run_case("golden", lvgl, tmp_path, patch=_break_label_setter)
    assert result.returncode == 1
    assert result.failures and result.failures[0].startswith("testscreen panel_top.time set_text #0:")
    assert "panel_top.time set_text #0" in result.describe()


def test_harness_catches_a_buffer_overflow(lvgl, tmp_path):
    result = run_case("golden", lvgl, tmp_path, patch=_overflow_label_setter)
    assert result.returncode != 0
    assert "AddressSanitizer: global-buffer-overflow" in result.stderr
    assert result.steps[-1] == ("testscreen", "panel_top.time set_text #2")
