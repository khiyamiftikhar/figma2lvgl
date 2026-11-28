# core/generator.py  (v0.4.0)
#
# Orchestrates the three emitters to produce .c and .h files for one screen.
# Three-pass approach:
#   1. node_emitter   → struct type definition + static initializer
#   2. init_emitter   → flat BFS _init() body
#   3. setter_emitter → setters, callbacks, prototypes

from figma2lvgl.core.figma_parser import ParsedScreen, assign_design_symbols
from figma2lvgl.core.node_emitter import emit_screen_struct_type
from figma2lvgl.core.init_emitter import emit_init_body
from figma2lvgl.core.setter_emitter import collect_setters_and_callbacks
from figma2lvgl.core.widget_type import WidgetType


_BAR_ANIM_CB = """\
static void bar_anim_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value((lv_obj_t *)obj, v, LV_ANIM_OFF);
}"""


def generate_screen(screen: ParsedScreen) -> tuple[str, str, str, str]:
    """
    Generate C and H source for one screen.

    Returns (c_filename, h_filename, h_text, c_text).
    """
    if not screen.symbols:
        # Parsed on its own (parse_screen), not through parse_design.
        assign_design_symbols([screen])
    snake   = screen.snake
    sv      = f"s_{snake}"
    guard   = f"UI_{snake.upper()}_H"
    h_fname = f"ui_{snake}.h"
    c_fname = f"ui_{snake}.c"
    init_fn = screen.symbols["init"]
    load_fn = screen.symbols["load"]
    deinit_fn = screen.symbols["deinit"]
    nodes = screen.all_nodes_bfs()
    # deinit: read back what the user can change (slider values), delete
    # the hierarchy, reset every pointer (design doc 11, rules 1 and 3).
    readback = "".join(f"    {path}.value = lv_slider_get_value({path}.lv_obj);\n"
                       for node, path, _parent in nodes
                       if node.widget_type == WidgetType.SLIDER)
    resets = "".join(f"    {path}.lv_obj = NULL;\n" for _node, path, _parent in nodes)

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
        deinit_fn  = deinit_fn,
        readback   = readback,
        resets     = resets,
    )

    # ── Assemble .h ───────────────────────────────────────────────────────────
    h_text = _H_LAYOUT.format(
        guard       = guard,
        init_fn     = init_fn,
        load_fn     = load_fn,
        deinit_fn   = deinit_fn,
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
// Implement these in your application .c file.
// Linker will report undefined reference if not implemented.
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
    if (!{sv}.lv_screen) {{
        UI_LIFECYCLE_WARN("{load_fn}: screen not built");
        return;
    }}
    lv_scr_load({sv}.lv_screen);
}}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void {init_fn}(void)
{{
    if ({sv}.lv_screen)
        return;
    {sv}.lv_screen = lv_obj_create(NULL);

{init_body}
}}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void {deinit_fn}(void)
{{
    if (!{sv}.lv_screen)
        return;
    if (lv_screen_active() == {sv}.lv_screen) {{
        UI_LIFECYCLE_WARN("{deinit_fn}: refused, it is the active screen; load another first");
        return;
    }}
{readback}    lv_obj_delete({sv}.lv_screen);   /* frees every child, callback and animation */
    {sv}.lv_screen = NULL;
{resets}}}
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
// The screen's values live in RAM whether or not it is built: setters store
// them, init builds the screen from them, deinit frees the LVGL objects and
// keeps them. Call these from the thread that runs LVGL, or with LVGL locked.
// ------------------------------

/* Builds the screen from its values. Does nothing if it is already built. */
void {init_fn}(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void {load_fn}(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void {deinit_fn}(void);

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
