"""Property tests (L8), compiled: random designs build as C and C++.

The designs of tests/test_property_naming.py, built against LVGL 9.3 with
gcc (gcc only; skipped with MSVC). Per design, two compiler runs:

- every screen's .c in one C translation unit, so a function defined twice
  across screens is a compile error too (C3), with -Wall -Werror;
- every generated header included from one C++ file (C1 for C++ users).

Runs with the "full" and "nightly" Hypothesis profiles only (PRs, main,
tags, nightly); feature-branch pushes run the uncompiled tests.
"""

import subprocess
import tempfile
from pathlib import Path

import pytest

pytest.importorskip("hypothesis")

from hypothesis import assume, given, note  # noqa: E402

from tests.compile.test_compile import LV_CONF_DIR, _compiler, _lvgl_dir  # noqa: E402
from tests.conftest import hypothesis_profile  # noqa: E402
from tests.naming_strategies import designs  # noqa: E402
from tests.property_support import run_design, to_xml, write_tree  # noqa: E402

FLAGS = ["-fsyntax-only", "-Wall", "-Werror",
         "-DLV_CONF_INCLUDE_SIMPLE", "-DLV_LVGL_H_INCLUDE_SIMPLE"]


@pytest.fixture(scope="module")
def toolchain():
    if hypothesis_profile() == "ci":
        pytest.skip("compiled property tests run with the full and nightly profiles")
    lvgl = _lvgl_dir()
    if _compiler() != "gcc":
        pytest.skip("compiled property tests use gcc only")
    return lvgl


def _compile(cmd, source, includes):
    result = subprocess.run(cmd + FLAGS + [f"-I{i}" for i in includes] + [str(source)],
                            capture_output=True, text=True)
    assert result.returncode == 0, result.stderr[-4000:]


@given(page=designs())
def test_random_designs_compile(toolchain, page):
    note(to_xml(page))
    run = run_design(page)
    assume(run.error is None)
    with tempfile.TemporaryDirectory() as tmp:
        ui = write_tree(run, Path(tmp))
        includes = [ui / "include", ui / "priv_include", toolchain, LV_CONF_DIR]

        # All screens in one C unit; each screen's file-static helper renamed
        # so the only possible clashes are between external names.
        unit = Path(tmp) / "all_screens.c"
        lines = []
        for i, c_file in enumerate(sorted((ui / "src").glob("*.c"))):
            lines += [f"#define bar_anim_exec_cb bar_anim_exec_cb_{i}",
                      f'#include "{c_file.as_posix()}"',
                      "#undef bar_anim_exec_cb"]
        unit.write_text("\n".join(lines) + "\n", encoding="utf-8")
        _compile(["gcc", "-std=c99", "-Werror=implicit-function-declaration"], unit, includes)

        cpp = Path(tmp) / "headers.cpp"
        cpp.write_text("".join(f'#include "{h.name}"\n'
                               for h in sorted((ui / "include").glob("*.h"))),
                       encoding="utf-8")
        _compile(["g++", "-x", "c++", "-std=c++20"], cpp, includes)
