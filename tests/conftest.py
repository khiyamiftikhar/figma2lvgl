"""Shared pytest setup: Hypothesis profiles for the naming property tests.

Select with HYPOTHESIS_PROFILE (CI sets it per trigger, design repo
docs/planning/02_test_strategy.md):

    ci       feature-branch push: 50 examples, fixed seed, no compiling
    full     PR, push to main, tag: 200 examples, fixed seed, compiled
    nightly  scheduled run: 200 examples, random seed; a failure prints the
             example and a @reproduce_failure line to replay it

Without the variable (local runs) the "ci" profile is used.
"""

import os

try:
    from hypothesis import HealthCheck, settings
except ImportError:      # hypothesis is a test dependency; tests that need it skip
    settings = None

if settings is not None:
    common = dict(
        deadline=None,                       # compiling makes timings vary
        suppress_health_check=[HealthCheck.too_slow, HealthCheck.data_too_large],
        print_blob=True,
    )
    settings.register_profile("ci", max_examples=50, derandomize=True, **common)
    settings.register_profile("full", max_examples=200, derandomize=True, **common)
    settings.register_profile("nightly", max_examples=200, derandomize=False, **common)
    settings.load_profile(os.environ.get("HYPOTHESIS_PROFILE", "ci"))


def hypothesis_profile() -> str:
    return os.environ.get("HYPOTHESIS_PROFILE", "ci")
