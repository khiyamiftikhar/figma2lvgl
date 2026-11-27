"""Are L10 renders identical whatever LVGL is built with? (doc 12, Q11)

Renders every golden design with LVGL and the generated code built three
ways (the sanitizer build the tests use, plain -O0, plain -O2) and compares
the frames pixel for pixel. Prints one line per screen and a verdict. Run
on different machines or runner images and compare the printed hashes.

    LVGL_DIR=... python -m tests.runtime.measure_determinism
"""

import hashlib
import os
import sys
import tempfile
from pathlib import Path

from tests.golden_cases import CASES
from tests.runtime.harness import run_case
from tests.runtime.lvgl_lib import SAN_FLAGS, gcc_version

BUILDS = {
    "sanitizer -O1": SAN_FLAGS,
    "plain -O0": ["-O0", "-g"],
    "plain -O2": ["-O2"],
}


def main() -> int:
    lvgl = Path(os.environ["LVGL_DIR"])
    print(f"gcc {gcc_version()}")
    frames = {}                                  # (case, screen) -> {build: bytes}
    for build, flags in BUILDS.items():
        for case in CASES:
            work = Path(tempfile.mkdtemp(prefix=f"det_{case}_"))
            r = run_case(case, lvgl, work, mode="render", flags=flags)
            if r.returncode != 0:
                print(r.describe())
                return 2
            for screen, (_w, _h, path) in r.snapshots.items():
                frames.setdefault((case, screen), {})[build] = path.read_bytes()
    identical = True
    for (case, screen), by_build in sorted(frames.items()):
        hashes = {b: hashlib.sha256(d).hexdigest()[:12] for b, d in by_build.items()}
        same = len(set(hashes.values())) == 1
        identical &= same
        print(f"{'same' if same else 'DIFFERENT':9} {case}/{screen}: "
              + ", ".join(f"{b}={h}" for b, h in hashes.items()))
    print("verdict:", "identical across builds" if identical else "builds differ")
    return 0 if identical else 1


if __name__ == "__main__":
    sys.exit(main())
