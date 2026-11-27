"""Regenerate L10 reference images (tests/snapshots/), for review.

    LVGL_DIR=... python tests/regen_snapshots.py [--case NAME ...] [--screen NAME]

Renders the chosen designs (all by default; --screen narrows to one screen)
exactly as tests/runtime/test_l10.py does, then:

- writes the new references, leaving every reference it wasn't asked to
  regenerate untouched;
- prints each screen as added, changed (with the number of changed pixels),
  unchanged, or removed (a reference whose screen no longer exists; removed
  only when the whole design is regenerated);
- writes old, new and difference images of every added or changed screen
  to snapshot_review/ (git-ignored) to look at before committing.

Regenerating is not approving: review the images (and the PR's image diff)
against Figma's export of the frame.
"""

import argparse
import os
import shutil
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))

from tests.golden_cases import CASES, REPORT_ONLY  # noqa: E402
from tests.runtime.harness import run_case  # noqa: E402
from tests.runtime.snapshots import (SNAPSHOTS, changed_pixels, diff_rows,  # noqa: E402
                                     read_png, rgb565_to_rows, write_png)

REVIEW = Path(__file__).resolve().parents[1] / "snapshot_review"


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("--case", action="append", choices=sorted(set(CASES) - REPORT_ONLY),
                        help="design to regenerate (repeatable; default: all)")
    parser.add_argument("--screen", help="only this screen (snake name)")
    args = parser.parse_args(argv)
    lvgl = Path(os.environ["LVGL_DIR"])
    cases = args.case or [c for c in CASES if c not in REPORT_ONLY]
    if REVIEW.exists():
        shutil.rmtree(REVIEW)
    summary = []
    for case in cases:
        work = Path(tempfile.mkdtemp(prefix=f"regen_{case}_"))
        result = run_case(case, lvgl, work, mode="render")
        if result.returncode != 0:
            print(result.describe())
            return 1
        for screen, (w, h, raw) in sorted(result.snapshots.items()):
            if args.screen and screen != args.screen:
                continue
            new = rgb565_to_rows(raw.read_bytes(), w, h)
            ref_path = SNAPSHOTS / case / f"{screen}.png"
            if ref_path.exists():
                old, ow, oh = read_png(ref_path)
                n = (w * h) if (ow, oh) != (w, h) else changed_pixels(new, old, w)
                if n == 0:
                    summary.append(f"unchanged  {case}/{screen}")
                    continue
                write_png(REVIEW / case / f"{screen}.old.png", old, ow, oh)
                if (ow, oh) == (w, h):
                    write_png(REVIEW / case / f"{screen}.diff.png", diff_rows(new, old, w), w, h)
                summary.append(f"changed    {case}/{screen}: {n} pixels"
                               + ("" if (ow, oh) == (w, h) else f" (size {ow}x{oh} -> {w}x{h})"))
            else:
                summary.append(f"added      {case}/{screen}")
            write_png(REVIEW / case / f"{screen}.new.png", new, w, h)
            write_png(ref_path, new, w, h)
        if not args.screen:
            for stale in sorted((SNAPSHOTS / case).glob("*.png")):
                if stale.stem not in result.snapshots:
                    stale.unlink()
                    summary.append(f"removed    {case}/{stale.stem}")
    print("\n".join(summary))
    if REVIEW.exists():
        print(f"\nreview images: {REVIEW}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
