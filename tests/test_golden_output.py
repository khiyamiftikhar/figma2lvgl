"""Complete generated output, compared with the committed golden files.

Each case runs the real CLI and compares every file it writes under ui_src/
(screens, images, assets.h, ui_config.h, bundled files) with
tests/golden/full/<case>/. Any difference fails: a change to the output must
be intended, reviewed in its PR and committed with tests/regen_golden.py.

v0.5.0 PR 1 commits 0.4.6's output as the baseline, so each later v0.5.0 PR
shows exactly which generated lines it changes (design doc 10, section 6).
"""

import difflib

import pytest

from tests.golden_cases import CASES, FULL_GOLDEN, generate, read_tree


@pytest.mark.parametrize("case", list(CASES))
def test_output_matches_golden(case, tmp_path):
    actual = read_tree(generate(case, tmp_path))
    expected = read_tree(FULL_GOLDEN / case)

    missing = sorted(set(expected) - set(actual))
    extra = sorted(set(actual) - set(expected))
    assert not missing and not extra, (
        f"file list changed for {case}: missing {missing}, new {extra}. "
        f"Run tests/regen_golden.py if intended.")

    changed = [name for name in expected if actual[name] != expected[name]]
    if changed:
        name = changed[0]
        diff = "".join(difflib.unified_diff(
            expected[name].splitlines(True), actual[name].splitlines(True),
            f"golden/{name}", f"generated/{name}", n=2))
        pytest.fail(
            f"{len(changed)} file(s) differ for {case}: {changed}. First diff:\n"
            f"{diff[:4000]}\nRun tests/regen_golden.py if intended.")
