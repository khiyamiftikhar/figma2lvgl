# Releasing figma2lvgl

How a version gets from `main` to PyPI. The release plans and the reasons
behind these rules live in the design repo, `docs/planning/`.

## Version numbers

The version comes from the git tag. `setuptools-scm` reads it at build time
and writes `figma2lvgl/_version.py`, which is git-ignored. No file holds the
version, so releasing never means editing a version number. Tagging the
commit is what sets it.

- **Patch (0.4.x):** bug fixes and internal changes. For a design that worked
  on the previous release, no generated struct field, function name or
  widget type changes.
- **Minor (0.x.0):** anything that changes what a Figma node becomes, or the
  names generated for it. These changes are listed under "Breaking" in the
  changelog.

## Rules

1. **Tag on `main` only**, on a commit that is already on `main`.
2. **Tag only after the full test suite has passed on that exact commit**
   (the run triggered by the push to `main`, not only the PR's checks).
3. **Tagging is not publishing.** A release is done only when the version is
   on PyPI and its files match what CI tested.

## Checklist

| # | Stage | What to do |
|---|---|---|
| 1 | Prepare | Changelog entry under the new version with the date. Design docs updated if behaviour changed |
| 2 | Test the PR | All CI checks green on the release PR |
| 3 | Merge | Merge the PR into `main` |
| 4 | Test the commit | Wait for the full suite to pass on the resulting commit on `main` |
| 5 | Tag | `git tag -a vX.Y.Z <commit> -m "figma2lvgl X.Y.Z"` then `git push origin vX.Y.Z` |
| 6 | Gate | The release workflow confirms the commit is on `main` with a green run, then runs the full suite again |
| 7 | Build and verify | The wheel and sdist are built once, their SHA-256 recorded, and the wheel tested from a clean install |
| 8 | Publish | Those same files are uploaded to PyPI |
| 9 | Confirm | The SHA-256 on PyPI is compared with stage 7. A mismatch or a missing version fails the release |
| 10 | Record | GitHub release notes from the changelog. The release plan's Results section is filled in |

Stages 6–9 are automated by `.github/workflows/publish.yml`, which runs on a
release tag. It refuses to publish if the tagged commit isn't on `main` or
has no green test run there, calls `test.yml` for the full suite, builds once,
tests that wheel, publishes the same files and compares their SHA-256 with
PyPI. The workflow keeps the name `publish.yml` because PyPI's trusted
publisher is registered for that file name.

Stage 4 (the push run on `main`) is the "Tests" workflow run for the merge
commit. The "CI result" job is the single check to require in branch
protection.

## Where the old tags are

Of the release tags, only v0.1.2 is on `main`. The others (v0.1.0, v0.1.1,
v0.1.3, v0.2.0, v0.3.0, v0.4.0 and v0.4.2–v0.4.5; there is no v0.4.1) were
made on feature branches that were later squash-merged into `main`. For
example, v0.4.5 is on `feat/refactor-for-input`. Squashing creates new
commits, so none of these tags is an ancestor of `main`. The package code at
v0.4.5 is identical to `main` at commit e16146c.

These tags are published and stay where they are. Moving them would
rewrite history that others may have fetched.

On PyPI, 0.3.0, 0.4.0 and 0.4.2–0.4.4 were tagged but never published. The
published versions are 0.1.0–0.1.3, 0.2.0 and 0.4.5.

## Tag names

Release tags are exactly `vMAJOR.MINOR.PATCH`, for example `v0.4.6`. The
publish workflow matches only `v<digits>.<digits>.<digits>`, so tags such as
the existing `v1-espidf` and `v1.1-espidf` don't publish. Still, use a
different prefix for milestone tags (for example `example/espidf-1`), so a
tag name never looks like a release.
