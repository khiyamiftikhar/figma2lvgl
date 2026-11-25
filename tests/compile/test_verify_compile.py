"""--verify-compile against real LVGL headers (fix B7).

Needs LVGL_DIR like the other compile tests, and gcc (the check uses gcc).
"""

import os
import shutil
from pathlib import Path

import pytest

from figma2lvgl import main as cli
from figma2lvgl.core import output

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = ROOT / "tests" / "fixtures"
LV_CONF_DIR = ROOT / "tests" / "lvgl"
EXAMPLE_DESIGN = ROOT / "examples" / "espidf" / "ili9486" / "design"


@pytest.fixture
def lvgl_dir():
    if os.environ.get("FIGMA2LVGL_CC", "gcc") != "gcc":
        pytest.skip("--verify-compile uses gcc; covered by the gcc compile job")
    value = os.environ.get("LVGL_DIR")
    message = None
    if not value or not (Path(value) / "lvgl.h").is_file():
        message = "LVGL_DIR not set to an LVGL checkout"
    elif shutil.which("gcc") is None:
        message = "gcc not found (--verify-compile uses gcc)"
    if message:
        if os.environ.get("FIGMA2LVGL_REQUIRE_COMPILE") == "1":
            pytest.fail(message)
        pytest.skip(message)
    return Path(value)


def cli_run(*args):
    try:
        cli.main([str(a) for a in args])
    except SystemExit as e:
        return e.code or 0
    return 0


def test_verify_compile_passes_with_real_headers(tmp_path, lvgl_dir):
    code = cli_run("-x", EXAMPLE_DESIGN / "ili9486.xml", "-i", EXAMPLE_DESIGN,
                   "-d", tmp_path, "-y", "--verify-compile",
                   "--lvgl-include", lvgl_dir, "--lvgl-include", LV_CONF_DIR)
    assert code == 0
    assert (tmp_path / output.UI_SRC / "src" / "ui_home.c").is_file()


def test_failed_compile_check_keeps_previous_output(tmp_path, lvgl_dir, caplog, monkeypatch):
    assert cli_run("-x", FIXTURES / "probe_b1_settings.xml", "-d", tmp_path, "-y") == 0
    before = {p.relative_to(tmp_path).as_posix(): p.read_bytes()
              for p in (tmp_path / output.UI_SRC).rglob("*") if p.is_file()}

    # A generated screen with a C syntax error: the check must fail.
    real = cli.generate_screen

    def broken(screen):
        c_name, h_name, h_text, c_text = real(screen)
        return c_name, h_name, h_text, c_text + "\nint broken_on_purpose = ;\n"

    monkeypatch.setattr(cli, "generate_screen", broken)
    code = cli_run("-x", EXAMPLE_DESIGN / "ili9486.xml", "-i", EXAMPLE_DESIGN,
                   "-d", tmp_path, "-y", "--verify-compile",
                   "--lvgl-include", lvgl_dir, "--lvgl-include", LV_CONF_DIR)
    assert code == 1
    assert "compile check failed" in caplog.text
    after = {p.relative_to(tmp_path).as_posix(): p.read_bytes()
             for p in (tmp_path / output.UI_SRC).rglob("*") if p.is_file()}
    assert after == before
    assert not (tmp_path / output.STAGING_NAME).exists()
