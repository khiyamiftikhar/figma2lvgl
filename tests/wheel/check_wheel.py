"""Wheel install check (test strategy layer L4).

Installs a built wheel into a clean virtual environment from a local
wheelhouse, with no package index, then runs the installed CLI on the
ESP-IDF example's design with networking blocked. Proves that the package
carries everything it needs at runtime and downloads nothing.

    python tests/wheel/check_wheel.py dist/

Steps:
  1. Download the wheel's dependencies into a wheelhouse (needs network).
  2. Create a clean venv and install with --no-index --find-links.
  3. Run the installed CLI with sockets blocked in every Python process
     (sitecustomize.py on PYTHONPATH), inside a network namespace too when
     `unshare -rn` is available.

KNOWN_BUGS: v0.4.5's image conversion needs pypng and lz4, which the
package doesn't declare (B5). While B5 is listed, the check passes only if
the run fails with that exact cause. v0.4.6 PR 4 fixes B5 and empties it.
"""

import argparse
import os
import shutil
import subprocess
import sys
import tempfile
import venv
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DESIGN = ROOT / "examples" / "espidf" / "ili9486" / "design"

# bug -> text that must appear in the CLI output while the bug exists
KNOWN_BUGS = {
    "B5": "No module named 'png'",
}

EXPECTED_FILES = [
    "include/ui_home.h",
    "src/ui_home.c",
    "priv_include/assets.h",
    "priv_include/ui_config.h",
    "priv_include/ui_defs.h",
    "priv_include/ui_style.h",
    "priv_src/ui_style.c",
    "priv_src/icon_wifi.c",
    "priv_src/wifi_off.c",
]

SITECUSTOMIZE = '''\
import socket

def _blocked(*args, **kwargs):
    raise OSError("network blocked by figma2lvgl wheel check")

socket.socket.connect = _blocked
socket.socket.connect_ex = _blocked
socket.create_connection = _blocked
socket.getaddrinfo = _blocked
'''


def run(cmd, **kwargs):
    print("+", " ".join(str(c) for c in cmd), flush=True)
    return subprocess.run(cmd, text=True, capture_output=True, **kwargs)


def must(result, what):
    if result.returncode != 0:
        sys.exit(f"{what} failed:\n{result.stdout}\n{result.stderr}")
    return result


def venv_python(env_dir: Path) -> Path:
    if os.name == "nt":
        return env_dir / "Scripts" / "python.exe"
    return env_dir / "bin" / "python"


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("dist", type=Path, help="folder holding the built wheel")
    args = parser.parse_args()

    wheels = sorted(args.dist.glob("figma2lvgl-*.whl"))
    if len(wheels) != 1:
        sys.exit(f"expected exactly one figma2lvgl wheel in {args.dist}, found {len(wheels)}")
    wheel = wheels[0].resolve()

    work = Path(tempfile.mkdtemp(prefix="f2l-wheel-"))
    try:
        # 1. Wheelhouse: the wheel plus its dependencies, from the index.
        wheelhouse = work / "wheelhouse"
        must(run([sys.executable, "-m", "pip", "download", "--dest", wheelhouse, wheel]),
             "pip download")

        # 2. Clean venv, offline install.
        env_dir = work / "venv"
        venv.create(env_dir, with_pip=True)
        py = venv_python(env_dir)
        must(run([py, "-m", "pip", "install", "--no-index",
                  "--find-links", wheelhouse, wheel]), "offline install")

        tool = must(run([py, "-c",
                         "import figma2lvgl, pathlib;"
                         "print(pathlib.Path(figma2lvgl.__file__).parent / 'tools' / 'LVGLImage.py')"],
                        cwd=work),  # not the repo root, or the source tree is imported
                    "locate bundled LVGLImage.py").stdout.strip()
        if not tool.startswith(str(env_dir)):
            sys.exit(f"figma2lvgl was imported from {tool}, not from the venv")
        if not Path(tool).is_file():
            sys.exit(f"the wheel does not contain {tool}")

        # 3. Network blocked for every Python process the CLI starts.
        guard = work / "guard"
        guard.mkdir()
        (guard / "sitecustomize.py").write_text(SITECUSTOMIZE)
        env = dict(os.environ, PYTHONPATH=str(guard))

        probe = run([py, "-c", "import socket; socket.create_connection(('pypi.org', 443), 3)"],
                    env=env, cwd=work)
        if probe.returncode == 0 or "network blocked" not in probe.stderr:
            sys.exit(f"network guard is not active:\n{probe.stderr}")

        prefix = []
        if shutil.which("unshare") and run(["unshare", "-rn", "true"]).returncode == 0:
            prefix = ["unshare", "-rn"]
        print(f"network isolation: sitecustomize{' + unshare -rn' if prefix else ''}")

        out = work / "out"
        result = run(prefix + [py, "-m", "figma2lvgl.main",
                               "-x", DESIGN / "ili9486.xml", "-i", DESIGN,
                               "-d", out, "-y", "--lvgl-tool", tool],
                     env=env, cwd=work)
        output = result.stdout + result.stderr

        if KNOWN_BUGS:
            if result.returncode == 0:
                sys.exit(f"the CLI succeeded, but {', '.join(KNOWN_BUGS)} should still "
                         "make it fail. If PR 4 fixed it, remove it from KNOWN_BUGS.")
            missing = [bug for bug, text in KNOWN_BUGS.items() if text not in output]
            if missing:
                sys.exit(f"the CLI failed, but not because of {', '.join(missing)}:\n{output}")
            print(f"OK: fails only because of known bug(s) {', '.join(KNOWN_BUGS)}")
            return

        if result.returncode != 0:
            sys.exit(f"the installed CLI failed:\n{output}")
        absent = [f for f in EXPECTED_FILES if not (out / "ui_src" / f).is_file()]
        if absent:
            sys.exit(f"missing generated files: {absent}\n{output}")
        print(f"OK: {wheel.name} installed offline and generated {len(EXPECTED_FILES)} files")
    finally:
        shutil.rmtree(work, ignore_errors=True)


if __name__ == "__main__":
    main()
