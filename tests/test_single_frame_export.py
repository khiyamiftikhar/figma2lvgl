"""A frame exported on its own is one screen (N33, PR #18).

`tests/fixtures/showcase/showcase_frame.xml` is the `showcase` frame
exported alone with the FigML exporter: its root is `<frame>`, not a page.
Before 0.6.0 every child of that frame became a screen. It must generate
exactly what the page export with the same frame generates.
"""

import subprocess
import sys
import xml.etree.ElementTree as ET

from figma2lvgl.core.xml_input import screen_frames
from tests.golden_cases import CASES, FIXTURES, ROOT, read_tree


def _generate(xml, out):
    images = CASES["showcase"][1]
    subprocess.run([sys.executable, "-m", "figma2lvgl.main", "-x", str(xml), "-d", str(out),
                    "-i", str(images), "-y"], cwd=ROOT, check=True, capture_output=True)
    return read_tree(out / "ui_src")


def test_single_frame_export_generates_the_same_as_the_page(tmp_path):
    single = _generate(FIXTURES / "showcase" / "showcase_frame.xml", tmp_path / "single")
    page = _generate(CASES["showcase"][0], tmp_path / "page")
    assert [p for p in single if p.startswith("src/")] == ["src/ui_showcase.c"]
    assert single == page


def test_screen_frames():
    page = ET.fromstring('<page type="PAGE"><children><Frame name="a"/><Component name="c"/>'
                         '<Frame name="b"/></children></page>')
    assert [f.get("name") for f in screen_frames(page)] == ["a", "b"]
    for root in ('<frame name="s" type="FRAME"><children><Frame name="x"/></children></frame>',
                 '<Frame name="s"><children><Frame name="x"/></children></Frame>'):
        assert [f.get("name") for f in screen_frames(ET.fromstring(root))] == ["s"]
    assert screen_frames(ET.fromstring("<page/>")) is None
