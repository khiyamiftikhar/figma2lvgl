"""Where LVGL draws the widgets of the coordinate fixtures.

Every button of the two Figma exports must be drawn at Figma's own position
(tests/fixtures/coords/figma_positions*.json) and must not be clipped away.
Runs on the run-time harness (doc 12); before v0.6.0 this test built its
own program (tests/compile/test_positions.py, added for N16/N19/N20/N23).
"""

import pytest

from tests.coords_cases import FIXTURES, figma_positions
from tests.runtime.harness import run_case


@pytest.mark.parametrize("fixture", list(FIXTURES))
def test_lvgl_draws_buttons_where_figma_does(fixture, lvgl, tmp_path):
    result = run_case(fixture, lvgl, tmp_path)
    assert result.returncode == 0, result.describe()
    figma = figma_positions(fixture)
    _xml, _rep, _name, generated, not_generated = FIXTURES[fixture]
    drawn = {member.split(".")[-1]: pos for (_s, member), pos in result.positions.items()}
    for name in sorted(generated):
        x, y, visible = drawn[name]
        assert (x, y) == figma[name], f"{name}: LVGL draws it at {(x, y)}"
        assert visible, f"{name} is clipped away"
    assert not (set(drawn) & not_generated)
