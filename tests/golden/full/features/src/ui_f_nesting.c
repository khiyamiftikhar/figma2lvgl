#include "ui_f_nesting.h"
#include "assets.h"
#include "ui_defs.h"
#include "ui_style.h"
#include <stdio.h>

// ------------------------------
// SCREEN STRUCT (file-static)
// ------------------------------
static struct {
    lv_obj_t *lv_screen;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            struct {
                lv_obj_t   *lv_obj;
                ui_style_t  style;
                struct {
                    lv_obj_t   *lv_obj;
                    ui_style_t  style;
                } box_clipped;
            } panel_inner;
        } panel_middle;
    } panel_outer;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            struct {
                lv_obj_t   *lv_obj;
                ui_style_t  style;
                struct {
                    lv_obj_t   *lv_obj;
                    ui_style_t  style;
                } box_overflow;
            } panel_open_inner;
        } panel_open_mid;
    } panel_open;
} s_f_nesting = {
    .panel_outer = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE3F2FD }
        },
        .panel_middle = {
            .style = {
                .box = { .has_bg = true, .bg = 0x90CAF9 }
            },
            .panel_inner = {
                .style = {
                    .box = { .has_bg = true, .bg = 0x1E88E5 }
                },
                .box_clipped = {
                    .style = {
                        .box = { .has_bg = true, .bg = 0x0D47A1 }
                    },
                },
            },
        },
    },
    .panel_open = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFFF3E0 }
        },
        .panel_open_mid = {
            .style = {
                .box = { .has_bg = true, .bg = 0xFFCC80 }
            },
            .panel_open_inner = {
                .style = {
                    .box = { .has_bg = true, .bg = 0xFB8C00 }
                },
                .box_overflow = {
                    .style = {
                        .box = { .has_bg = true, .bg = 0xE65100 }
                    },
                },
            },
        },
    },
};

// ------------------------------
// BAR ANIMATION HELPER
// ------------------------------


// ------------------------------
// EVENT CALLBACKS
// Implement these in your application .c file.
// Linker will report undefined reference if not implemented.
// ------------------------------


// ------------------------------
// SETTERS
// ------------------------------


// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_nesting_load(void)
{
    if (!s_f_nesting.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_nesting_load: screen not built");
        return;
    }
    lv_scr_load(s_f_nesting.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_nesting_init(void)
{
    if (s_f_nesting.lv_screen)
        return;
    s_f_nesting.lv_screen = lv_obj_create(NULL);

    /* panel_outer (panel) */
    s_f_nesting.panel_outer.lv_obj = lv_obj_create(s_f_nesting.lv_screen);
    lv_obj_remove_style_all(s_f_nesting.panel_outer.lv_obj);
    lv_obj_set_pos(s_f_nesting.panel_outer.lv_obj, 10, 10);
    lv_obj_set_size(s_f_nesting.panel_outer.lv_obj, 140, 140);
    lv_obj_clear_flag(s_f_nesting.panel_outer.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_nesting.panel_outer.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_f_nesting.panel_outer.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_nesting.panel_outer.lv_obj, UI_CHILD_PANEL, &s_f_nesting.panel_outer.style);

    /* panel_open (panel) */
    s_f_nesting.panel_open.lv_obj = lv_obj_create(s_f_nesting.lv_screen);
    lv_obj_remove_style_all(s_f_nesting.panel_open.lv_obj);
    lv_obj_set_pos(s_f_nesting.panel_open.lv_obj, 170, 10);
    lv_obj_set_size(s_f_nesting.panel_open.lv_obj, 140, 140);
    lv_obj_clear_flag(s_f_nesting.panel_open.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_nesting.panel_open.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_f_nesting.panel_open.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_f_nesting.panel_open.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_nesting.panel_open.lv_obj, UI_CHILD_PANEL, &s_f_nesting.panel_open.style);

    /* panel_middle (panel) */
    s_f_nesting.panel_outer.panel_middle.lv_obj = lv_obj_create(s_f_nesting.panel_outer.lv_obj);
    lv_obj_remove_style_all(s_f_nesting.panel_outer.panel_middle.lv_obj);
    lv_obj_set_pos(s_f_nesting.panel_outer.panel_middle.lv_obj, 20, 20);
    lv_obj_set_size(s_f_nesting.panel_outer.panel_middle.lv_obj, 120, 100);
    lv_obj_clear_flag(s_f_nesting.panel_outer.panel_middle.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_nesting.panel_outer.panel_middle.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_f_nesting.panel_outer.panel_middle.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_nesting.panel_outer.panel_middle.lv_obj, UI_CHILD_PANEL, &s_f_nesting.panel_outer.panel_middle.style);

    /* panel_open_mid (panel) */
    s_f_nesting.panel_open.panel_open_mid.lv_obj = lv_obj_create(s_f_nesting.panel_open.lv_obj);
    lv_obj_remove_style_all(s_f_nesting.panel_open.panel_open_mid.lv_obj);
    lv_obj_set_pos(s_f_nesting.panel_open.panel_open_mid.lv_obj, 20, 20);
    lv_obj_set_size(s_f_nesting.panel_open.panel_open_mid.lv_obj, 120, 100);
    lv_obj_clear_flag(s_f_nesting.panel_open.panel_open_mid.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_nesting.panel_open.panel_open_mid.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_f_nesting.panel_open.panel_open_mid.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_f_nesting.panel_open.panel_open_mid.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_nesting.panel_open.panel_open_mid.lv_obj, UI_CHILD_PANEL, &s_f_nesting.panel_open.panel_open_mid.style);

    /* panel_inner (panel) */
    s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj = lv_obj_create(s_f_nesting.panel_outer.panel_middle.lv_obj);
    lv_obj_remove_style_all(s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj);
    lv_obj_set_pos(s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj, 30, 30);
    lv_obj_set_size(s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj, 110, 60);
    lv_obj_clear_flag(s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj, UI_CHILD_PANEL, &s_f_nesting.panel_outer.panel_middle.panel_inner.style);

    /* panel_open_inner (panel) */
    s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj = lv_obj_create(s_f_nesting.panel_open.panel_open_mid.lv_obj);
    lv_obj_remove_style_all(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj);
    lv_obj_set_pos(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj, 30, 30);
    lv_obj_set_size(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj, 110, 60);
    lv_obj_clear_flag(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj, UI_CHILD_PANEL, &s_f_nesting.panel_open.panel_open_mid.panel_open_inner.style);

    /* box_clipped (panel) */
    s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj = lv_obj_create(s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj);
    lv_obj_remove_style_all(s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj);
    lv_obj_set_pos(s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj, 40, 10);
    lv_obj_set_size(s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj, 60, 30);
    lv_obj_clear_flag(s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj, UI_CHILD_PANEL, &s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.style);

    /* box_overflow (panel) */
    s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj = lv_obj_create(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj);
    lv_obj_remove_style_all(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj);
    lv_obj_set_pos(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj, 40, 10);
    lv_obj_set_size(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj, 60, 30);
    lv_obj_clear_flag(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj, UI_CHILD_PANEL, &s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_nesting_deinit(void)
{
    if (!s_f_nesting.lv_screen)
        return;
    if (lv_screen_active() == s_f_nesting.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_nesting_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_f_nesting.lv_screen);   /* frees every child, callback and animation */
    s_f_nesting.lv_screen = NULL;
    s_f_nesting.panel_outer.lv_obj = NULL;
    s_f_nesting.panel_open.lv_obj = NULL;
    s_f_nesting.panel_outer.panel_middle.lv_obj = NULL;
    s_f_nesting.panel_open.panel_open_mid.lv_obj = NULL;
    s_f_nesting.panel_outer.panel_middle.panel_inner.lv_obj = NULL;
    s_f_nesting.panel_open.panel_open_mid.panel_open_inner.lv_obj = NULL;
    s_f_nesting.panel_outer.panel_middle.panel_inner.box_clipped.lv_obj = NULL;
    s_f_nesting.panel_open.panel_open_mid.panel_open_inner.box_overflow.lv_obj = NULL;
}
