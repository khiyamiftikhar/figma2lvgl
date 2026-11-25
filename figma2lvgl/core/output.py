"""Writing ui_src/ so that a failed run never destroys the last good output.

Design: figma2lvgl-design, docs/design/09_cli_execution_flow.md, steps 7-13.

    with OutputLock(dest):              # step 7: one run per destination
        for message in recover(dest):   # step 8: leftovers from a killed run
            ...
        write_output(dest, build, validate)   # steps 10-13

Layout next to ui_src/ in <dest>:
    ui_src.lock       lock file, never deleted
    ui_src.staging/   the new output while it is generated and validated
    ui_src.backup/    the previous output during the swap

The four generated subfolders belong to figma2lvgl. Every other top-level
entry in ui_src/ is the user's and is copied into each new output.
"""

import logging
import os
import shutil
from pathlib import Path, PurePosixPath
from typing import Callable, Iterable, List, Optional, Set

logger = logging.getLogger(__name__)

UI_SRC = "ui_src"
LOCK_NAME = "ui_src.lock"
STAGING_NAME = "ui_src.staging"
BACKUP_NAME = "ui_src.backup"
MANAGED = ("src", "include", "priv_src", "priv_include")


class OutputError(Exception):
    """The output could not be written. The previous ui_src/ is intact
    unless the message says otherwise."""


class LockBusy(OutputError):
    """Another figma2lvgl run holds the lock for this destination."""


# Filesystem operations the swap uses, as module functions so tests can
# make them fail at a chosen point.
def _rename(src: Path, dst: Path) -> None:
    os.rename(src, dst)


def _remove_tree(path: Path) -> None:
    shutil.rmtree(path)


# ── Step 7: lock ──────────────────────────────────────────────────────────────

class OutputLock:
    """Exclusive, non-blocking OS lock on <dest>/ui_src.lock.

    The OS releases it when the process exits, so a killed run leaves no
    stale lock. The lock file is never deleted: deleting and recreating it
    would let two processes lock two different files. Protects separate
    processes, not threads within one process.
    """

    def __init__(self, dest: Path):
        self.dest = Path(dest)
        self.path = self.dest / LOCK_NAME
        self._file = None

    def acquire(self) -> None:
        self.dest.mkdir(parents=True, exist_ok=True)
        f = open(self.path, "a+b")
        try:
            if os.name == "nt":
                import msvcrt
                f.seek(0)
                msvcrt.locking(f.fileno(), msvcrt.LK_NBLCK, 1)
            else:
                import fcntl
                fcntl.flock(f.fileno(), fcntl.LOCK_EX | fcntl.LOCK_NB)
        except OSError:
            f.close()
            raise LockBusy(f"another figma2lvgl run is using {self.dest}")
        self._file = f

    def release(self) -> None:
        if self._file is None:
            return
        try:
            if os.name == "nt":
                import msvcrt
                self._file.seek(0)
                msvcrt.locking(self._file.fileno(), msvcrt.LK_UNLCK, 1)
            else:
                import fcntl
                fcntl.flock(self._file.fileno(), fcntl.LOCK_UN)
        finally:
            self._file.close()
            self._file = None

    def __enter__(self):
        self.acquire()
        return self

    def __exit__(self, *exc):
        self.release()
        return False


# ── Step 8: recovery ──────────────────────────────────────────────────────────

def recover(dest: Path) -> List[str]:
    """Handle leftovers from a killed run. Call with the lock held.

    Returns one message per action taken. The backup is handled first, so
    the last good output is back in place before anything else happens. A
    backup is never deleted while ui_src/ is missing.
    """
    dest = Path(dest)
    ui, staging, backup = dest / UI_SRC, dest / STAGING_NAME, dest / BACKUP_NAME
    messages = []

    if backup.exists():
        if not ui.exists():
            _rename(backup, ui)
            messages.append(
                f"Restored the previous output from {backup.name}/ "
                "(an earlier run was interrupted during the swap).")
        else:
            _remove_tree(backup)
            messages.append(
                f"Removed {backup.name}/ left by an earlier run "
                "(its new output was already in place).")

    if staging.exists():
        _remove_tree(staging)
        messages.append(
            f"Removed {staging.name}/ left by an interrupted run "
            "(the output was not changed by it).")

    return messages


# ── Step 10: generating into staging ──────────────────────────────────────────

class OutputTree:
    """The staging folder. Every file figma2lvgl writes is requested through
    path(), which records it and refuses paths that would land outside."""

    def __init__(self, root: Path):
        self.root = Path(root)
        self.recorded: Set[str] = set()
        self.carried: Set[str] = set()

    def _checked(self, rel: str) -> Path:
        pure = PurePosixPath(rel.replace("\\", "/"))
        if pure.is_absolute() or rel.startswith(("/", "\\")) or (len(rel) > 1 and rel[1] == ":"):
            raise OutputError(f"refusing to write outside the output folder: {rel!r} is absolute")
        if any(part in ("..", "") for part in pure.parts):
            raise OutputError(f"refusing to write outside the output folder: {rel!r}")
        target = self.root.joinpath(*pure.parts)
        # No symlink anywhere between the root and the target.
        probe = self.root
        for part in pure.parts:
            probe = probe / part
            if probe.is_symlink():
                raise OutputError(f"refusing to write through a symlink: {probe}")
        root_real = os.path.realpath(self.root)
        if os.path.commonpath([root_real, os.path.realpath(target)]) != root_real:
            raise OutputError(f"refusing to write outside the output folder: {rel!r}")
        return target

    def path(self, rel: str) -> Path:
        """Absolute path for a file to write, inside staging. Recorded."""
        target = self._checked(rel)
        target.parent.mkdir(parents=True, exist_ok=True)
        self.recorded.add(PurePosixPath(*PurePosixPath(rel.replace("\\", "/")).parts).as_posix())
        return target

    def dir(self, rel: str) -> Path:
        """Absolute path of a folder inside staging, created if needed."""
        target = self._checked(rel)
        target.mkdir(parents=True, exist_ok=True)
        return target

    def record_tree(self, rel: str) -> None:
        """Record every file under a staging folder, for files written by a
        separate process (LVGLImage.py)."""
        base = self._checked(rel)
        for path in self._walk(base):
            self.recorded.add(path.relative_to(self.root).as_posix())

    def carry_over(self, previous: Path) -> None:
        """Copy the user's own entries (anything but the four generated
        subfolders) from the previous ui_src/ into staging."""
        previous = Path(previous)
        if not previous.is_dir():
            return
        for entry in sorted(previous.iterdir()):
            if entry.name in MANAGED:
                continue
            target = self._checked(entry.name)
            if entry.is_symlink():
                os.symlink(os.readlink(entry), target)
                self.carried.add(entry.name)
            elif entry.is_dir():
                shutil.copytree(entry, target, symlinks=True)
                self.carried.add(entry.name)
            else:
                shutil.copy2(entry, target)
                self.carried.add(entry.name)

    def _walk(self, base: Path) -> Iterable[Path]:
        for dirpath, dirnames, filenames in os.walk(base, followlinks=False):
            for name in dirnames + filenames:
                p = Path(dirpath) / name
                if p.is_symlink():
                    if self._is_carried(p):
                        continue
                    raise OutputError(f"unexpected symlink in the new output: {p}")
            for name in filenames:
                yield Path(dirpath) / name

    def _is_carried(self, path: Path) -> bool:
        top = path.relative_to(self.root).parts[0]
        return top in self.carried

    def check_contents(self) -> None:
        """Step 11, generic part: every recorded file exists and isn't empty,
        and staging holds nothing else besides carried-over entries."""
        problems = []
        for rel in sorted(self.recorded):
            p = self.root / rel
            if not p.is_file():
                problems.append(f"missing: {rel}")
            elif p.stat().st_size == 0:
                problems.append(f"empty: {rel}")
        for p in self._walk(self.root):
            rel = p.relative_to(self.root).as_posix()
            if rel in self.recorded or self._is_carried(p):
                continue
            problems.append(f"not written through the output tree: {rel}")
        if problems:
            raise OutputError("the new output is incomplete:\n  " + "\n  ".join(problems))

    def require(self, rels: Iterable[str]) -> None:
        """Step 11: these files must have been written."""
        missing = [r for r in rels if r not in self.recorded]
        if missing:
            raise OutputError("the new output is missing:\n  " + "\n  ".join(missing))


# ── Steps 10-13: the transaction ──────────────────────────────────────────────

def write_output(
    dest: Path,
    build: Callable[[OutputTree], None],
    validate: Optional[Callable[[OutputTree], None]] = None,
) -> List[str]:
    """Generate into staging, validate, swap it in. Call with the lock held
    and after recover(). Returns warnings (a backup that couldn't be deleted).

    Raises OutputError (or the build's own exception) on failure. Unless
    the message says otherwise, ui_src/ is then exactly as before.
    """
    dest = Path(dest)
    ui, staging, backup = dest / UI_SRC, dest / STAGING_NAME, dest / BACKUP_NAME
    if staging.exists() or backup.exists():
        raise OutputError(f"{staging.name}/ or {backup.name}/ exists: run recover() first")

    staging.mkdir(parents=True)
    tree = OutputTree(staging)
    try:
        for name in MANAGED:
            tree.dir(name)
        build(tree)
        tree.carry_over(ui)
        tree.check_contents()
        if validate is not None:
            validate(tree)
    except BaseException:
        shutil.rmtree(staging, ignore_errors=True)
        raise

    # Step 12: swap.
    had_previous = ui.exists()
    if had_previous:
        try:
            _rename(ui, backup)
        except OSError as e:
            shutil.rmtree(staging, ignore_errors=True)
            raise OutputError(
                f"could not rename {ui} to {backup.name}/ ({e}). A file in it may be "
                "open in another program. The previous output is unchanged.") from e
    try:
        _rename(staging, ui)
    except OSError as e:
        if had_previous:
            try:
                _rename(backup, ui)
            except OSError as e2:
                raise OutputError(
                    f"could not install the new output ({e}) and could not restore the "
                    f"previous one ({e2}). The previous output is kept in {backup}: "
                    f"rename it to {UI_SRC}. The new output is in {staging}.") from e2
        shutil.rmtree(staging, ignore_errors=True)
        raise OutputError(
            f"could not rename {staging.name}/ to {UI_SRC}/ ({e}). "
            "The previous output is unchanged.") from e

    # Step 13: clean up.
    warnings = []
    if had_previous:
        try:
            _remove_tree(backup)
        except OSError as e:
            warnings.append(
                f"could not delete {backup} ({e}). The new output is in place; "
                "the next run will remove the backup.")
    return warnings
