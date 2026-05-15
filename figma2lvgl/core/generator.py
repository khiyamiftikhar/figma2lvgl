# core/generator.py  (v0.4.0)
#
# Orchestrates the three emitters to produce .c and .h files for one screen.
# Three-pass approach:
#   1. node_emitter   → struct type definition + static initializer
#   2. init_emitter   → flat BFS _init() body
#   3. setter_emitter → setters, callbacks, prototypes

from figma2lvgl.core.figma_parser import ParsedScreen
from figma2lvgl.core.node_emitter import emit_screen_struct_type
from figma2lvgl.core.init_emitter import emit_init_body
from figma2lvgl.core.setter_emitter import collect_setters_and_callbacks


_BAR_ANIM_CB = """\
static void _bar_anim_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value((lv_obj_t *)obj, v, LV_ANIM_OFF);
}"""


def generate_screen(screen: ParsedScreen) -> tuple[str, str, str, str]:
    """
    Generate C and H source for one screen.

    Returns (c_filename, h_filename, h_text, c_text).
    """
    snake   = screen.snake
    sv      = f"s_{snake}"
    guard   = f"UI_{snake.upper()}_H"
    h_fname = f"ui_{snake}.h"
    c_fname = f"ui_{snake}.c"
    init_fn = f"ui_{snake}_init"
    load_fn = f"ui_{snake}_load"

    # ── Pass 1: struct type + initializer ────────────────────────────────────
    struct_block = emit_screen_struct_type(screen)

    # ── Pass 2: init body ─────────────────────────────────────────────────────
    init_body = emit_init_body(screen)

    # ── Pass 3: setters, callbacks, prototypes ────────────────────────────────
    sc = collect_setters_and_callbacks(screen)

    bar_anim = _BAR_ANIM_CB if sc["bar_anim_needed"] else ""

    # ── Assemble .c ───────────────────────────────────────────────────────────
    c_text = _C_LAYOUT.format(
        h_fname    = h_fname,
        struct     = struct_block,
        bar_anim   = bar_anim,
        callbacks  = sc["callbacks"],
        setters    = sc["setters"],
        load_fn    = load_fn,
        sv         = sv,
        init_fn    = init_fn,
        init_body  = init_body,
    )

    # ── Assemble .h ───────────────────────────────────────────────────────────
    h_text = _H_LAYOUT.format(
        guard       = guard,
        init_fn     = init_fn,
        load_fn     = load_fn,
        setters     = sc["prototypes"],
        cb_decls    = sc["cb_declarations"],
    )

    return c_fname, h_fname, h_text, c_text


# ── Layout templates ──────────────────────────────────────────────────────────

_C_LAYOUT = """\
#include "{h_fname}"
#include "assets.h"
#include "ui_defs.h"
#include "ui_style.h"
#include <stdio.h>

// ------------------------------
// SCREEN STRUCT (file-static)
// ------------------------------
{struct}

// ------------------------------
// BAR ANIMATION HELPER
// ------------------------------
{bar_anim}

// ------------------------------
// EVENT CALLBACKS
// (override in your application .c)
// ------------------------------
{callbacks}

// ------------------------------
// SETTERS
// ------------------------------
{setters}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void {load_fn}(void)
{{
    lv_scr_load({sv}.lv_screen);
}}

// ------------------------------
// SCREEN INIT
// ------------------------------
void {init_fn}(void)
{{
    {sv}.lv_screen = lv_obj_create(NULL);

{init_body}
}}
"""

_H_LAYOUT = """\
#ifndef {guard}
#define {guard}
#ifdef __cplusplus
extern "C" {{
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void {init_fn}(void);
void {load_fn}(void);

// ------------------------------
// SETTERS
// ------------------------------
{setters}

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
{cb_decls}

#ifdef __cplusplus
}}
#endif
#endif
"""
