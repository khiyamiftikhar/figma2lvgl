"""The coordinate fixtures (tests/fixtures/coords/) and what they must give.

Shared by tests/test_coords.py (parser only) and
tests/compile/test_positions.py (real LVGL). See the fixtures' README.
"""

import json
import xml.etree.ElementTree as ET
from pathlib import Path

COORDS = Path(__file__).resolve().parent / "fixtures" / "coords"

# fixture -> (xml, Figma's position report, screen frame name,
#             buttons that must be generated, buttons that must not be)
FIXTURES = {
    "coords": (
        COORDS / "coords.xml", COORDS / "figma_positions.json", "coords_test",
        {"btn_a", "btn_b", "btn_c", "btn_d", "btn_ghost"},
        {"btn_e", "btn_hidden"},          # hidden, or inside a hidden frame
    ),
    "coords2": (
        COORDS / "coords2.xml", COORDS / "figma_positions2.json", "coords_test_2",
        {"btn_g2", "btn_f2", "btn_mix", "btn_al1", "btn_al2", "btn_al3",
         "btn_card", "btn_pg1", "btn_pg2", "btn_overflow", "btn_rot"},
        {"btn_chip"},                     # inside an instance (rule R9)
    ),
}


def screen_frame(fixture: str):
    xml, _report, name, _gen, _not = FIXTURES[fixture]
    root = ET.parse(xml).getroot()
    return next(f for f in root.find("children")
                if f.tag == "Frame" and f.attrib.get("name") == name)


def figma_positions(fixture: str) -> dict[str, tuple[float, float]]:
    """Button name -> Figma's own screen position (plugin report)."""
    _xml, report, _name, _gen, _not = FIXTURES[fixture]
    nodes = json.loads(report.read_text(encoding="utf-8"))["nodes"]
    return {n["name"]: (n["screen_x"], n["screen_y"])
            for n in nodes if n["name"].startswith("btn_")}
