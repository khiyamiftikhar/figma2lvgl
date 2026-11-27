"""Designs whose complete output is kept in tests/golden/full/.

Shared by tests/test_golden_output.py and tests/regen_golden.py. The same
designs as the compile tests (tests/compile/test_compile.py, CASES).
"""

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "tests" / "fixtures"
EXAMPLE_DESIGN = ROOT / "examples" / "espidf" / "ili9486" / "design"
FULL_GOLDEN = ROOT / "tests" / "golden" / "full"

# name -> (xml, images folder or None)
CASES = {
    "golden": (FIXTURES / "testscreen.xml", None),
    "example": (EXAMPLE_DESIGN / "ili9486.xml", EXAMPLE_DESIGN),
    "selection": (FIXTURES / "real" / "selection.xml", EXAMPLE_DESIGN),
    "probe_b1": (FIXTURES / "probe_b1_settings.xml", None),
    "coords": (FIXTURES / "coords" / "coords.xml", None),
    "coords2": (FIXTURES / "coords" / "coords2.xml", None),
    "classify": (FIXTURES / "naming_classify.xml", EXAMPLE_DESIGN),
    "hostile": (FIXTURES / "hostile" / "hostile.xml", FIXTURES / "hostile"),
    "events": (FIXTURES / "events.xml", None),
}


def generate(case: str, out: Path) -> Path:
    """Run the CLI on a case into ``out``. Returns the ui_src folder."""
    xml, images = CASES[case]
    cmd = [sys.executable, "-m", "figma2lvgl.main", "-x", str(xml), "-d", str(out), "-y"]
    if images is not None:
        cmd += ["-i", str(images)]
    result = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"figma2lvgl failed for {case}:\n{result.stdout}\n{result.stderr}")
    return out / "ui_src"


def read_tree(folder: Path) -> dict[str, str]:
    """Relative path -> text, line endings normalized to LF.

    Git checks golden files out with LF everywhere (.gitattributes), and the
    generator writes the platform's line endings, so the comparison ignores
    CR. Every other byte must match.
    """
    return {
        p.relative_to(folder).as_posix(): p.read_bytes().decode("utf-8").replace("\r\n", "\n")
        for p in sorted(folder.rglob("*")) if p.is_file()
    }
