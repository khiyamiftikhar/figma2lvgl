"""Output that survives a failed run (design doc 09, steps 7-13).

Invariant: a failed run never destroys the last successfully generated
ui_src/. Failures are injected by patching the step functions; these tests
run on Ubuntu and Windows in CI.
"""

import os
import subprocess
import sys
import textwrap
from pathlib import Path

import pytest

from figma2lvgl import main as cli
from figma2lvgl.core import output

ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "tests" / "fixtures"
EXAMPLE_DESIGN = ROOT / "examples" / "espidf" / "ili9486" / "design"


# ── helpers ───────────────────────────────────────────────────────────────────

def snapshot(folder: Path) -> dict:
    """relative path -> bytes, for comparing a folder byte for byte."""
    return {
        p.relative_to(folder).as_posix(): p.read_bytes()
        for p in sorted(folder.rglob("*")) if p.is_file()
    }


def make_previous_output(dest: Path) -> dict:
    ui = dest / output.UI_SRC
    for sub, name, text in [
        ("src", "ui_old.c", "old screen"),
        ("include", "ui_old.h", "old header"),
        ("priv_src", "ui_style.c", "old style"),
        ("priv_include", "ui_defs.h", "old defs"),
    ]:
        (ui / sub).mkdir(parents=True, exist_ok=True)
        (ui / sub / name).write_text(text)
    (ui / "CMakeLists.txt").write_text("user file")
    return snapshot(ui)


def good_build(tree: output.OutputTree) -> None:
    tree.path("src/ui_new.c").write_text("new screen")
    tree.path("include/ui_new.h").write_text("new header")


def leftovers(dest: Path):
    return [n for n in (output.STAGING_NAME, output.BACKUP_NAME) if (dest / n).exists()]


def run(dest: Path, build=good_build, validate=None):
    with output.OutputLock(dest):
        output.recover(dest)
        return output.write_output(dest, build, validate)


def fail_rename_when(monkeypatch, predicate):
    real = output._rename

    def rename(src, dst):
        if predicate(Path(src).name, Path(dst).name):
            raise OSError("injected rename failure")
        real(src, dst)

    monkeypatch.setattr(output, "_rename", rename)


# ── write_output: success and generic failures ────────────────────────────────

def test_successful_run_replaces_output_and_keeps_user_files(tmp_path):
    make_previous_output(tmp_path)
    assert run(tmp_path) == []
    new = snapshot(tmp_path / output.UI_SRC)
    assert new == {
        "src/ui_new.c": b"new screen",
        "include/ui_new.h": b"new header",
        "CMakeLists.txt": b"user file",
    }
    assert leftovers(tmp_path) == []
    assert (tmp_path / output.LOCK_NAME).exists()   # the lock file stays


def test_generation_raises(tmp_path):
    before = make_previous_output(tmp_path)

    def build(tree):
        good_build(tree)
        raise RuntimeError("screen generation failed")

    with pytest.raises(RuntimeError):
        run(tmp_path, build)
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert leftovers(tmp_path) == []


def test_validation_fails(tmp_path):
    before = make_previous_output(tmp_path)

    def validate(tree):
        raise output.OutputError("validation failed")

    with pytest.raises(output.OutputError):
        run(tmp_path, validate=validate)
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert leftovers(tmp_path) == []


def test_file_not_written_through_the_tree_fails_validation(tmp_path):
    before = make_previous_output(tmp_path)

    def build(tree):
        good_build(tree)
        (tree.root / "src" / "stray.c").write_text("not recorded")

    with pytest.raises(output.OutputError, match="not written through the output tree"):
        run(tmp_path, build)
    assert snapshot(tmp_path / output.UI_SRC) == before


@pytest.mark.parametrize("rel", ["../escape.c", "/abs/escape.c", "src/../../escape.c", "C:/escape.c"])
def test_write_outside_staging_is_refused(tmp_path, rel):
    before = make_previous_output(tmp_path)

    def build(tree):
        tree.path(rel).write_text("escaped")

    with pytest.raises(output.OutputError, match="outside the output folder"):
        run(tmp_path, build)
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert not (tmp_path / "escape.c").exists()
    assert leftovers(tmp_path) == []


@pytest.mark.skipif(os.name == "nt", reason="symlinks need extra rights on Windows")
def test_write_through_symlink_is_refused(tmp_path):
    before = make_previous_output(tmp_path)
    outside = tmp_path / "outside"
    outside.mkdir()

    def build(tree):
        os.symlink(outside, tree.root / "src" / "link")
        tree.path("src/link/escape.c").write_text("escaped")

    with pytest.raises(output.OutputError, match="symlink"):
        run(tmp_path, build)
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert list(outside.iterdir()) == []


def test_first_run_fails_creates_no_output(tmp_path):
    def build(tree):
        raise output.OutputError("failed")

    with pytest.raises(output.OutputError):
        run(tmp_path, build)
    assert not (tmp_path / output.UI_SRC).exists()
    assert leftovers(tmp_path) == []


# ── write_output: swap failures ───────────────────────────────────────────────

def test_rename_old_to_backup_fails(tmp_path, monkeypatch):
    before = make_previous_output(tmp_path)
    fail_rename_when(monkeypatch, lambda s, d: s == output.UI_SRC and d == output.BACKUP_NAME)
    with pytest.raises(output.OutputError, match="may be open in another program"):
        run(tmp_path)
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert leftovers(tmp_path) == []


def test_rename_staging_to_output_fails_restores_backup(tmp_path, monkeypatch):
    before = make_previous_output(tmp_path)
    fail_rename_when(monkeypatch, lambda s, d: s == output.STAGING_NAME)
    with pytest.raises(output.OutputError, match="previous output is unchanged"):
        run(tmp_path)
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert leftovers(tmp_path) == []


def test_restoring_backup_fails_keeps_backup_and_names_it(tmp_path, monkeypatch):
    before = make_previous_output(tmp_path)
    fail_rename_when(monkeypatch, lambda s, d: d == output.UI_SRC)
    with pytest.raises(output.OutputError) as err:
        run(tmp_path)
    assert str(tmp_path / output.BACKUP_NAME) in str(err.value)
    assert snapshot(tmp_path / output.BACKUP_NAME) == before
    # The next run puts the backup back before doing anything else.
    monkeypatch.undo()
    with output.OutputLock(tmp_path):
        messages = output.recover(tmp_path)
    assert any("Restored the previous output" in m for m in messages)
    assert snapshot(tmp_path / output.UI_SRC) == before


def test_deleting_backup_fails_warns_and_next_run_cleans_up(tmp_path, monkeypatch):
    make_previous_output(tmp_path)
    real = output._remove_tree

    def remove(path):
        if Path(path).name == output.BACKUP_NAME:
            raise OSError("injected delete failure")
        real(path)

    monkeypatch.setattr(output, "_remove_tree", remove)
    warnings = run(tmp_path)
    assert len(warnings) == 1 and "next run will remove the backup" in warnings[0]
    assert snapshot(tmp_path / output.UI_SRC)["src/ui_new.c"] == b"new screen"
    assert (tmp_path / output.BACKUP_NAME).exists()

    monkeypatch.undo()
    with output.OutputLock(tmp_path):
        messages = output.recover(tmp_path)
    assert any("Removed ui_src.backup" in m for m in messages)
    assert leftovers(tmp_path) == []


# ── recovery after a killed run ───────────────────────────────────────────────

def test_killed_while_generating(tmp_path):
    before = make_previous_output(tmp_path)
    (tmp_path / output.STAGING_NAME / "src").mkdir(parents=True)
    (tmp_path / output.STAGING_NAME / "src" / "half.c").write_text("partial")
    messages = output.recover(tmp_path)
    assert len(messages) == 1 and "ui_src.staging" in messages[0]
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert leftovers(tmp_path) == []


def test_killed_between_the_two_renames(tmp_path):
    before = make_previous_output(tmp_path)
    os.rename(tmp_path / output.UI_SRC, tmp_path / output.BACKUP_NAME)
    (tmp_path / output.STAGING_NAME).mkdir()
    (tmp_path / output.STAGING_NAME / "new.c").write_text("complete new output")
    messages = output.recover(tmp_path)
    assert len(messages) == 2
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert leftovers(tmp_path) == []


def test_killed_before_backup_deleted(tmp_path):
    make_previous_output(tmp_path)
    os.rename(tmp_path / output.UI_SRC, tmp_path / output.BACKUP_NAME)
    (tmp_path / output.UI_SRC).mkdir()
    (tmp_path / output.UI_SRC / "new.c").write_text("new")
    output.recover(tmp_path)
    assert snapshot(tmp_path / output.UI_SRC) == {"new.c": b"new"}
    assert leftovers(tmp_path) == []


def test_nothing_to_recover(tmp_path):
    make_previous_output(tmp_path)
    assert output.recover(tmp_path) == []


# ── lock ──────────────────────────────────────────────────────────────────────

HOLDER = textwrap.dedent("""
    import sys
    from figma2lvgl.core.output import OutputLock
    with OutputLock(sys.argv[1]):
        print("locked", flush=True)
        sys.stdin.readline()
""")


def test_second_process_is_refused_while_lock_is_held(tmp_path):
    holder = subprocess.Popen(
        [sys.executable, "-c", HOLDER, str(tmp_path)],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, cwd=ROOT,
    )
    try:
        assert holder.stdout.readline().strip() == "locked"
        with pytest.raises(output.LockBusy, match="another figma2lvgl run"):
            output.OutputLock(tmp_path).acquire()
    finally:
        holder.stdin.write("\n")
        holder.stdin.flush()
        holder.wait(timeout=10)
    # Released when the holder exits; the lock file itself stays.
    with output.OutputLock(tmp_path):
        pass
    assert (tmp_path / output.LOCK_NAME).exists()


def test_lock_released_when_holder_is_killed(tmp_path):
    holder = subprocess.Popen(
        [sys.executable, "-c", HOLDER, str(tmp_path)],
        stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, cwd=ROOT,
    )
    assert holder.stdout.readline().strip() == "locked"
    holder.kill()
    holder.wait(timeout=10)
    with output.OutputLock(tmp_path):
        pass


# ── through the CLI ───────────────────────────────────────────────────────────

def cli_run(*args):
    """Run main() in-process; return the exit code (0 when it returns)."""
    try:
        cli.main([str(a) for a in args])
    except SystemExit as e:
        return e.code or 0
    return 0


def test_cli_image_conversion_failure_keeps_previous_output(tmp_path, monkeypatch):
    assert cli_run("-x", EXAMPLE_DESIGN / "ili9486.xml", "-i", EXAMPLE_DESIGN,
                   "-d", tmp_path, "-y") == 0
    before = snapshot(tmp_path / output.UI_SRC)
    (tmp_path / output.UI_SRC / "CMakeLists.txt").write_text("user file")
    before["CMakeLists.txt"] = b"user file"

    monkeypatch.setattr(cli, "convert_images", lambda *a, **k: False)
    assert cli_run("-x", EXAMPLE_DESIGN / "ili9486.xml", "-i", EXAMPLE_DESIGN,
                   "-d", tmp_path, "-y") == 1
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert leftovers(tmp_path) == []


def test_cli_regeneration_keeps_user_files(tmp_path):
    assert cli_run("-x", FIXTURES / "probe_b1_settings.xml", "-d", tmp_path, "-y") == 0
    (tmp_path / output.UI_SRC / "CMakeLists.txt").write_text("user file")
    assert cli_run("-x", FIXTURES / "probe_b1_settings.xml", "-d", tmp_path, "-y") == 0
    assert (tmp_path / output.UI_SRC / "CMakeLists.txt").read_text() == "user file"
    assert (tmp_path / output.UI_SRC / "src" / "ui_settings.c").is_file()
    assert leftovers(tmp_path) == []


def test_cli_declined_overwrite_changes_nothing(tmp_path, monkeypatch):
    assert cli_run("-x", FIXTURES / "probe_b1_settings.xml", "-d", tmp_path, "-y") == 0
    before = snapshot(tmp_path / output.UI_SRC)
    monkeypatch.setattr("builtins.input", lambda *a: "n")
    assert cli_run("-x", FIXTURES / "probe_b1_settings.xml", "-d", tmp_path) == 0
    assert snapshot(tmp_path / output.UI_SRC) == before
    assert leftovers(tmp_path) == []


# ── --verify-compile setup (B7) ───────────────────────────────────────────────

def test_verify_compile_needs_lvgl_include(tmp_path, caplog):
    code = cli_run("-x", FIXTURES / "probe_b1_settings.xml", "-d", tmp_path, "-y",
                   "--verify-compile")
    assert code == 1
    assert "needs --lvgl-include" in caplog.text
    assert not (tmp_path / output.UI_SRC).exists()


def test_verify_compile_rejects_folder_without_lvgl_h(tmp_path, caplog):
    empty = tmp_path / "not-lvgl"
    empty.mkdir()
    code = cli_run("-x", FIXTURES / "probe_b1_settings.xml", "-d", tmp_path / "out", "-y",
                   "--verify-compile", "--lvgl-include", empty)
    assert code == 1
    assert "no lvgl.h found" in caplog.text
    assert not (tmp_path / "out" / output.UI_SRC).exists()
