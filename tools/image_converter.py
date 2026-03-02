import subprocess
from pathlib import Path

# Folder layout relative to this script
BASE_DIR = Path(__file__).resolve().parent.parent
ASSETS_DIR = BASE_DIR / "ui_component"/"assets"/"images"
OUTPUT_DIR = BASE_DIR / "ui_component"/"assets"/ "src_generated"
LVGL_TOOL = Path(__file__).parent / "LVGLImage.py"

COLOR_FORMAT = "RGB565"   # Must match LV_COLOR_DEPTH=16

def main():
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    image_names = []

    for png in ASSETS_DIR.glob("*.png"):
        print(f"Converting {png.name}")

        cmd = [
            "python",
            str(LVGL_TOOL),
            "--ofmt", "C",
            "--cf", COLOR_FORMAT,
            "-o", str(OUTPUT_DIR),
            str(png)
        ]

        subprocess.run(cmd, check=True)

        image_names.append(png.stem)

    generate_assets_header(image_names)
    print("Done.")


def generate_assets_header(names):
    header = OUTPUT_DIR / "assets.h"

    with open(header, "w") as f:
        f.write("#pragma once\n\n")
        f.write('#include "lvgl.h"\n\n')

        for name in names:
            f.write(f"LV_IMG_DECLARE({name});\n")

    print("Generated assets.h")


if __name__ == "__main__":
    main()