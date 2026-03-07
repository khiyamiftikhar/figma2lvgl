import subprocess
import sys
import shutil
from pathlib import Path

LVGL_TOOL = Path(__file__).parent / "LVGLImage.py"
COLOR_FORMAT = "RGB565"


def main():
    if len(sys.argv) != 4:
        print("Usage: python image_converter.py <images_dir> <priv_src_dir> <priv_include_dir>")
        print("  All three arguments must be full paths.")
        print("  This script is normally called by main.py automatically.")
        sys.exit(1)

    # All three are full paths — no joining needed
    assets_dir = Path(sys.argv[1]).resolve()
    src_dir    = Path(sys.argv[2]).resolve()
    inc_dir    = Path(sys.argv[3]).resolve()

    if not assets_dir.is_dir():
        print(f"ERROR: Images directory not found: {assets_dir}")
        sys.exit(1)

    src_dir.mkdir(parents=True, exist_ok=True)
    inc_dir.mkdir(parents=True, exist_ok=True)

    # Staging dir sits next to src_dir
    staging_dir = src_dir.parent / "_lvgl_staging"
    staging_dir.mkdir(parents=True, exist_ok=True)

    image_names = []

    png_files = list(assets_dir.glob("*.png"))
    if not png_files:
        print(f"WARNING: No PNG files found in {assets_dir}")

    for png in png_files:
        print(f"Converting {png.name} ...")
        cmd = [
            sys.executable,
            str(LVGL_TOOL),
            "--ofmt", "C",
            "--cf", COLOR_FORMAT,
            "-o", str(staging_dir),
            str(png)
        ]
        subprocess.run(cmd, check=True)
        image_names.append(png.stem)

    # Move .c to priv_src, .h to priv_include
    for file in staging_dir.iterdir():
        if file.suffix == ".c":
            shutil.move(str(file), str(src_dir / file.name))
        elif file.suffix == ".h":
            shutil.move(str(file), str(inc_dir / file.name))

    staging_dir.rmdir()

    generate_assets_header(image_names, inc_dir)
    print(f"\nDone.\n  Sources -> {src_dir}\n  Headers -> {inc_dir}")


def generate_assets_header(names, inc_dir):
    header = inc_dir / "assets.h"
    with open(header, "w") as f:
        f.write("#pragma once\n\n")
        f.write('#include "lvgl.h"\n\n')
        for name in names:
            f.write(f"LV_IMG_DECLARE({name});\n")
    print(f"Generated assets.h with {len(names)} image(s)")


if __name__ == "__main__":
    main()

r"""
---

### What was wrong

**The hardcoded `ASSETS_DIR`** — the old script completely ignored `sys.argv[1]` for finding PNGs and always looked in `ui_component/assets/images` relative to itself. So it either found nothing (empty `assets.h`) or looked in the wrong place entirely.

**The argument mismatch** — `main.py` passes 3 full paths:
```
images_dir   -> e:\project\assets\images      (where PNGs are)
priv_src     -> e:\project\ui_src\priv_src    (full path)
priv_include -> e:\project\ui_src\priv_include (full path)
"""