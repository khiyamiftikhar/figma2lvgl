import subprocess
import sys
import shutil
from pathlib import Path

COLOR_FORMAT = "RGB565"


def main():
    if len(sys.argv) != 5:
        print("Usage: python image_converter.py <images_dir> <priv_src_dir> <priv_include_dir> <lvgl_tool_path>")
        print("  This script is normally called by main.py automatically.")
        sys.exit(1)

    assets_dir = Path(sys.argv[1]).resolve()
    src_dir    = Path(sys.argv[2]).resolve()
    inc_dir    = Path(sys.argv[3]).resolve()
    lvgl_tool  = Path(sys.argv[4]).resolve()

    if not assets_dir.is_dir():
        print(f"ERROR: Images directory not found: {assets_dir}")
        sys.exit(1)

    if not lvgl_tool.is_file():
        print(f"ERROR: LVGLImage.py not found at: {lvgl_tool}")
        sys.exit(1)

    src_dir.mkdir(parents=True, exist_ok=True)
    inc_dir.mkdir(parents=True, exist_ok=True)

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
            str(lvgl_tool),       # ← use passed path, no guessing
            "--ofmt", "C",
            "--cf", COLOR_FORMAT,
            "-o", str(staging_dir),
            str(png)
        ]
        subprocess.run(cmd, check=True)
        image_names.append(png.stem)

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
r'''

---

### Flow summary
```
main.py
  find_or_download_lvgl_tool()  ← asks user once, caches in platformdirs
        ↓ lvgl_tool path
  run_image_converter(..., lvgl_tool)
        ↓ passes as sys.argv[4]
  image_converter.py
        ↓ uses it directly, no searching
  LVGLImage.py
  '''