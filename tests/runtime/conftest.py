"""Fixtures for the run-time tests (design doc 12).

Needs gcc and LVGL_DIR (an LVGL v9.3.0 checkout). Without them the tests
skip, unless FIGMA2LVGL_REQUIRE_RUNTIME=1 (CI), where they fail.
"""

import os
import shutil
from pathlib import Path

import pytest

from tests.runtime.lvgl_lib import build_library


def _need(message):
    if os.environ.get("FIGMA2LVGL_REQUIRE_RUNTIME") == "1":
        pytest.fail(message)
    pytest.skip(message)


@pytest.fixture(scope="session")
def lvgl():
    value = os.environ.get("LVGL_DIR")
    if not value or not (Path(value) / "lvgl.h").is_file():
        _need("LVGL_DIR not set to an LVGL checkout (folder with lvgl.h)")
    if shutil.which("gcc") is None:
        _need("gcc not found")
    path = Path(value)
    build_library(path)          # build or reuse once, before the first test
    return path
