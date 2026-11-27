"""LVGL built once with the sanitizers, as a cached static library.

Design doc 12, section 2. The cache is only a speed-up: the key covers
everything that changes the library (LVGL source, lv_conf.h, gcc version,
flags), and FIGMA2LVGL_RUNTIME_NO_CACHE=1 (the nightly run) rebuilds it.
"""

import hashlib
import os
import shutil
import subprocess
import tempfile
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
LV_CONF_DIR = ROOT / "tests" / "lvgl"

# _FORTIFY_SOURCE off: Ubuntu's gcc enables it, and its checks abort before
# AddressSanitizer can report (with a stack trace) the overflow they found.
SAN_FLAGS = ["-O1", "-g", "-fno-omit-frame-pointer", "-U_FORTIFY_SOURCE",
             "-fsanitize=address,undefined", "-fno-sanitize-recover=all"]
DEFINES = ["-DLV_CONF_INCLUDE_SIMPLE", "-DLV_LVGL_H_INCLUDE_SIMPLE"]

ASAN_OPTIONS = "detect_leaks=1:abort_on_error=1"
UBSAN_OPTIONS = "print_stacktrace=1:halt_on_error=1"


def gcc_version() -> str:
    return subprocess.run(["gcc", "-dumpfullversion"], capture_output=True,
                          text=True, check=True).stdout.strip()


def _source_id(lvgl: Path) -> str:
    """The LVGL commit, or a hash of the source tree if it isn't a git checkout."""
    result = subprocess.run(["git", "-C", str(lvgl), "rev-parse", "HEAD"],
                            capture_output=True, text=True)
    if result.returncode == 0:
        return result.stdout.strip()
    h = hashlib.sha256()
    for p in sorted((lvgl / "src").rglob("*.[ch]")):
        h.update(p.relative_to(lvgl).as_posix().encode())
        h.update(p.read_bytes())
    return "tree-" + h.hexdigest()


def cache_key(lvgl: Path, flags=SAN_FLAGS) -> str:
    h = hashlib.sha256()
    for part in (_source_id(lvgl), (LV_CONF_DIR / "lv_conf.h").read_text("utf-8"),
                 gcc_version(), " ".join(list(flags) + DEFINES)):
        h.update(part.encode())
        h.update(b"\0")
    return h.hexdigest()[:16]


def cache_dir() -> Path:
    return Path(os.environ.get("FIGMA2LVGL_RUNTIME_CACHE",
                               Path.home() / ".cache" / "figma2lvgl" / "runtime"))


def build_library(lvgl: Path, flags=SAN_FLAGS) -> Path:
    """Path to liblvgl-<key>.a, building it if needed. Other flags than the
    sanitizer set are for measurements (tests/runtime/measure_determinism.py)."""
    lib = cache_dir() / f"liblvgl-{cache_key(lvgl, flags)}.a"
    if lib.is_file() and os.environ.get("FIGMA2LVGL_RUNTIME_NO_CACHE") != "1":
        return lib
    lib.parent.mkdir(parents=True, exist_ok=True)
    sources = sorted((lvgl / "src").rglob("*.c"))
    with tempfile.TemporaryDirectory() as tmp:
        tmp = Path(tmp)

        def compile_one(i_src):
            i, src = i_src
            obj = tmp / f"{i:04d}_{src.stem}.o"
            subprocess.run(["gcc", "-c", *flags, *DEFINES, "-w",
                            f"-I{LV_CONF_DIR}", f"-I{lvgl}", str(src), "-o", str(obj)],
                           check=True, capture_output=True)
            return obj

        with ThreadPoolExecutor(max_workers=os.cpu_count() or 2) as pool:
            objects = list(pool.map(compile_one, enumerate(sources)))
        partial = tmp / "lib.a"
        subprocess.run(["ar", "rcs", str(partial), *map(str, objects)], check=True)
        shutil.move(str(partial), str(lib))       # appears complete or not at all
    return lib
