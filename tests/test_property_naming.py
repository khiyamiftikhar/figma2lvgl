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
