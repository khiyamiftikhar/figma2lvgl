#include "ui_f_covering.h"
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
        char        label_text[UI_TEXT_SIZE(6)];
    } btn_under_panel;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_cover;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(6)];
    } btn_under_button;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(4)];
    } btn_on_top;
} s_f_covering = {
    .btn_under_panel = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Under",
    },
    .panel_cover = {
        .style = {
            .box = { .has_bg = true, .bg = 0xBDBDBD }
        },
    },
    .btn_under_button = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Under",
    },
    .btn_on_top = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE53935 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Top",
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
void ui_f_covering_btn_under_panel_set_label(const char *text)
{
    ui_copy_text(s_f_covering.btn_under_panel.label_text, sizeof s_f_covering.btn_under_panel.label_text, text);
    if (!s_f_covering.btn_under_panel.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_covering_btn_under_panel_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_f_covering.btn_under_panel.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_f_covering.btn_under_panel.label_text);
}

void ui_f_covering_btn_under_button_set_label(const char *text)
{
    ui_copy_text(s_f_covering.btn_under_button.label_text, sizeof s_f_covering.btn_under_button.label_text, text);
    if (!s_f_covering.btn_under_button.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_covering_btn_under_button_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_f_covering.btn_under_button.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_f_covering.btn_under_button.label_text);
}

void ui_f_covering_btn_on_top_set_label(const char *text)
{
    ui_copy_text(s_f_covering.btn_on_top.label_text, sizeof s_f_covering.btn_on_top.label_text, text);
    if (!s_f_covering.btn_on_top.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_covering_btn_on_top_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_f_covering.btn_on_top.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_f_covering.btn_on_top.label_text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_covering_load(void)
{
    if (!s_f_covering.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_covering_load: screen not built");
        return;
    }
    lv_scr_load(s_f_covering.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_covering_init(void)
{
    if (s_f_covering.lv_screen)
        return;
    s_f_covering.lv_screen = lv_obj_create(NULL);

    /* btn_under_panel (button) */
    s_f_covering.btn_under_panel.lv_obj = lv_button_create(s_f_covering.lv_screen);
    lv_obj_set_pos(s_f_covering.btn_under_panel.lv_obj, 20, 30);
    lv_obj_set_size(s_f_covering.btn_under_panel.lv_obj, 120, 50);
    {
        lv_obj_t *lbl = lv_label_create(s_f_covering.btn_under_panel.lv_obj);
        lv_label_set_text(lbl, s_f_covering.btn_under_panel.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_covering.btn_under_panel.style);
    }
    lv_obj_add_event_cb(s_f_covering.btn_under_panel.lv_obj, ui_f_covering_on_btn_under_panel_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_covering.btn_under_panel.lv_obj, UI_CHILD_BUTTON, &s_f_covering.btn_under_panel.style);

    /* panel_cover (panel) */
    s_f_covering.panel_cover.lv_obj = lv_obj_create(s_f_covering.lv_screen);
    lv_obj_remove_style_all(s_f_covering.panel_cover.lv_obj);
    lv_obj_set_pos(s_f_covering.panel_cover.lv_obj, 10, 20);
    lv_obj_set_size(s_f_covering.panel_cover.lv_obj, 100, 70);
    lv_obj_clear_flag(s_f_covering.panel_cover.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_covering.panel_cover.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_f_covering.panel_cover.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_covering.panel_cover.lv_obj, UI_CHILD_PANEL, &s_f_covering.panel_cover.style);

    /* btn_under_button (button) */
    s_f_covering.btn_under_button.lv_obj = lv_button_create(s_f_covering.lv_screen);
    lv_obj_set_pos(s_f_covering.btn_under_button.lv_obj, 180, 30);
    lv_obj_set_size(s_f_covering.btn_under_button.lv_obj, 120, 50);
    {
        lv_obj_t *lbl = lv_label_create(s_f_covering.btn_under_button.lv_obj);
        lv_label_set_text(lbl, s_f_covering.btn_under_button.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_covering.btn_under_button.style);
    }
    lv_obj_add_event_cb(s_f_covering.btn_under_button.lv_obj, ui_f_covering_on_btn_under_button_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_covering.btn_under_button.lv_obj, UI_CHILD_BUTTON, &s_f_covering.btn_under_button.style);

    /* btn_on_top (button) */
    s_f_covering.btn_on_top.lv_obj = lv_button_create(s_f_covering.lv_screen);
    lv_obj_set_pos(s_f_covering.btn_on_top.lv_obj, 170, 20);
    lv_obj_set_size(s_f_covering.btn_on_top.lv_obj, 80, 70);
    {
        lv_obj_t *lbl = lv_label_create(s_f_covering.btn_on_top.lv_obj);
        lv_label_set_text(lbl, s_f_covering.btn_on_top.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_covering.btn_on_top.style);
    }
    lv_obj_add_event_cb(s_f_covering.btn_on_top.lv_obj, ui_f_covering_on_btn_on_top_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_covering.btn_on_top.lv_obj, UI_CHILD_BUTTON, &s_f_covering.btn_on_top.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_covering_deinit(void)
{
    if (!s_f_covering.lv_screen)
        return;
    if (lv_screen_active() == s_f_covering.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_covering_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_f_covering.lv_screen);   /* frees every child, callback and animation */
    s_f_covering.lv_screen = NULL;
    s_f_covering.btn_under_panel.lv_obj = NULL;
    s_f_covering.panel_cover.lv_obj = NULL;
    s_f_covering.btn_under_button.lv_obj = NULL;
    s_f_covering.btn_on_top.lv_obj = NULL;
}
