"""Where LVGL actually draws the generated widgets (coordinate fixtures).

Builds each coordinate fixture's generated code with LVGL 9.3 (gcc only),
runs it on a 320x480 display without a screen, and reads every widget's
screen position and visibility from LVGL itself. Each button must be at
Figma's position (tests/fixtures/coords/figma_positions*.json) and must not
be clipped away.

This is how N16, N19, N20 and N23 were found: under 0.4.6, btn_a, btn_c and
btn_d of the first fixture were clipped by their group panels and not drawn,
and btn_b was 15 px off because of the theme's panel padding and border.

A first, narrow version of the run-time harness planned for v0.6.0 (L9).
Needs LVGL_DIR and gcc, like tests/compile/test_compile.py.
"""

import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

import pytest

from figma2lvgl.core.figma_parser import parse_screen
from tests.compile.test_compile import LV_CONF_DIR, ROOT, _compiler, _lvgl_dir
from tests.coords_cases import FIXTURES, figma_positions, screen_frame

MAIN = r"""
#include <stdio.h>
#include "lvgl.h"
#include "%(screen_c)s"
%(stubs)s
static uint8_t buf[320 * 48 * 2];
static void flush(lv_display_t *d, const lv_area_t *a, uint8_t *p)
{ (void)a; (void)p; lv_display_flush_ready(d); }
static void show(const char *name, lv_obj_t *o)
{
    lv_area_t c;
    lv_obj_get_coords(o, &c);
    printf("%%s %%d %%d %%d\n", name, (int)c.x1, (int)c.y1, lv_obj_is_visible(o) ? 1 : 0);
}
int main(void)
{
    lv_init();
    lv_display_t *d = lv_display_create(320, 480);
    lv_display_set_buffers(d, buf, NULL, sizeof buf, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(d, flush);
    %(screen)s_init();
    %(screen)s_load();
    lv_obj_update_layout(lv_screen_active());
%(probes)s
    return 0;
}
"""


@pytest.fixture(scope="module")
def lvgl_objects(tmp_path_factory):
    """LVGL compiled once for this module: list of object files."""
    lvgl = _lvgl_dir()
    if _compiler() != "gcc":
        pytest.skip("run-time position check uses gcc only")
    out = tmp_path_factory.mktemp("lvgl_objs")
    sources = sorted((lvgl / "src").rglob("*.c"))

    def build(i_src):
        i, src = i_src
        obj = out / f"{i}_{src.stem}.o"
        subprocess.run(
            ["gcc", "-c", "-O0", "-w", "-DLV_CONF_INCLUDE_SIMPLE",
             f"-I{LV_CONF_DIR}", f"-I{lvgl}", str(src), "-o", str(obj)],
            check=True, capture_output=True)
        return obj

    with ThreadPoolExecutor(max_workers=os.cpu_count() or 2) as pool:
        return list(pool.map(build, enumerate(sources)))


def _run(fixture, tmp_path, lvgl_objects):
    lvgl = _lvgl_dir()
    xml = FIXTURES[fixture][0]
    out = tmp_path / "gen"
    subprocess.run([sys.executable, "-m", "figma2lvgl.main", "-x", str(xml),
                    "-d", str(out), "-y"], cwd=ROOT, check=True, capture_output=True)
    ui_src = out / "ui_src"
    screen = parse_screen(screen_frame(fixture))
    prefix = f"ui_{screen.snake}"
    header = (ui_src / "include" / f"{prefix}.h").read_text(encoding="utf-8")
    callbacks = sorted(set(re.findall(r"void (ui_\w+_on_\w+)\(lv_event_t", header)))
    stubs = "\n".join(f"void {cb}(lv_event_t *e) {{ (void)e; }}" for cb in callbacks)
    probes = "\n".join(
        f'    show("{node.id}", {path}.lv_obj);'
        for node, path, _parent in screen.all_nodes_bfs())
    main = tmp_path / "main.c"
    main.write_text(MAIN % {
        "screen_c": (ui_src / "src" / f"{prefix}.c").as_posix(),
        "stubs": stubs, "screen": prefix, "probes": probes})
    exe = tmp_path / "run"
    subprocess.run(
        ["gcc", "-O0", "-w", "-DLV_CONF_INCLUDE_SIMPLE", f"-I{LV_CONF_DIR}", f"-I{lvgl}",
         f"-I{ui_src / 'include'}", f"-I{ui_src / 'priv_include'}",
         str(main), str(ui_src / "priv_src" / "ui_style.c"),
         *map(str, lvgl_objects), "-lm", "-o", str(exe)],
        check=True, capture_output=True)
    result = subprocess.run([str(exe)], check=True, capture_output=True, text=True)
    drawn = {}
    for line in result.stdout.splitlines():
        name, x, y, visible = line.split()
        drawn[name] = (int(x), int(y), visible == "1")
    return drawn


@pytest.mark.parametrize("fixture", list(FIXTURES))
def test_lvgl_draws_buttons_where_figma_does(fixture, tmp_path, lvgl_objects):
    drawn = _run(fixture, tmp_path, lvgl_objects)
    figma = figma_positions(fixture)
    _xml, _rep, _name, generated, not_generated = FIXTURES[fixture]
    for name in sorted(generated):
        x, y, visible = drawn[name]
        assert (x, y) == figma[name], f"{name}: LVGL draws it at {(x, y)}"
        assert visible, f"{name} is clipped away"
    assert not (set(drawn) & not_generated)
