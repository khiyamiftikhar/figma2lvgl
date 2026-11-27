"""L10: every screen draws exactly its reviewed reference image.

Design doc 12, section 5. Each screen is rendered after init, load and 100
timer cycles (animations settled) and compared pixel for pixel with
tests/snapshots/<case>/<screen>.png. A missing, stale or resized reference
fails with its own message. On a mismatch the new image, the reference and
a difference image (changed pixels red over a faded reference) are written
to the CI artifacts. Update references with tests/regen_snapshots.py.

Exact comparison (Q11): measured with tests/runtime/measure_determinism.py,
renders are identical across LVGL builds (sanitizer -O1, -O0, -O2) and
across machines (references made with gcc 13.3, compared on CI's runner).
"""

import os
from pathlib import Path

import pytest

from tests.golden_cases import CASES
from tests.runtime.harness import keep_artifacts, run_case
from tests.runtime.snapshots import (SNAPSHOTS, changed_pixels, diff_rows, read_png,
                                     rgb565_to_rows, write_png)


def _artifacts(case: str) -> Path | None:
    target = os.environ.get("FIGMA2LVGL_RUNTIME_ARTIFACTS")
    return Path(target) / case / "snapshots" if target else None


def compare(case: str, result, out: Path | None) -> list[str]:
    """Problems with one design's renders against its references; on a
    pixel mismatch, new/reference/difference images go to out (if given)."""
    refs = {p.stem for p in (SNAPSHOTS / case).glob("*.png")}
    screens = set(result.snapshots)
    problems = []
    for screen in sorted(screens - refs):
        problems.append(f"{case}/{screen}: new screen without a reference; "
                        f"run: python tests/regen_snapshots.py --case {case}")
    for screen in sorted(refs - screens):
        problems.append(f"{case}/{screen}: stale reference, the design has no such screen")

    for screen in sorted(screens & refs):
        w, h, raw_path = result.snapshots[screen]
        new = rgb565_to_rows(raw_path.read_bytes(), w, h)
        ref, rw, rh = read_png(SNAPSHOTS / case / f"{screen}.png")
        if (rw, rh) != (w, h):
            problems.append(f"{case}/{screen}: renders {w}x{h}, reference is {rw}x{rh}")
            continue
        n = changed_pixels(new, ref, w)
        if n:
            where = ""
            if out is not None:
                write_png(out / f"{screen}.new.png", new, w, h)
                write_png(out / f"{screen}.ref.png", ref, w, h)
                write_png(out / f"{screen}.diff.png", diff_rows(new, ref, w), w, h)
                where = f" (new, reference and difference images in {out})"
            problems.append(f"{case}/{screen}: {n} of {w * h} pixels differ from the "
                            f"reference{where}")
    return problems


def _render(case, lvgl, tmp_path, patch=None):
    result = run_case(case, lvgl, tmp_path, mode="render", patch=patch)
    if result.returncode != 0 or not result.done:
        keep_artifacts(tmp_path, case)
    assert result.returncode == 0 and result.done, result.describe()
    return result


@pytest.mark.parametrize("case", list(CASES))
def test_screens_match_references(case, lvgl, tmp_path):
    result = _render(case, lvgl, tmp_path)
    problems = compare(case, result, _artifacts(case))
    assert problems == [], "\n".join(problems)


def test_no_references_for_unknown_designs():
    stale = sorted(p.name for p in SNAPSHOTS.iterdir() if p.is_dir() and p.name not in CASES)
    assert stale == [], f"reference folders without a design: {stale}"


# The checker's own tests: a one-pixel move must fail with images to look at;
# a screen the references don't know must be named.

def _nudge_button(ui_src):
    c = ui_src / "src" / "ui_testscreen.c"
    text = c.read_text(encoding="utf-8")
    old = "lv_obj_set_pos(s_testscreen.btn_ok.lv_obj, 100, 380);"
    assert old in text
    c.write_text(text.replace(old, old.replace("100, 380", "101, 380"), 1), encoding="utf-8")


def test_checker_catches_a_one_pixel_move(lvgl, tmp_path):
    result = _render("golden", lvgl, tmp_path, patch=_nudge_button)
    out = tmp_path / "review"
    problems = compare("golden", result, out)
    assert len(problems) == 1 and problems[0].startswith("golden/testscreen: ")
    assert "pixels differ" in problems[0]
    assert {p.name for p in out.iterdir()} == {
        "testscreen.new.png", "testscreen.ref.png", "testscreen.diff.png"}
    diff, w, h = read_png(out / "testscreen.diff.png")
    red = sum(1 for row in diff for x in range(w) if tuple(row[3 * x:3 * x + 3]) == (255, 0, 0))
    assert red == int(problems[0].split(": ")[1].split(" ")[0])


def test_checker_names_missing_and_stale_references(lvgl, tmp_path):
    result = _render("golden", lvgl, tmp_path)
    w, h, raw = result.snapshots.pop("testscreen")
    result.snapshots["renamed"] = (w, h, raw)
    assert compare("golden", result, None) == [
        "golden/renamed: new screen without a reference; "
        "run: python tests/regen_snapshots.py --case golden",
        "golden/testscreen: stale reference, the design has no such screen"]
