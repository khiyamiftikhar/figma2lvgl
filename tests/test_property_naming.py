"""Property tests (L8): random designs always give valid, unique names.

Design doc 10, sections 2 and 8. For any design the strategies produce:
the run either succeeds with every invariant C1-C5 and C8 holding, or stops
with a NamingError naming a layer; and generating twice gives the same
output (C6). Example counts per trigger: tests/conftest.py. The same
designs are compiled in tests/compile/test_property_compile.py.
"""

import pytest

pytest.importorskip("hypothesis")

from hypothesis import given, note  # noqa: E402

from tests.naming_strategies import designs  # noqa: E402
from tests.property_support import check_invariants, run_design, to_xml  # noqa: E402


@given(designs())
def test_random_designs_keep_the_invariants(page):
    note(to_xml(page))
    run = run_design(page)
    check_invariants(run, page)


@given(designs())
def test_random_designs_are_deterministic(page):  # C6
    note(to_xml(page))
    first, second = run_design(page), run_design(page)
    assert (first.error is None) == (second.error is None)
    if first.error is None:
        assert first.files == second.files
    else:
        assert first.error.diagnostic == second.error.diagnostic


def test_text_layer_named_text_found_by_the_nightly_run():
    """Nightly 2026-11-28: a Text named "text" becomes the member label_text
    (the type prefix), which is also the button's field name. Allowed by C2
    (doc 10 rev 6): buttons hold no members, so it never meets that field."""
    import xml.etree.ElementTree as ET
    page = ET.fromstring(
        '<page name="Page_1" type="PAGE"><children><Frame name="home" type="FRAME" '
        'x="0" y="0" width="320" height="480"><children><Text name="text" type="TEXT" '
        'x="0" y="0" width="1" height="1" characters="t" fontSize="12" rotation="30" />'
        '</children><fills><fill color="#ffffff" /></fills></Frame></children></page>')
    run = run_design(page)
    check_invariants(run, page)
    assert [n.id for n in run.screens[0].children] == ["label_text"]
