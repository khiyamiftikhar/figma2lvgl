"""Showcase progress report (design doc 12, section 7): information only.

Prints, per frame of the showcase and the feature fixtures, the generated
share and the similarity to Figma's own PNG export, and adds them to the CI
job summary. Nothing here fails on the numbers; the render must only run.
"""

import os

import pytest

from tests.golden_cases import CASES
from tests.runtime.harness import keep_artifacts, run_case
from tests.runtime.showcase import FIGMA_PNGS, metrics, report


@pytest.mark.parametrize("case", ["showcase", "features"])
def test_progress_report(case, lvgl, tmp_path):
    result = run_case(case, lvgl, tmp_path, mode="render")
    if result.returncode != 0 or not result.done:
        keep_artifacts(tmp_path, case)
    assert result.returncode == 0 and result.done, result.describe()
    rows = metrics(result, CASES[case][0], FIGMA_PNGS[case])
    assert [r[0] for r in rows] == result.screens
    assert all(sim is not None for _, _, _, sim in rows), "a frame without Figma's PNG"
    text = report(case, rows)
    print(text)
    summary = os.environ.get("GITHUB_STEP_SUMMARY")
    if summary:
        with open(summary, "a", encoding="utf-8") as f:
            f.write(f"```\n{text}\n```\n")
