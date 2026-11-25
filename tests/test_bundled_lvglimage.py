"""The bundled LVGLImage.py is LVGL's file, unmodified, with a provenance
header (fix B6). The header records the upstream tag, commit and SHA-256;
removing it must give back exactly the upstream file."""

import hashlib
import re
from pathlib import Path

TOOL = Path(__file__).resolve().parents[1] / "figma2lvgl" / "tools" / "LVGLImage.py"
START = "# --- figma2lvgl: bundled third-party file"
END = "# --- end of figma2lvgl header"


def _split():
    text = TOOL.read_bytes().decode("utf-8").replace("\r\n", "\n")
    lines = text.split("\n")
    start = next(i for i, l in enumerate(lines) if l.startswith(START))
    end = next(i for i, l in enumerate(lines) if l.startswith(END))
    return lines[start:end + 1], lines[:start] + lines[end + 1:]


def test_header_records_source_and_licence():
    header, _ = _split()
    text = "\n".join(header)
    assert "#   tag:     v9.3.0" in text
    assert "#   commit:  c033a98afddd65aaafeebea625382a94020fe4a7" in text
    assert re.search(r"#   sha256:  [0-9a-f]{64}", text)
    assert "MIT licence" in text
    assert "Copyright (c) 2025 LVGL Kft" in text
    assert "The above copyright notice and this permission notice shall be included" in text


def test_body_is_the_upstream_file_unmodified():
    header, body = _split()
    recorded = re.search(r"#   sha256:  ([0-9a-f]{64})", "\n".join(header)).group(1)
    actual = hashlib.sha256("\n".join(body).encode("utf-8")).hexdigest()
    assert actual == recorded, "LVGLImage.py differs from the upstream file its header names"


def test_shebang_stays_first():
    assert TOOL.read_text(encoding="utf-8").startswith("#!/usr/bin/env python3\n")
