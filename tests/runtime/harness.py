"""Run generated designs under LVGL with sanitizers (design doc 12).

For one design this module generates the UI with the real CLI, writes one
C program that drives every screen (doc 12, sections 3-4), builds it with
the sanitizers against the cached LVGL library, runs it and parses its
output. The C program prints one line per event:

    STEP <screen> <step>            before each step
    FAIL <screen> <step>: <why>     a failed check; the program stops
    CB <callback>                   a callback stub ran
    POS <screen> <member> <x> <y> <visible>   after load
    SETTERS <n>                     distinct setters called
    DONE                            after lv_deinit()

Debugging one design or one screen:

    python -m tests.runtime.harness CASE [--screen NAME]

with LVGL_DIR set. The C program and its output are kept in the folder it
prints.
"""

import argparse
import logging
import os
import re
import shutil
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from figma2lvgl.core.figma_parser import parse_design
from figma2lvgl.core.widget_type import WidgetType as T
from figma2lvgl.core.xml_input import read_design, screen_frames
from tests.golden_cases import CASES, generate
from tests.runtime.lvgl_lib import (ASAN_OPTIONS, DEFINES, LV_CONF_DIR, SAN_FLAGS,
                                    UBSAN_OPTIONS, build_library)

TICK_MS = 5


@dataclass
class Result:
    case: str
    returncode: int
    stdout: str
    stderr: str
    program: Path
    screens: list                       # snake names in XML order
    expected_setters: int
    steps: list = field(default_factory=list)          # (screen, step)
    failures: list = field(default_factory=list)       # "screen step: why"
    callbacks: list = field(default_factory=list)
    positions: dict = field(default_factory=dict)      # (screen, member) -> (x, y, visible)
    setters_called: int = -1
    done: bool = False
    step_callbacks: dict = field(default_factory=dict)  # (screen, step) -> [callback]
    covered: list = field(default_factory=list)         # (screen, member, by, x, y)
    not_visible: list = field(default_factory=list)     # (screen, member)
    values: dict = field(default_factory=dict)          # (screen, member) -> slider value
    drag_values: dict = field(default_factory=dict)     # (screen, step) -> values during a drag
    model: list = field(default_factory=list)           # parsed screens
    snapshots: dict = field(default_factory=dict)       # screen -> (w, h, path)

    @property
    def reached(self) -> list:
        seen = []
        for screen, _ in self.steps:
            if screen not in seen and screen != "-":
                seen.append(screen)
        return seen

    @property
    def not_reached(self) -> list:
        return [s for s in self.screens if s not in self.reached]

    def describe(self) -> str:
        """Failure summary naming the design, the screen and the step."""
        last = self.steps[-1] if self.steps else ("(none)", "(before any step)")
        lines = [f"design '{self.case}': exit code {self.returncode}",
                 f"last step: screen '{last[0]}', {last[1]}"]
        if self.failures:
            lines.append("check failed: " + self.failures[0])
        if self.not_reached:
            lines.append("screens not run: " + ", ".join(self.not_reached))
        report = [l for l in self.stderr.splitlines()
                  if "ERROR: " in l or "runtime error" in l or "SUMMARY" in l
                  or l.startswith("***")]
        if report:
            lines.append("sanitizer: " + " | ".join(report[:3]))
        lines.append(f"program and output: {self.program.parent}")
        return "\n".join(lines)


def _c_string(s: str) -> str:
    return '"' + s.replace("\\", "\\\\").replace('"', '\\"') + '"'


def _frame_sizes(xml: Path) -> list:
    root = read_design(xml)
    frames = screen_frames(root)
    return frames, [(int(float(f.attrib.get("width", 320))),
                     int(float(f.attrib.get("height", 480)))) for f in frames]


def _widgets(screen):
    """(node, C path, member path) depth-first, XML order."""
    sv = f"s_{screen.snake}"

    def walk(nodes, cpath, mpath):
        for n in nodes:
            yield n, f"{cpath}.{n.id}", mpath + (n.id,)
            yield from walk(n.children, f"{cpath}.{n.id}", mpath + (n.id,))
    return list(walk(screen.children, sv, ()))


def _widget_block(screen, node, path, member) -> tuple[list, int]:
    """C lines that call and check every setter of one widget (doc 12, 4)."""
    s, name, sym, wt = screen.snake, ".".join(member), node.symbols, node.widget_type
    lines, setters = [], 0

    def step(text):
        lines.append(f'    step("{s}", "{name} {text}");')

    if wt == T.LABEL:
        setters = 1
        for i in range(4):
            step(f"set_text #{i}")
            lines += [f"    {sym['set_text']}(texts[{i}]); setters_called_once(&called_{id(node)});",
                      f'    check_text("{name}", lv_label_get_text({path}.lv_obj), cut(texts[{i}], sizeof {path}.text));']
    elif wt == T.BUTTON:
        setters = 1
        for i in range(4):
            step(f"set_label #{i}")
            lines += [f"    {sym['set_label']}(texts[{i}]); setters_called_once(&called_{id(node)});",
                      f"    {{ lv_obj_t *l = lv_obj_get_child({path}.lv_obj, 0);",
                      f'      if (!l) fail("{name}: button has no label child");',
                      f'      check_text("{name} label", lv_label_get_text(l), cut(texts[{i}], sizeof {path}.label_text)); }}']
    elif wt in (T.BAR, T.SLIDER):
        setters = 1
        a, b = ((node.bar_min, node.bar_max) if wt == T.BAR
                else (node.slider_min, node.slider_max))
        lo, hi = min(a, b), max(a, b)
        values = [lo, hi, lo - 1, hi + 1, (lo + hi) // 2]
        getter = "lv_bar_get_value" if wt == T.BAR else "lv_slider_get_value"
        calls = ([(v, 0) for v in values] + [((lo + hi) // 2, 300), (hi, 300)]
                 if wt == T.BAR else [(v, None) for v in values])
        for v, dur in calls:
            step(f"set_value {v}" + (f" over {dur} ms" if dur is not None else ""))
            args = f"{v}, {dur}" if dur is not None else f"{v}"
            lines += [f"    {sym['set_value']}({args}); setters_called_once(&called_{id(node)});",
                      "    cycles(80);" if wt == T.BAR else "",
                      f'    check_int("{name} value", {getter}({path}.lv_obj), clamp({v}, {a}, {b}));']
    elif wt == T.IMAGE:
        setters = 1
        asset = node.asset_name or node.id
        step("display")
        lines += [f"    {{ uint32_t n = lv_obj_get_child_count(lv_obj_get_parent({path}.lv_obj));",
                  f"      {sym['display']}(); {sym['display']}(); setters_called_once(&called_{id(node)});",
                  f'      if (lv_image_get_src({path}.lv_obj) != (const void *)&{asset})',
                  f'          fail("{name}: image source is not &{asset}");',
                  f'      check_int("{name} siblings after display x2",',
                  f"                (int32_t)lv_obj_get_child_count(lv_obj_get_parent({path}.lv_obj)), (int32_t)n); }}"]
    elif wt == T.DYNAMIC:
        setters = 1
        step("get")
        lines += [f"    {{ lv_obj_t *o = {sym['get']}(); setters_called_once(&called_{id(node)});",
                  f'      if (o != {path}.lv_obj) fail("{name}: accessor returned another object");',
                  "      uint32_t n = lv_obj_get_child_count(o);",
                  "      lv_obj_t *c = lv_label_create(o);",
                  f'      check_int("{name} children after add", (int32_t)lv_obj_get_child_count(o), (int32_t)n + 1);',
                  "      lv_obj_delete(c);",
                  f'      check_int("{name} children after delete", (int32_t)lv_obj_get_child_count(o), (int32_t)n); }}']
    decl = [f"static int called_{id(node)};"] if setters else []
    return decl, [l for l in lines if l], setters


def _touch_block(screen, node, path, member) -> list:
    """C lines that press one widget (doc 12, section 6)."""
    s, name, wt = screen.snake, ".".join(member), node.widget_type
    lines = []
    if wt == T.BUTTON:
        holds = [("tap", 50)]
        if set(node.event_modifiers) & {"lp", "lpr"}:
            holds.append(("long press", 1000))
        for label, ms in holds:
            lines += [f'    step("{s}", "{name} {label}");',
                      f'    {{ lv_point_t pt; if (reachable("{s}", "{name}", {path}.lv_obj, &pt)) press_at(pt, {ms}); }}']
    elif wt == T.SLIDER:
        reversed_ = 1 if node.slider_min > node.slider_max else 0
        lines += [f'    step("{s}", "{name} drag");',
                  f'    {{ lv_point_t pt; if (reachable("{s}", "{name}", {path}.lv_obj, &pt)) {{',
                  f"        slider_drag({path}.lv_obj, {reversed_});",
                  f'        printf("VALUE {s} {name} %ld\\n", (long)lv_slider_get_value({path}.lv_obj)); }} }}']
    return lines


PROGRAM = r"""/* Generated by tests/runtime/harness.py for design '%(case)s'. */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "lvgl.h"
#include "ui_config.h"

%(screens_c)s

/* Callback stubs: each prints its name (doc 12, section 3). */
%(stubs)s

static uint32_t now_ms;
static uint32_t tick_cb(void) { return now_ms; }
static void cycles(int n) { while (n--) { now_ms += %(tick)d; lv_timer_handler(); } }
static void flush_cb(lv_display_t *d, const lv_area_t *a, uint8_t *p)
{ (void)a; (void)p; lv_display_flush_ready(d); }

static const char *cur_screen = "-", *cur_step = "start";
static void step(const char *screen, const char *what)
{ cur_screen = screen; cur_step = what; printf("STEP %%s %%s\n", screen, what); fflush(stdout); }
static void fail(const char *fmt, ...)
{
    va_list ap; va_start(ap, fmt);
    printf("FAIL %%s %%s: ", cur_screen, cur_step); vprintf(fmt, ap); printf("\n");
    va_end(ap); fflush(stdout);
    _exit(1);   /* no atexit: the live LVGL objects aren't leaks */
}
static const char *shown(const char *s, char *buf)
{   /* first 40 bytes, with control bytes escaped, for one-line messages */
    if (!s) return "(null)";
    char *o = buf;
    for (int i = 0; s[i] && i < 40; i++) {
        unsigned char ch = (unsigned char)s[i];
        if (ch < 0x20) o += sprintf(o, "\\x%%02x", ch); else *o++ = (char)ch;
    }
    *o = 0;
    return buf;
}
static void check_text(const char *what, const char *got, const char *want)
{
    char a[200], b[200];
    if (!got || strcmp(got, want) != 0)
        fail("%%s is \"%%s\" (%%zu bytes), expected \"%%s\" (%%zu bytes)", what,
             shown(got, a), got ? strlen(got) : (size_t)0, shown(want, b), strlen(want));
}
static void check_int(const char *what, int32_t got, int32_t want)
{ if (got != want) fail("%%s is %%ld, expected %%ld", what, (long)got, (long)want); }
/* What a setter stores (design doc 11 rev 5, Q17): the text cut to fit a
 * buffer of size bytes, at a whole UTF-8 character. */
static const char *cut(const char *text, size_t size)
{
    static char buf[4 * UI_MAX_STRING_LENGTH + 256];
    size_t n = strlen(text);
    if (n > size - 1) {
        n = size - 1;
        while (n > 0 && ((unsigned char)text[n] & 0xC0) == 0x80) n--;
    }
    memcpy(buf, text, n); buf[n] = 0;
    return buf;
}
static int32_t clamp(int32_t v, int32_t a, int32_t b)
{ int32_t lo = a < b ? a : b, hi = a < b ? b : a; return v < lo ? lo : v > hi ? hi : v; }
static int setters_called;
static void setters_called_once(int *flag) { if (!*flag) { *flag = 1; setters_called++; } }
static void positions(const char *screen, const char *name, lv_obj_t *o)
{
    lv_area_t c; lv_obj_get_coords(o, &c);
    printf("POS %%s %%s %%d %%d %%d\n", screen, name, (int)c.x1, (int)c.y1, lv_obj_is_visible(o) ? 1 : 0);
}

/* L11: a scripted pointer (doc 12, section 6). */
static lv_point_t ptr_pt;
static int ptr_down;
static void ptr_read(lv_indev_t *i, lv_indev_data_t *d)
{ (void)i; d->point = ptr_pt; d->state = ptr_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED; }
typedef struct { lv_obj_t *obj; const char *name; } named_t;
static named_t names[%(max_names)d];
static int n_names;
static void add_name(lv_obj_t *o, const char *n) { names[n_names].obj = o; names[n_names].name = n; n_names++; }
static const char *name_of(lv_obj_t *o)
{
    for (; o; o = lv_obj_get_parent(o))
        for (int i = n_names - 1; i >= 0; i--)
            if (names[i].obj == o) return names[i].name;
    return "(nothing)";
}
/* Test point: centre of the widget's area clipped by every ancestor that
 * clips (no LV_OBJ_FLAG_OVERFLOW_VISIBLE). */
static int test_point(lv_obj_t *w, lv_point_t *pt)
{
    lv_area_t a;
    lv_obj_get_coords(w, &a);
    for (lv_obj_t *p = lv_obj_get_parent(w); p; p = lv_obj_get_parent(p)) {
        if (lv_obj_has_flag(p, LV_OBJ_FLAG_OVERFLOW_VISIBLE)) continue;
        lv_area_t pa;   /* intersect by hand: lv_area_intersect is private in 9.3 */
        lv_obj_get_coords(p, &pa);
        if (pa.x1 > a.x1) a.x1 = pa.x1;
        if (pa.y1 > a.y1) a.y1 = pa.y1;
        if (pa.x2 < a.x2) a.x2 = pa.x2;
        if (pa.y2 < a.y2) a.y2 = pa.y2;
        if (a.x1 > a.x2 || a.y1 > a.y2) return 0;
    }
    pt->x = (a.x1 + a.x2) / 2;
    pt->y = (a.y1 + a.y2) / 2;
    return 1;
}
/* The widget receives a press at its test point if LVGL's own hit test
 * finds it or one of its descendants there. */
static int reachable(const char *screen, const char *name, lv_obj_t *w, lv_point_t *pt)
{
    if (!test_point(w, pt)) { printf("NOT_VISIBLE %%s %%s\n", screen, name); return 0; }
    lv_obj_t *f = lv_indev_search_obj(lv_screen_active(), pt);
    for (lv_obj_t *o = f; o; o = lv_obj_get_parent(o))
        if (o == w) return 1;
    printf("COVERED %%s %%s %%s %%d %%d\n", screen, name, name_of(f), (int)pt->x, (int)pt->y);
    return 0;
}
static void press_at(lv_point_t pt, int ms)
{ ptr_pt = pt; ptr_down = 1; cycles(ms / %(tick)d); ptr_down = 0; cycles(20); }
/* Drags a slider and prints its value before and after every move, so the
 * test can count the value changes (one VALUE_CHANGED callback each). */
static void drag(lv_obj_t *s, lv_point_t a, lv_point_t b)
{
    printf("DRAGV %%ld\n", (long)lv_slider_get_value(s));
    ptr_pt = a; ptr_down = 1; cycles(5);
    printf("DRAGV %%ld\n", (long)lv_slider_get_value(s));
    for (int i = 1; i <= 10; i++) {
        ptr_pt.x = (int32_t)(a.x + (b.x - a.x) * i / 10);
        ptr_pt.y = (int32_t)(a.y + (b.y - a.y) * i / 10);
        cycles(5);
        printf("DRAGV %%ld\n", (long)lv_slider_get_value(s));
    }
    ptr_down = 0; cycles(20);
    printf("DRAGV %%ld\n", (long)lv_slider_get_value(s));
}
/* From 10 %% to 90 %% along the slider, starting at its minimum end: left or
 * bottom (LVGL's AUTO orientation: horizontal when width >= height),
 * mirrored when the range is reversed. */
static void slider_drag(lv_obj_t *s, int reversed)
{
    lv_area_t a; lv_obj_get_coords(s, &a);
    int32_t w = lv_area_get_width(&a), h = lv_area_get_height(&a);
    lv_point_t p0, p1;
    if (w >= h) {
        int32_t y = (a.y1 + a.y2) / 2;
        p0.y = p1.y = y;
        p0.x = reversed ? a.x2 - w / 10 : a.x1 + w / 10;
        p1.x = reversed ? a.x1 + w / 10 : a.x2 - w / 10;
    } else {
        int32_t x = (a.x1 + a.x2) / 2;
        p0.x = p1.x = x;
        p0.y = reversed ? a.y1 + h / 10 : a.y2 - h / 10;
        p1.y = reversed ? a.y2 - h / 10 : a.y1 + h / 10;
    }
    drag(s, p0, p1);
}

/* L10: the rendered frame, as raw RGB565 (doc 12, section 5). */
static void snapshot(const char *screen, const char *path, int w, int h);

static char long_text[2 * UI_MAX_STRING_LENGTH + 1];
static const char *texts[4] = { "x", "", long_text, "Gr\xc3\xbc\xc3\x9f" "e \xe2\x9c\x93" };
%(decls)s
static uint16_t framebuffer[%(max_pixels)d];
static void snapshot(const char *screen, const char *path, int w, int h)
{
    FILE *f = fopen(path, "wb");
    if (!f) fail("can't write %%s", path);
    fwrite(framebuffer, sizeof framebuffer[0], (size_t)(w * h), f);
    fclose(f);
    printf("SNAP %%s %%d %%d %%s\n", screen, w, h, path);
}

int main(void)
{
    memset(long_text, 'a', sizeof long_text - 1);
    lv_init();
    lv_tick_set_cb(tick_cb);
    lv_display_t *disp = lv_display_create(%(w0)d, %(h0)d);
    lv_display_set_buffers(disp, framebuffer, NULL, sizeof framebuffer, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_indev_t *pointer = lv_indev_create();
    lv_indev_set_type(pointer, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(pointer, ptr_read);
%(body)s
    step("-", "lv_deinit");
    lv_deinit();
    printf("SETTERS %%d\n", setters_called);
    printf("DONE\n");
    return 0;
}
"""


def write_program(case: str, ui_src: Path, xml: Path, out: Path,
                  only_screen: str | None = None, mode: str = "run"):
    """Write main.c for a design. mode "run": L9 (setters and checks);
    "touch": L11 (scripted presses). Returns (path, screen names, setter
    count, parsed screens)."""
    frames, sizes = _frame_sizes(xml)
    parser_log = logging.getLogger("figma2lvgl.core.figma_parser")
    level = parser_log.level
    parser_log.setLevel(logging.ERROR)     # the CLI run already reported warnings
    try:
        screens = parse_design(frames)
    finally:
        parser_log.setLevel(level)
    pairs = [(s, sz) for s, sz in zip(screens, sizes)
             if only_screen is None or s.snake == only_screen]
    if only_screen is not None and not pairs:
        raise SystemExit(f"no screen '{only_screen}' in {case}: "
                         + ", ".join(s.snake for s in screens))
    screens_c, stubs, decls, body = [], [], [], []
    expected = 0
    for i, s in enumerate(screens):
        c_file = (ui_src / "src" / f"ui_{s.snake}.c").as_posix()
        screens_c += [f"#define bar_anim_exec_cb bar_anim_exec_cb_{i}",
                      f'#include "{c_file}"', "#undef bar_anim_exec_cb"]
        header = (ui_src / "include" / f"ui_{s.snake}.h").read_text(encoding="utf-8")
        for cb in sorted(set(re.findall(r"void (ui_\w+_on_\w+)\(lv_event_t", header))):
            stubs.append(f'void {cb}(lv_event_t *e) {{ (void)e; printf("CB {cb}\\n"); }}')
    for s, (w, h) in pairs:
        body += [f"    lv_display_set_resolution(disp, {w}, {h});",
                 "    lv_display_set_buffers(disp, framebuffer, NULL, sizeof framebuffer, LV_DISPLAY_RENDER_MODE_FULL);",
                 f'    step("{s.snake}", "init");', f"    {s.symbols['init']}();",
                 f'    step("{s.snake}", "load");', f"    {s.symbols['load']}();",
                 "    cycles(20);"]
        widgets = _widgets(s)
        body += [f'    positions("{s.snake}", "{".".join(m)}", {p}.lv_obj);'
                 for _n, p, m in widgets]
        if mode == "render":
            # L10 (doc 12, section 5): settle animations, draw, dump RGB565.
            snap = (out / "snapshots" / f"{s.snake}.rgb565").as_posix()
            body += [f'    step("{s.snake}", "render");', "    cycles(100);",
                     "    lv_refr_now(disp);",
                     f'    snapshot("{s.snake}", "{snap}", {w}, {h});']
        elif mode == "touch":
            body += [f'    add_name({p}.lv_obj, "{".".join(m)}");' for _n, p, m in widgets]
            for node, path, member in widgets:
                body += _touch_block(s, node, path, member)
        else:
            for node, path, member in widgets:
                d, lines, n = _widget_block(s, node, path, member)
                decls += d
                body += lines
                expected += n
        body += [f'    step("{s.snake}", "settle");', "    cycles(100);"]
    max_pixels = max(w * h for _s, (w, h) in pairs)
    program = out / "main.c"
    program.write_text(PROGRAM % {
        "case": case, "screens_c": "\n".join(screens_c), "stubs": "\n".join(stubs),
        "tick": TICK_MS, "decls": "\n".join(decls), "max_pixels": max_pixels,
        "w0": pairs[0][1][0], "h0": pairs[0][1][1], "body": "\n".join(body),
        "max_names": max(1, sum(len(_widgets(s)) for s, _ in pairs)),
    }, encoding="utf-8")
    return program, [s.snake for s, _ in pairs], expected, [s for s, _ in pairs]


def build(program: Path, ui_src: Path, lvgl: Path, lib: Path, flags=SAN_FLAGS) -> Path:
    exe = program.with_suffix("")
    sources = [program, ui_src / "priv_src" / "ui_style.c",
               *(p for p in sorted((ui_src / "priv_src").glob("*.c")) if p.name != "ui_style.c")]
    # A missing declaration is an error everywhere: gcc 14 makes it one by
    # default, and a private LVGL function must fail locally too. (No -w: it
    # would cancel this -Werror. Default warnings only print; the compile
    # tests check the generated code's warnings.)
    cmd = ["gcc", *flags, *DEFINES, "-std=gnu99",
           "-Werror=implicit-function-declaration",
           f"-I{LV_CONF_DIR}", f"-I{lvgl}", f"-I{ui_src / 'include'}",
           f"-I{ui_src / 'priv_include'}", *map(str, sources), str(lib), "-lm", "-o", str(exe)]
    (program.parent / "build_command.txt").write_text(" ".join(cmd) + "\n", encoding="utf-8")
    result = subprocess.run(cmd, capture_output=True, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"building the harness failed:\n{result.stderr[-4000:]}")
    return exe


def parse_output(result: Result) -> Result:
    for line in result.stdout.splitlines():
        if line.startswith("STEP "):
            _, screen, what = line.split(" ", 2)
            result.steps.append((screen, what))
        elif line.startswith("FAIL "):
            result.failures.append(line[5:])
        elif line.startswith("CB "):
            result.callbacks.append(line[3:])
            if result.steps:
                result.step_callbacks.setdefault(result.steps[-1], []).append(line[3:])
        elif line.startswith("COVERED "):
            _, screen, member, by, x, y = line.split()
            result.covered.append((screen, member, by, int(x), int(y)))
        elif line.startswith("NOT_VISIBLE "):
            _, screen, member = line.split()
            result.not_visible.append((screen, member))
        elif line.startswith("DRAGV ") and result.steps:
            result.drag_values.setdefault(result.steps[-1], []).append(int(line.split()[1]))
        elif line.startswith("VALUE "):
            _, screen, member, v = line.split()
            result.values[(screen, member)] = int(v)
        elif line.startswith("POS "):
            _, screen, member, x, y, vis = line.split()
            result.positions[(screen, member)] = (int(x), int(y), vis == "1")
        elif line.startswith("SETTERS "):
            result.setters_called = int(line.split()[1])
        elif line.startswith("SNAP "):
            _, screen, w, h, path = line.split(" ", 4)
            result.snapshots[screen] = (int(w), int(h), Path(path))
        elif line == "DONE":
            result.done = True
    return result


def run_case(case: str, lvgl: Path, work: Path, only_screen: str | None = None,
             patch=None, mode: str = "run", flags=SAN_FLAGS) -> Result:
    """Generate, build and run one design of tests/golden_cases.py.

    patch: optional function called with the ui_src folder before building,
    for the harness's self-tests (tests that break generated code on purpose).
    """
    xml, _images = CASES[case]
    ui_src = generate(case, work / "gen")
    if patch is not None:
        patch(ui_src)
    (work / "snapshots").mkdir(parents=True, exist_ok=True)
    program, screens, expected, model = write_program(case, ui_src, xml, work, only_screen, mode)
    exe = build(program, ui_src, lvgl, build_library(lvgl, flags), flags)
    env = dict(os.environ, ASAN_OPTIONS=ASAN_OPTIONS, UBSAN_OPTIONS=UBSAN_OPTIONS)
    proc = subprocess.run([str(exe)], capture_output=True, text=True, env=env, timeout=300)
    (work / "output.txt").write_text(proc.stdout + "\n--- stderr ---\n" + proc.stderr,
                                     encoding="utf-8")
    result = Result(case, proc.returncode, proc.stdout, proc.stderr, program, screens, expected)
    result.model = model
    return parse_output(result)


def keep_artifacts(work: Path, case: str) -> None:
    """Copy the program, build command and output for CI (doc 12, section 4)."""
    target = os.environ.get("FIGMA2LVGL_RUNTIME_ARTIFACTS")
    if not target:
        return
    dest = Path(target) / case
    dest.mkdir(parents=True, exist_ok=True)
    for name in ("main.c", "build_command.txt", "output.txt"):
        if (work / name).exists():
            shutil.copy2(work / name, dest / name)


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("case", choices=sorted(CASES))
    parser.add_argument("--screen", help="run only this screen (its snake name)")
    parser.add_argument("--touch", action="store_true", help="L11 presses instead of L9 setters")
    args = parser.parse_args(argv)
    lvgl = Path(os.environ["LVGL_DIR"])
    work = Path(tempfile.mkdtemp(prefix=f"harness_{args.case}_"))
    result = run_case(args.case, lvgl, work, args.screen,
                      mode="touch" if args.touch else "run")
    ok = result.returncode == 0 and result.done and not result.failures
    print("OK" if ok else result.describe())
    print(f"work folder: {work}")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
