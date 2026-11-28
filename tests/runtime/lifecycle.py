"""Lifecycle cases of design doc 11 (rev 6), run on generated designs.

One C program per design (and per build variant). It takes a case name and
runs that case on every screen of the design, in XML order, then prints
DONE. Each case runs in its own process, because the screen structs are
static: a case starts with every screen not built.

Output lines:

    CASE <case> <screen> OK           the case passed on that screen
    SKIP <case> <screen>: <why>       the case doesn't apply to that screen
    FAIL <screen> <case>: <why>       a check failed; the program stops
    MEASURE <screen> <what> <n>       a measured number (bytes, counts)
    DONE                              every screen done

An LVGL assert prints `ASSERT file:line` and aborts (tests/lvgl/lv_conf.h).
Functions the code under test doesn't have yet (0.6.0 has no `deinit` and
no image `hide`) are not called: the case fails with "no <function>()".

Debugging one case:

    python -m tests.runtime.lifecycle DESIGN CASE [--variant debug|length64]
"""

import argparse
import os
import re
import subprocess
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from figma2lvgl.core.widget_type import WidgetType as T
from tests.golden_cases import CASES, generate
from tests.runtime.harness import _frame_sizes, _widgets, build
from tests.runtime.lvgl_lib import ASAN_OPTIONS, SAN_FLAGS, UBSAN_OPTIONS, build_library
from figma2lvgl.core.figma_parser import parse_design

TICK_MS = 5
TIMEOUT_S = 120
MARKER_RGB565 = 0x07E0            # the replacement screen's colour, 0x00FF00

# Cases in the order of doc 11's tables. "debug" and "length64" run on their
# own build variants (-DUI_LIFECYCLE_DEBUG, -DUI_MAX_STRING_LENGTH=64).
CASE_NAMES = [
    "init_twice", "setters_before_init", "deinit_active", "deinit_inactive",
    "full_cycle", "cycles_100", "setters_after_deinit", "slider_readback",
    "bar_target", "image_matrix", "text_exact", "length64", "debug",
]
VARIANTS = {"base": [], "debug": ["-DUI_LIFECYCLE_DEBUG"],
            "length64": ["-DUI_MAX_STRING_LENGTH=64"]}
CASE_VARIANT = {"debug": "debug", "length64": "length64"}


@dataclass
class Outcome:
    design: str
    case: str
    returncode: int
    stdout: str
    stderr: str
    timed_out: bool = False
    ok: list = field(default_factory=list)          # screens
    skipped: list = field(default_factory=list)     # (screen, why)
    failure: str | None = None                      # "screen case: why"
    measures: list = field(default_factory=list)    # (screen, what, n)
    done: bool = False
    work: Path | None = None

    @property
    def clean_failure(self) -> bool:
        """Exactly one FAIL line, exit code 1, no sanitizer report, no
        assert, no timeout: the program stopped at a named check."""
        return (self.failure is not None and self.returncode == 1 and not self.timed_out
                and "ERROR: " not in self.stderr and "runtime error" not in self.stderr
                and "ASSERT " not in self.stdout)

    def describe(self) -> str:
        lines = [f"design '{self.design}', case '{self.case}': "
                 + ("timed out" if self.timed_out else f"exit code {self.returncode}")]
        if self.failure:
            lines.append("check failed: " + self.failure)
        asserts = [l for l in self.stdout.splitlines() if l.startswith("ASSERT ")]
        if asserts:
            lines.append("LVGL assert: " + asserts[0])
        report = [l for l in self.stderr.splitlines()
                  if "ERROR: " in l or "runtime error" in l or "SUMMARY" in l]
        if report:
            lines.append("sanitizer: " + " | ".join(report[:3]))
        if self.work:
            lines.append(f"program and output: {self.work}")
        return "\n".join(lines)


def _c_string(text: str) -> str:
    """A C string literal for any text: escapes, UTF-8 bytes as they are."""
    esc = {"\\": "\\\\", '"': '\\"', "\n": "\\n", "\r": "\\r", "\t": "\\t"}
    body = "".join(esc.get(ch, ch) for ch in text)
    return '"' + body.replace("??", '?""?') + '"'


def _design_text(node) -> str:
    """The text a label shows by design: Figma's, with the exporter's line
    and paragraph separators as line breaks (as the generator writes them)."""
    return (node.design_text or "").replace("\u2028", "\n").replace("\u2029", "\n")


def _has(header: str, fn: str) -> bool:
    return re.search(rf"\b{re.escape(fn)}\s*\(", header) is not None


def _test_value(node):
    """A value for a setter, different from the design's (None: no setter)."""
    wt = node.widget_type
    if wt == T.LABEL:
        return "pre-set"
    if wt == T.BUTTON:
        return "Pre"
    if wt in (T.BAR, T.SLIDER):
        a, b = ((node.bar_min, node.bar_max) if wt == T.BAR
                else (node.slider_min, node.slider_max))
        lo, hi = min(a, b), max(a, b)
        if lo == hi:
            return None
        mid = (lo + hi) // 2
        return mid if mid != lo else hi
    if wt == T.IMAGE:
        return "display"
    return None


def _screen_pieces(screen, header: str) -> dict:
    """C fragments for one screen."""
    sv = f"s_{screen.snake}"
    widgets = _widgets(screen)
    deinit = f"ui_{screen.snake}_deinit"
    ptrs = [f"{sv}.lv_screen"] + [f"{p}.lv_obj" for _n, p, _m in widgets]
    set_calls, checks, slider, bars, images, texts = [], [], [], [], [], []
    for node, path, member in widgets:
        name, sym, wt = ".".join(member), node.symbols, node.widget_type
        v = _test_value(node)
        if wt == T.LABEL:
            set_calls.append(f"{sym['set_text']}({_c_string(v)});")
            checks.append(f'check_text("{name}", lv_label_get_text({path}.lv_obj), {_c_string(v)});')
            texts.append(("label", name, path, _design_text(node), sym["set_text"]))
        elif wt == T.BUTTON:
            set_calls.append(f"{sym['set_label']}({_c_string(v)});")
            checks.append(f'check_text("{name} label", lv_label_get_text(lv_obj_get_child({path}.lv_obj, 0)), {_c_string(v)});')
            texts.append(("button", name, path, _design_text(node), sym["set_label"]))
        elif wt in (T.BAR, T.SLIDER) and v is not None:
            call = f"{sym['set_value']}({v}, 0);" if wt == T.BAR else f"{sym['set_value']}({v});"
            getter = "lv_bar_get_value" if wt == T.BAR else "lv_slider_get_value"
            set_calls.append(call)
            checks.append(f'check_int("{name} value", {getter}({path}.lv_obj), {v});')
            a, b = ((node.bar_min, node.bar_max) if wt == T.BAR
                    else (node.slider_min, node.slider_max))
            entry = (name, path, sym["set_value"], min(a, b), max(a, b),
                     1 if a > b else 0)
            (bars if wt == T.BAR else slider).append(entry)
        elif wt == T.IMAGE:
            asset = node.asset_name or node.id
            set_calls.append(f"{sym['display']}();")
            checks.append(f'check_ptr("{name} image", lv_image_get_src({path}.lv_obj), &{asset});')
            hide = sym["display"][: -len("_display")] + "_hide"
            images.append((name, path, sym["display"], hide, asset, _has(header, hide)))
    return {"sv": sv, "init": screen.symbols["init"], "load": screen.symbols["load"],
            "deinit": deinit, "has_deinit": _has(header, deinit), "ptrs": ptrs,
            "set_calls": set_calls, "checks": checks, "sliders": slider, "bars": bars,
            "images": images, "texts": texts}


def _case_functions(screen, p, size) -> str:
    """One C function per case for one screen: case_<case>_<snake>()."""
    s, w, h = screen.snake, size[0], size[1]
    ptr_n = len(p["ptrs"])
    ptr_list = ", ".join(f"(void *){x}" for x in p["ptrs"])
    snap_ptrs = f"void *ptrs[{ptr_n}] = {{ {ptr_list} }};"
    need_deinit = (f'if (!HAS_DEINIT_{s}) fail("no {p["deinit"]}()");'
                   if not p["has_deinit"] else "")
    deinit = f"{p['deinit']}();" if p["has_deinit"] else ""
    setters = "\n    ".join(p["set_calls"])
    checks = "\n    ".join(p["checks"])

    def fn(case, body):
        return (f"static void case_{case}_{s}(void)\n{{\n    begin(\"{s}\", \"{case}\", {w}, {h});\n"
                f"    {body}\n    ok();\n}}\n")

    out = [f"#define HAS_DEINIT_{s} {1 if p['has_deinit'] else 0}",
           f"static int all_null_{s}(void) {{ {snap_ptrs} for (int i = 0; i < {ptr_n}; i++) if (ptrs[i]) return 0; return 1; }}",
           f"static int any_null_{s}(void) {{ {snap_ptrs} for (int i = 0; i < {ptr_n}; i++) if (!ptrs[i]) return 1; return 0; }}",
           f"static void save_ptrs_{s}(void **out) {{ {snap_ptrs} memcpy(out, ptrs, sizeof ptrs); }}",
           f"static int same_ptrs_{s}(void **saved) {{ {snap_ptrs} return memcmp(saved, ptrs, sizeof ptrs) == 0; }}",
           f"static void deinit_{s}(void) {{ {deinit} }}"]

    out.append(fn("init_twice", f"""void *saved[{ptr_n}];
    {p['init']}();
    if (any_null_{s}()) fail("a pointer is NULL after init");
    save_ptrs_{s}(saved);
    size_t before = live();
    {p['init']}();
    if (!same_ptrs_{s}(saved)) fail("second init built a new screen (pointers changed)");
    if (live() != before) fail("second init allocated %zu bytes", live() - before);"""))

    out.append(fn("setters_before_init", (f"""{setters}
    if (!all_null_{s}()) fail("a setter created an object before init");
    {p['init']}(); {p['load']}(); cycles(5);
    set_context("value set before init");
    {checks}""") if p["set_calls"] else 'skip("no setters");'))

    out.append(fn("deinit_active", f"""{need_deinit}
    void *saved[{ptr_n}];
    {p['init']}(); {p['load']}(); cycles(5);
    save_ptrs_{s}(saved);
    size_t before = live(); int warns = warnings;
    deinit_{s}();
    if (lv_screen_active() != {p['sv']}.lv_screen) fail("deinit of the active screen changed the active screen");
    if (!same_ptrs_{s}(saved)) fail("deinit of the active screen changed pointers");
    if (live() != before) fail("deinit of the active screen freed %zu bytes", before - live());
    if (warnings - warns != 1) fail("deinit of the active screen logged %d warnings, expected 1", warnings - warns);"""))

    out.append(fn("deinit_inactive", f"""{need_deinit}
    void *screen;
    size_t base = live();
    {p['init']}(); {p['load']}(); cycles(5);
    screen = {p['sv']}.lv_screen;
    show_sentinel();
    deinit_{s}();
    if (!all_null_{s}()) fail("a pointer isn't NULL after deinit");
    if (lv_obj_is_valid(screen)) fail("the screen object still exists after deinit");
    check_sentinel();
    measure("deinit_inactive bytes left", (long)(live() - base));
    if (live() != base) fail("%ld bytes not freed by deinit", (long)(live() - base));"""))

    out.append(fn("full_cycle", (f"""{need_deinit}
    {p['init']}();
    {setters}
    {p['load']}(); cycles(5);
    show_sentinel();
    deinit_{s}();
    {p['init']}(); {p['load']}(); cycles(5);
    set_context("value kept across deinit and init");
    {checks}""") if p["set_calls"] else 'skip("no setters");'))

    out.append(fn("cycles_100", f"""{need_deinit}
    show_sentinel();
    size_t first = 0;
    for (int i = 0; i < 100; i++) {{
        {p['init']}(); deinit_{s}(); cycles(1);
        if (i == 0) first = live();
    }}
    measure("cycles_100 growth bytes", (long)(live() - first));
    if (live() != first) fail("%ld bytes more after 100 init/deinit cycles than after 1", (long)(live() - first));"""))

    out.append(fn("setters_after_deinit", (f"""{need_deinit}
    {p['init']}(); {p['load']}(); cycles(5);
    show_sentinel();
    deinit_{s}();
    size_t before = live();
    {setters}
    if (!all_null_{s}()) fail("a setter created an object after deinit");
    if (live() != before) fail("setters after deinit allocated %zu bytes", live() - before);
    {p['init']}(); {p['load']}(); cycles(5);
    set_context("value set after deinit");
    {checks}""") if p["set_calls"] else 'skip("no setters");'))

    if p["sliders"]:
        body = [need_deinit]
        for name, path, setter, lo, hi, rev in p["sliders"]:
            body.append(f"""{{ {p['init']}(); {p['load']}(); cycles(5);
      {setter}({lo});
      slider_drag({path}.lv_obj, {rev});
      int32_t b = lv_slider_get_value({path}.lv_obj);
      if (b == {lo}) fail("{name}: the drag didn't change the value");
      show_sentinel(); deinit_{s}();
      {p['init']}(); {p['load']}(); cycles(5);
      check_int("{name} value after deinit and init (dragged)", lv_slider_get_value({path}.lv_obj), b);
      show_sentinel(); deinit_{s}(); }}""")
        out.append(fn("slider_readback", "\n    ".join(body)))
    else:
        out.append(fn("slider_readback", 'skip("no sliders");'))

    if p["bars"]:
        body = [need_deinit]
        for name, path, setter, lo, hi, _rev in p["bars"]:
            body.append(f"""{{ {p['init']}(); {p['load']}(); cycles(5);
      {setter}({lo}, 0);
      {setter}({hi}, 300); cycles(30);
      int32_t mid = lv_bar_get_value({path}.lv_obj);
      if (!(mid > {lo} && mid < {hi})) fail("{name}: not mid-animation (%ld)", (long)mid);
      show_sentinel(); deinit_{s}();
      {p['init']}(); {p['load']}();
      check_int("{name} value after deinit mid-animation (target)", lv_bar_get_value({path}.lv_obj), {hi});
      show_sentinel(); deinit_{s}(); }}""")
        out.append(fn("bar_target", "\n    ".join(body)))
    else:
        out.append(fn("bar_target", 'skip("no bars");'))

    if p["images"]:
        body = [need_deinit]
        for name, path, display, hide, asset, has_hide in p["images"]:
            if not has_hide:
                body.append(f'fail("no {hide}()");')
                break
            body.append(f"""{{ {hide}(); {p['init']}(); {p['load']}();
      check_ptr("{name} after hide before init", lv_image_get_src({path}.lv_obj), NULL);
      show_sentinel(); deinit_{s}();
      {display}(); {p['init']}(); {p['load']}();
      check_ptr("{name} after display before init", lv_image_get_src({path}.lv_obj), &{asset});
      {hide}(); show_sentinel(); deinit_{s}(); {p['init']}(); {p['load']}();
      check_ptr("{name} rebuilt after hide", lv_image_get_src({path}.lv_obj), NULL);
      {display}(); show_sentinel(); deinit_{s}(); {p['init']}(); {p['load']}();
      check_ptr("{name} rebuilt after display", lv_image_get_src({path}.lv_obj), &{asset});
      show_sentinel(); deinit_{s}(); }}""")
        out.append(fn("image_matrix", "\n    ".join(body)))
    else:
        out.append(fn("image_matrix", 'skip("no images");'))

    if p["texts"]:
        body = [f"{p['init']}(); {p['load']}(); cycles(5);"]
        # Every design text first (Q17: never cut), then the setters.
        for kind, name, path, design, setter in p["texts"]:
            obj = f"{path}.lv_obj" if kind == "label" else f"lv_obj_get_child({path}.lv_obj, 0)"
            body.append(f'check_text("{name} design text after init", lv_label_get_text({obj}), {_c_string(design)});')
        for kind, name, path, design, setter in p["texts"]:
            obj = f"{path}.lv_obj" if kind == "label" else f"lv_obj_get_child({path}.lv_obj, 0)"
            field_ = f"{path}.text" if kind == "label" else f"{path}.label_text"
            body.append(f"""{{ size_t cap = buffer_size(sizeof {field_}, "{name}");
      char want[1024]; const char *in = cut_input(cap, want);
      {setter}(in);
      check_utf8("{name} after a long text", lv_label_get_text({obj}));
      check_text("{name} after a text longer than its buffer", lv_label_get_text({obj}), want);
      check_text("{name} struct text after the setter", (const char *){field_}, want); }}""")
        out.append(fn("text_exact", "\n    ".join(body)))
        first = p["texts"][0]
        field_ = f"{first[2]}.text" if first[0] == "label" else f"{first[2]}.label_text"
        out.append(fn("length64", f"""if (sizeof {field_} < 64) fail("{first[1]} buffer is %zu bytes with UI_MAX_STRING_LENGTH=64", (size_t)sizeof {field_});"""))
    else:
        out.append(fn("text_exact", 'skip("no text");'))
        out.append(fn("length64", 'skip("no text");'))

    out.append(fn("debug", (f"""{need_deinit}
    int warns = warnings;
    {p['set_calls'][0]}
    if (warnings - warns < 1) fail("a setter on a screen that isn't built logged no warning with UI_LIFECYCLE_DEBUG");""")
                  if p["set_calls"] else 'skip("no setters");'))
    return "\n".join(out)


PROGRAM = r"""/* Generated by tests/runtime/lifecycle.py for design '%(design)s'. */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
/* AddressSanitizer's live allocated bytes (its runtime defines it; declared
 * here because the sanitizer headers aren't always installed). */
size_t __sanitizer_get_current_allocated_bytes(void);
#include "lvgl.h"
#include "ui_config.h"

%(screens_c)s

%(stubs)s

static uint32_t now_ms;
static uint32_t tick_cb(void) { return now_ms; }
static void cycles(int n) { while (n--) { now_ms += %(tick)d; lv_timer_handler(); } }
static void flush_cb(lv_display_t *d, const lv_area_t *a, uint8_t *p)
{ (void)a; (void)p; lv_display_flush_ready(d); }
static size_t live(void) { return __sanitizer_get_current_allocated_bytes(); }

static int warnings;
static void log_cb(lv_log_level_t level, const char *buf)
{ if (level >= LV_LOG_LEVEL_WARN) { warnings++; printf("LOG %%s", buf); } }

static const char *cur_screen = "-", *cur_case = "-", *context = "";
static void fail(const char *fmt, ...)
{
    va_list ap; va_start(ap, fmt);
    printf("FAIL %%s %%s: ", cur_screen, cur_case); vprintf(fmt, ap);
    if (*context) printf(" (%%s)", context);
    printf("\n");
    va_end(ap); fflush(stdout);
    _exit(1);
}
static void set_context(const char *c) { context = c; }
static int skipped_flag;
static void skip(const char *why) { skipped_flag = 1; printf("SKIP %%s %%s: %%s\n", cur_case, cur_screen, why); }
static void measure(const char *what, long n) { printf("MEASURE %%s %%s %%ld\n", cur_screen, what, n); }
static const char *shown(const char *s, char *buf)
{
    if (!s) return "(null)";
    char *o = buf;
    for (int i = 0; s[i] && i < 60; i++) {
        unsigned char ch = (unsigned char)s[i];
        if (ch < 0x20 || ch > 0x7e) o += sprintf(o, "\\x%%02x", ch); else *o++ = (char)ch;
    }
    *o = 0;
    return buf;
}
static void check_text(const char *what, const char *got, const char *want)
{
    char a[400], b[400];
    if (!got || strcmp(got, want) != 0)
        fail("%%s is \"%%s\" (%%zu bytes), expected \"%%s\" (%%zu bytes)", what,
             shown(got, a), got ? strlen(got) : (size_t)0, shown(want, b), strlen(want));
}
static void check_int(const char *what, int32_t got, int32_t want)
{ if (got != want) fail("%%s is %%ld, expected %%ld", what, (long)got, (long)want); }
static void check_ptr(const char *what, const void *got, const void *want)
{ if (got != want) fail("%%s is %%p, expected %%p", what, got, want); }
static void check_utf8(const char *what, const char *s)
{
    const unsigned char *p = (const unsigned char *)s;
    while (*p) {
        int n = *p < 0x80 ? 1 : (*p >> 5) == 6 ? 2 : (*p >> 4) == 14 ? 3 : (*p >> 3) == 30 ? 4 : 0;
        if (!n) fail("%%s: invalid UTF-8 lead byte 0x%%02x", what, *p);
        for (int i = 1; i < n; i++) if ((p[i] >> 6) != 2) fail("%%s: cut UTF-8 character", what);
        p += n;
    }
}
/* The size of a text field: a char array (doc 11 rule 3), not a pointer. */
static size_t buffer_size(size_t size, const char *name)
{
    if (size == sizeof(char *)) fail("%%s: the text field is a pointer, not a buffer", name);
    return size;
}
/* A text one byte too long for a buffer of cap bytes whose last character
 * is two bytes wide: the setter must drop that whole character. */
static const char *cut_input(size_t cap, char *want)
{
    static char in[1024];
    memset(in, 'a', cap - 2); memcpy(in + cap - 2, "\xc3\xa9", 2); strcpy(in + cap, "zz");
    memcpy(want, in, cap - 2); want[cap - 2] = 0;
    return in;
}

/* L11 pointer, for dragging sliders. */
static lv_point_t ptr_pt;
static int ptr_down;
static void ptr_read(lv_indev_t *i, lv_indev_data_t *d)
{ (void)i; d->point = ptr_pt; d->state = ptr_down ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED; }
static void slider_drag(lv_obj_t *s, int reversed)
{
    lv_area_t a; lv_obj_get_coords(s, &a);
    int32_t w = lv_area_get_width(&a), h = lv_area_get_height(&a);
    lv_point_t p0, p1;
    if (w >= h) { p0.y = p1.y = (a.y1 + a.y2) / 2;
        p0.x = reversed ? a.x2 - w / 10 : a.x1 + w / 10; p1.x = reversed ? a.x1 + w / 10 : a.x2 - w / 10; }
    else { p0.x = p1.x = (a.x1 + a.x2) / 2;
        p0.y = reversed ? a.y1 + h / 10 : a.y2 - h / 10; p1.y = reversed ? a.y2 - h / 10 : a.y1 + h / 10; }
    ptr_pt = p0; ptr_down = 1; cycles(5);
    for (int i = 1; i <= 10; i++) {
        ptr_pt.x = (int32_t)(p0.x + (p1.x - p0.x) * i / 10);
        ptr_pt.y = (int32_t)(p0.y + (p1.y - p0.y) * i / 10);
        cycles(5);
    }
    ptr_down = 0; cycles(20);
}

static lv_display_t *disp;
static lv_obj_t *sentinel;
static uint16_t framebuffer[%(max_pixels)d];
static int disp_w, disp_h;
static void begin(const char *screen, const char *c, int w, int h)
{
    cur_screen = screen; cur_case = c; context = ""; skipped_flag = 0;
    disp_w = w; disp_h = h;
    lv_display_set_resolution(disp, w, h);
    lv_display_set_buffers(disp, framebuffer, NULL, sizeof framebuffer, LV_DISPLAY_RENDER_MODE_FULL);
    lv_screen_load(sentinel); cycles(2);
}
static void ok(void) { if (!skipped_flag) printf("CASE %%s %%s OK\n", cur_case, cur_screen); fflush(stdout); }
/* The harness's own replacement screen (doc 11 rev 5): loaded before every
 * deinit that must succeed; must stay active and undamaged. */
static void show_sentinel(void) { lv_screen_load(sentinel); cycles(2); }
static void check_sentinel(void)
{
    if (lv_screen_active() != sentinel) fail("the replacement screen isn't active after deinit");
    lv_obj_invalidate(sentinel); lv_refr_now(disp);
    for (int i = 0; i < disp_w * disp_h; i++)
        if (framebuffer[i] != %(marker)d) fail("the replacement screen's pixel %%d is 0x%%04x", i, framebuffer[i]);
}

%(case_functions)s

int main(int argc, char **argv)
{
    if (argc < 2) { printf("usage: %%s CASE\n", argv[0]); return 2; }
    printf("start\n"); fflush(stdout);   /* stdio's buffers exist before any measurement */
    lv_init();
    lv_tick_set_cb(tick_cb);
    lv_log_register_print_cb(log_cb);
    disp = lv_display_create(%(w0)d, %(h0)d);
    lv_display_set_buffers(disp, framebuffer, NULL, sizeof framebuffer, LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(disp, flush_cb);
    lv_indev_t *pointer = lv_indev_create();
    lv_indev_set_type(pointer, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(pointer, ptr_read);
    sentinel = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(sentinel, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_bg_opa(sentinel, LV_OPA_COVER, 0);
    lv_screen_load(sentinel); cycles(5);
%(dispatch)s
    printf("FAIL - -: unknown case %%s\n", argv[1]);
    return 2;
}
"""


def write_program(design: str, ui_src: Path, xml: Path, out: Path) -> Path:
    frames, sizes = _frame_sizes(xml)
    import logging
    parser_log = logging.getLogger("figma2lvgl.core.figma_parser")
    level = parser_log.level
    parser_log.setLevel(logging.ERROR)
    try:
        screens = parse_design(frames)
    finally:
        parser_log.setLevel(level)
    screens_c, stubs, funcs = [], [], []
    for i, s in enumerate(screens):
        c_file = (ui_src / "src" / f"ui_{s.snake}.c").as_posix()
        screens_c += [f"#define bar_anim_exec_cb bar_anim_exec_cb_{i}",
                      f'#include "{c_file}"', "#undef bar_anim_exec_cb"]
        header = (ui_src / "include" / f"ui_{s.snake}.h").read_text(encoding="utf-8")
        for cb in sorted(set(re.findall(r"void (ui_\w+_on_\w+)\(lv_event_t", header))):
            stubs.append(f"void {cb}(lv_event_t *e) {{ (void)e; }}")
        funcs.append(_case_functions(s, _screen_pieces(s, header), sizes[i]))
    dispatch = []
    for case in CASE_NAMES:
        calls = " ".join(f"case_{case}_{s.snake}();" for s in screens)
        dispatch.append(f'    if (!strcmp(argv[1], "{case}")) {{ {calls} printf("DONE\\n"); return 0; }}')
    program = out / "lifecycle.c"
    program.write_text(PROGRAM % {
        "design": design, "screens_c": "\n".join(screens_c), "stubs": "\n".join(stubs),
        "tick": TICK_MS, "max_pixels": max(w * h for w, h in sizes),
        "marker": MARKER_RGB565, "case_functions": "\n".join(funcs),
        "w0": sizes[0][0], "h0": sizes[0][1], "dispatch": "\n".join(dispatch),
    }, encoding="utf-8")
    return program


def build_design(design: str, lvgl: Path, work: Path, variant: str = "base", patch=None) -> Path:
    """Generate the design and build its lifecycle program; returns the executable."""
    xml, _images = CASES[design]
    work.mkdir(parents=True, exist_ok=True)
    ui_src = generate(design, work / "gen")
    if patch is not None:
        patch(ui_src)
    program = write_program(design, ui_src, xml, work)
    return build(program, ui_src, lvgl, build_library(lvgl), SAN_FLAGS + VARIANTS[variant])


def run(design: str, case: str, exe: Path) -> Outcome:
    env = dict(os.environ, ASAN_OPTIONS=ASAN_OPTIONS, UBSAN_OPTIONS=UBSAN_OPTIONS)
    try:
        proc = subprocess.run([str(exe), case], capture_output=True, text=True, env=env,
                              timeout=TIMEOUT_S)
        out = Outcome(design, case, proc.returncode, proc.stdout, proc.stderr)
    except subprocess.TimeoutExpired as e:
        out = Outcome(design, case, -1, (e.stdout or b"").decode(errors="replace")
                      if isinstance(e.stdout, bytes) else (e.stdout or ""),
                      (e.stderr or b"").decode(errors="replace")
                      if isinstance(e.stderr, bytes) else (e.stderr or ""), timed_out=True)
    out.work = exe.parent
    (exe.parent / f"output_{case}.txt").write_text(
        out.stdout + "\n--- stderr ---\n" + out.stderr, encoding="utf-8")
    for line in out.stdout.splitlines():
        if line.startswith("CASE ") and line.endswith(" OK"):
            out.ok.append(line.split()[2])
        elif line.startswith("SKIP "):
            head, why = line[5:].split(": ", 1)
            out.skipped.append((head.split()[1], why))
        elif line.startswith("FAIL ") and out.failure is None:
            out.failure = line[5:]
        elif line.startswith("MEASURE "):
            parts = line.split()
            out.measures.append((parts[1], " ".join(parts[2:-1]), int(parts[-1])))
        elif line == "DONE":
            out.done = True
    return out


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("design", choices=sorted(CASES))
    parser.add_argument("case", choices=CASE_NAMES)
    args = parser.parse_args(argv)
    lvgl = Path(os.environ["LVGL_DIR"])
    work = Path(tempfile.mkdtemp(prefix=f"lifecycle_{args.design}_"))
    exe = build_design(args.design, lvgl, work, CASE_VARIANT.get(args.case, "base"))
    out = run(args.design, args.case, exe)
    print(out.stdout)
    print("OK" if out.done else out.describe())
    print(f"work folder: {work}")
    return 0 if out.done else 1


if __name__ == "__main__":
    sys.exit(main())
