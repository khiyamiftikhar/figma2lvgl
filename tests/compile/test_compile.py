"""Compile checks for generated output (test strategy layers L2 and L3).

Each case runs the real CLI on a fixture, exactly as a user would, into a
temporary folder. The generated files are then compiled against real LVGL
9.3 headers: screen files and static_src/ui_style.c as C and C++, image
files as C only.

Environment:
  LVGL_DIR                    LVGL source checkout (the folder holding
                              lvgl.h). Without it these tests are skipped,
                              unless FIGMA2LVGL_REQUIRE_COMPILE=1 (CI), which
                              turns the skip into a failure.
  FIGMA2LVGL_CC               "gcc" (default: gcc/g++) or "msvc" (cl.exe).

Known bugs: KNOWN_BUGS lists the v0.4.5 bugs that break some generated
files. Such a file passes only if each listed bug shows up in the compiler
output, as an error or as a warning, and every error is explained by a
listed bug. Any other error, such as a missing header, fails the test. A
bug may show as a warning with one compiler and an error with another:
MSVC only warns about B2, gcc rejects it. B1 and B2 were fixed in v0.4.6, so
KNOWN_BUGS is empty and every case must compile cleanly.
"""

import os
import re
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = ROOT / "tests" / "fixtures"
LV_CONF_DIR = ROOT / "tests" / "lvgl"
EXAMPLE_DESIGN = ROOT / "examples" / "espidf" / "ili9486" / "design"

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
}


@dataclass
class Diagnostic:
    file: str
    line: int
    severity: str  # "error" or "warning"
    message: str


def _b1(d: Diagnostic) -> bool:
    # The button callback is emitted with the screen's leading "s" stripped.
    return "ui_ettings_on_" in d.message


def _b2(d: Diagnostic) -> bool:
    # Empty style emitted as { .text = {0}, .box = {0} }, out of declaration
    # order; C++ requires declaration order. g++ rejects it (reported at the
    # end of the initializer, so match on the message); MSVC warns
    # "designator appearing out of declaration order" and compiles.
    return "ui_style_t" in d.message and "order" in d.message.lower()


# bug -> (generated files it breaks as {(case, file, lang)}, matcher).
# Empty since v0.4.6: B1 and B2 are fixed, and probe_b1 now must compile.
# The matchers above stay as a record of how each bug showed.
KNOWN_BUGS = {}

# Fixed bugs that must not come back, not even as a warning (MSVC only
# warns about B2, so a clean exit code alone would miss it).
REGRESSIONS = {"B1": _b1, "B2": _b2}


def _expected_bugs(case: str, src: Path, lang: str):
    return {
        bug: match
        for bug, (units, match) in KNOWN_BUGS.items()
        if (case, src.name, lang) in units
    }


def _lvgl_dir() -> Path:
    value = os.environ.get("LVGL_DIR")
    if value and (Path(value) / "lvgl.h").is_file():
        return Path(value)
    message = "LVGL_DIR not set to an LVGL checkout (folder with lvgl.h)"
    if os.environ.get("FIGMA2LVGL_REQUIRE_COMPILE") == "1":
        pytest.fail(message)
    pytest.skip(message)


def _compiler() -> str:
    cc = os.environ.get("FIGMA2LVGL_CC", "gcc")
    if cc not in ("gcc", "msvc"):
        pytest.fail(f"FIGMA2LVGL_CC must be gcc or msvc, not {cc!r}")
    tool = "cl" if cc == "msvc" else "gcc"
    if shutil.which(tool) is None:
        message = f"compiler {tool} not found"
        if os.environ.get("FIGMA2LVGL_REQUIRE_COMPILE") == "1":
            pytest.fail(message)
        pytest.skip(message)
    return cc


@pytest.fixture(scope="session")
def generated(tmp_path_factory):
    """Run the CLI once per case. Returns case -> ui_src folder."""
    cache = {}

    def get(case: str) -> Path:
        if case not in cache:
            xml, images = CASES[case]
            out = tmp_path_factory.mktemp(case)
            cmd = [
                sys.executable, "-m", "figma2lvgl.main",
                "-x", str(xml), "-d", str(out), "-y",
            ]
            if images is not None:
                cmd += ["-i", str(images)]
            result = subprocess.run(
                cmd, cwd=ROOT, capture_output=True, text=True
            )
            assert result.returncode == 0, (
                f"figma2lvgl failed for {case}:\n{result.stdout}\n{result.stderr}"
            )
            cache[case] = out / "ui_src"
        return cache[case]

    return get


def _units(ui_src: Path):
    """(kind, path, languages) for every generated C file."""
    for path in sorted((ui_src / "src").glob("*.c")):
        yield "screen", path, ("c", "cxx")
    for path in sorted((ui_src / "priv_src").glob("*.c")):
        if path.name == "ui_style.c":
            yield "static", path, ("c", "cxx")
        else:
            yield "image", path, ("c",)


def _command(cc: str, lang: str, src: Path, ui_src: Path, lvgl: Path):
    # LV_LVGL_H_INCLUDE_SIMPLE as the README tells users to set: image files
    # from LVGLImage.py otherwise fall back to "lvgl/lvgl.h" (unless
    # ESP_PLATFORM), while every other generated file includes "lvgl.h".
    includes = [LV_CONF_DIR, ui_src / "include", ui_src / "priv_include", lvgl]
    if cc == "gcc":
        if lang == "c":
            cmd = ["gcc", "-std=c99", "-Werror=implicit-function-declaration"]
        else:
            cmd = ["g++", "-x", "c++", "-std=c++20"]
        cmd += ["-fsyntax-only", "-Wall", "-DLV_CONF_INCLUDE_SIMPLE", "-DLV_LVGL_H_INCLUDE_SIMPLE"]
        cmd += [f"-I{p}" for p in includes]
    else:
        cmd = ["cl", "/nologo", "/Zs", "/DLV_CONF_INCLUDE_SIMPLE", "/DLV_LVGL_H_INCLUDE_SIMPLE"]
        if lang == "c":
            # C4013: function undefined, assuming extern returning int.
            cmd += ["/TC", "/std:c11", "/we4013"]
        else:
            cmd += ["/TP", "/std:c++20"]
        cmd += [f"/I{p}" for p in includes]
    return cmd + [str(src)]


_GCC_DIAG = re.compile(
    r"^(?P<file>.*?):(?P<line>\d+):(?:\d+:)? (?:fatal )?(?P<sev>error|warning): (?P<msg>.*)$")
_MSVC_DIAG = re.compile(
    r"^(?P<file>.*?)\((?P<line>\d+)(?:,\d+)?\)\s*: (?:fatal )?(?P<sev>error|warning) (?P<code>C\d+): (?P<msg>.*)$")


def _diagnostics(cc: str, output: str):
    pattern = _MSVC_DIAG if cc == "msvc" else _GCC_DIAG
    found = []
    for line in output.splitlines():
        m = pattern.match(line.strip())
        if m:
            msg = m.group("msg")
            if cc == "msvc":
                msg = f"{m.group('code')}: {msg}"
            found.append(Diagnostic(m.group("file"), int(m.group("line")), m.group("sev"), msg))
    return found


def _params():
    for case in CASES:
        for lang in ("c", "cxx"):
            yield pytest.param(case, lang, id=f"{case}-{lang}")


@pytest.mark.parametrize("case,lang", list(_params()))
def test_generated_output_compiles(generated, case, lang):
    lvgl = _lvgl_dir()
    cc = _compiler()
    ui_src = generated(case)

    checked = 0
    for kind, src, languages in _units(ui_src):
        if lang not in languages:
            continue
        checked += 1
        result = subprocess.run(
            _command(cc, lang, src, ui_src, lvgl),
            capture_output=True, text=True,
        )
        output = result.stdout + result.stderr
        expected = _expected_bugs(case, src, lang)
        where = f"{src.name} ({cc}, {lang})"

        if not expected:
            assert result.returncode == 0, f"{where} failed to compile:\n{output}"
            back = [bug for bug, match in REGRESSIONS.items()
                    if any(match(d) for d in _diagnostics(cc, output))]
            assert not back, f"{where}: fixed bug(s) {', '.join(back)} are back:\n{output}"
            continue

        # Each known bug must still show (as an error or a warning), and no
        # error may have another cause.
        diags = _diagnostics(cc, output)
        errors = [d for d in diags if d.severity == "error"]
        missing = [bug for bug, match in expected.items() if not any(match(d) for d in diags)]
        assert not missing, (
            f"{where} no longer shows {', '.join(missing)}. "
            f"If PR 4 fixed it, remove it from KNOWN_BUGS.\n{output}"
        )
        if result.returncode != 0:
            assert errors, f"{where} failed without a parsable error:\n{output}"
        unexplained = [e for e in errors if not any(m(e) for m in expected.values())]
        assert not unexplained, (
            f"{where} has errors not explained by {', '.join(expected)}:\n"
            + "\n".join(f"  {e.file}:{e.line}: {e.message}" for e in unexplained)
            + f"\n\nFull output:\n{output}"
        )

    assert checked, f"no {lang} units generated for {case}"
