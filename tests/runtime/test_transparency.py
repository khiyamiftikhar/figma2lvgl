"""Q16b: a PNG with transparency keeps it, to the exact pixel.

Design doc 11 rev 5. The test writes its own PNG, so nothing depends on
Figma: a fully transparent, a half-transparent, an opaque and a quarter-
opaque pixel. figma2lvgl's converter must pick RGB565A8 for it; LVGL 9.3
draws it on a screen of a known colour; every pixel must equal what LVGL's
own blend formula gives (computed below, with RGB565 rounding).
"""

import subprocess
from pathlib import Path

import png

from figma2lvgl.tools.image_converter import convert_images, color_format_for
from tests.runtime.lvgl_lib import DEFINES, LV_CONF_DIR, SAN_FLAGS, build_library

LVGL_TOOL = Path(__file__).resolve().parents[2] / "figma2lvgl" / "tools" / "LVGLImage.py"

PIXELS = [(255, 0, 0, 0), (0, 0, 255, 128), (0, 255, 0, 255), (255, 255, 255, 64)]
BACKGROUND = 0x204060


def rgb565(r: int, g: int, b: int) -> int:
    """LVGLImage.py and lv_color_to_u16: the top 5/6/5 bits, no rounding."""
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3)


def mix565(fg: int, bg: int, mix: int) -> int:
    """lv_color_16_16_mix (LVGL 9.3, src/misc/lv_color.c), which the
    software renderer uses for RGB565A8 images with the alpha plane as mask."""
    if mix == 255:
        return fg
    if mix == 0 or fg == bg:
        return bg
    mix = (mix + 4) >> 3
    b32 = (bg | (bg << 16)) & 0x7E0F81F
    f32 = (fg | (fg << 16)) & 0x7E0F81F
    result = ((((f32 - b32) * mix) >> 5) + b32) & 0x7E0F81F
    return ((result >> 16) | result) & 0xFFFF


PROGRAM = r"""
#include <stdio.h>
#include "lvgl.h"
LV_IMG_DECLARE(alpha_test);
static uint16_t fb[16 * 4];
static uint32_t now;
static uint32_t tick(void) { return now; }
static void flush(lv_display_t *d, const lv_area_t *a, uint8_t *p) { (void)a; (void)p; lv_display_flush_ready(d); }
int main(void)
{
    lv_init(); lv_tick_set_cb(tick);
    lv_display_t *disp = lv_display_create(16, 4);
    lv_display_set_buffers(disp, fb, NULL, sizeof fb, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, flush);
    lv_obj_t *scr = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x%06X), 0);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, 0);
    lv_obj_t *img = lv_image_create(scr);
    lv_image_set_src(img, &alpha_test);
    lv_obj_set_pos(img, 0, 0);
    lv_screen_load(scr);
    lv_refr_now(disp);
    for (int i = 0; i < %d; i++) printf("%%04x\n", fb[i]);
    return 0;
}
"""


def test_transparent_png_keeps_its_alpha(lvgl, tmp_path):
    assets = tmp_path / "assets"
    assets.mkdir()
    image = assets / "alpha_test.png"
    with open(image, "wb") as f:
        png.Writer(len(PIXELS), 1, greyscale=False, alpha=True).write(
            f, [[c for p in PIXELS for c in p]])
    assert color_format_for(image, "RGB565") == "RGB565A8"

    src, inc = tmp_path / "priv_src", tmp_path / "priv_include"
    assert convert_images(assets, src, inc, LVGL_TOOL)
    assert "LV_COLOR_FORMAT_RGB565A8" in (src / "alpha_test.c").read_text(encoding="utf-8")

    program = tmp_path / "alpha.c"
    program.write_text(PROGRAM % (BACKGROUND, len(PIXELS)), encoding="utf-8")
    exe = tmp_path / "alpha"
    subprocess.run(["gcc", *SAN_FLAGS, *DEFINES, f"-I{LV_CONF_DIR}", f"-I{lvgl}", f"-I{inc}",
                    str(program), str(src / "alpha_test.c"), str(build_library(lvgl)),
                    "-lm", "-o", str(exe)], check=True, capture_output=True, text=True)
    got = [int(line, 16) for line in
           subprocess.run([str(exe)], check=True, capture_output=True, text=True).stdout.split()]

    bg = rgb565(BACKGROUND >> 16, (BACKGROUND >> 8) & 255, BACKGROUND & 255)
    want = [mix565(rgb565(r, g, b), bg, a) for r, g, b, a in PIXELS]
    assert got == want, [f"{g:04x} vs {w:04x}" for g, w in zip(got, want)]
    # The formula's own sanity: transparent shows the background, opaque the image.
    assert want[0] == bg and want[2] == rgb565(0, 255, 0) and want[1] not in (bg, rgb565(0, 0, 255))


def test_opaque_png_stays_rgb565(tmp_path):
    image = tmp_path / "opaque.png"
    with open(image, "wb") as f:
        png.Writer(2, 1, greyscale=False, alpha=True).write(f, [[1, 2, 3, 255, 4, 5, 6, 255]])
    assert color_format_for(image, "RGB565") == "RGB565"
    assert color_format_for(image, "ARGB8888") == "ARGB8888"
