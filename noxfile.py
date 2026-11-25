"""Local sessions that mirror CI (docs/planning/02_test_strategy.md in the
design repo). Run `nox -l` to list them, `nox -s compile` to run one.

    tests         pytest on the parser and generator (compile tests skipped)
    compile       generated output compiled as C and C++ with gcc/g++
    compile_msvc  the same with cl.exe (run from a VS Developer prompt)
    wheel         build the wheel, install it offline, run it with no network
    espidf        build the ESP-IDF example (needs ESP-IDF in the shell)

LVGL is fetched once at the pinned tag and commit into .nox/lvgl-<tag>.
"""

import os
import shutil
import subprocess
from pathlib import Path

import nox

ROOT = Path(__file__).parent
LVGL_TAG = "v9.3.0"
LVGL_COMMIT = "c033a98afddd65aaafeebea625382a94020fe4a7"
EXAMPLE = ROOT / "examples" / "espidf" / "ili9486"

nox.options.sessions = ["tests", "compile", "wheel"]


def lvgl_dir(session) -> Path:
    """Shallow checkout of LVGL at the pinned tag, verified by commit."""
    target = ROOT / ".nox" / f"lvgl-{LVGL_TAG}"
    if not (target / "lvgl.h").is_file():
        shutil.rmtree(target, ignore_errors=True)
        session.run("git", "clone", "--quiet", "--depth", "1", "--branch", LVGL_TAG,
                    "https://github.com/lvgl/lvgl.git", str(target), external=True)
    head = subprocess.run(["git", "-C", str(target), "rev-parse", "HEAD"],
                          capture_output=True, text=True, check=True).stdout.strip()
    if head != LVGL_COMMIT:
        session.error(f"{target} is at {head}, expected {LVGL_TAG} = {LVGL_COMMIT}")
    return target


@nox.session
def tests(session):
    session.install("-e", ".", "pytest")
    session.run("pytest", "-q", "--ignore=tests/compile", *session.posargs)


def _compile(session, cc):
    session.install("-e", ".", "pytest", "pypng", "lz4")
    env = {
        "LVGL_DIR": str(lvgl_dir(session)),
        "FIGMA2LVGL_REQUIRE_COMPILE": "1",
        "FIGMA2LVGL_CC": cc,
    }
    session.run("pytest", "-q", "tests/compile", *session.posargs, env=env)


@nox.session
def compile(session):
    _compile(session, "gcc")


@nox.session
def compile_msvc(session):
    _compile(session, "msvc")


@nox.session
def wheel(session):
    session.install("build")
    dist = Path(session.create_tmp()) / "dist"
    shutil.rmtree(dist, ignore_errors=True)
    session.run("python", "-m", "build", "--wheel", "--outdir", str(dist))
    session.run("python", "tests/wheel/check_wheel.py", str(dist))


@nox.session
def espidf(session):
    if shutil.which("idf.py") is None:
        session.skip("idf.py not found: run from an ESP-IDF terminal")
    target = session.posargs[0] if session.posargs else "esp32c3"
    session.install("-e", ".", "pypng", "lz4")
    out = Path(session.create_tmp()) / "espidf"
    shutil.rmtree(out, ignore_errors=True)
    session.run("python", "-m", "figma2lvgl.main",
                "-x", str(EXAMPLE / "design" / "ili9486.xml"),
                "-i", str(EXAMPLE / "design"), "-d", str(out), "-y",
                "--lvgl-tool", str(ROOT / "figma2lvgl" / "tools" / "LVGLImage.py"))
    build = out / f"build_{target}"
    env = {"FIGMA2LVGL_UI_SRC": str(out / "ui_src"), "IDF_TARGET": target}
    session.run("idf.py", "-C", str(EXAMPLE), "-B", str(build),
                "-D", f"SDKCONFIG={build / 'sdkconfig'}",
                "-D", f"SDKCONFIG_DEFAULTS=sdkconfig.defaults;sdkconfig.defaults.{target}",
                "set-target", target, "build", external=True, env=env)
