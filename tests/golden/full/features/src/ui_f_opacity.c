#include "ui_f_opacity.h"
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
    } panel_opa_0;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_opa_50;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_opa_100;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_opa_50;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } text_opa_50;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_fill_50;
} s_f_opacity = {
    .panel_opa_0 = {
        .style = {
            .box = { .has_bg = true, .bg = 0x3F51B5 },
            .effects = { .has_opacity = true, .opacity = 0 }
        },
    },
    .panel_opa_50 = {
        .style = {
            .box = { .has_bg = true, .bg = 0x3F51B5 },
            .effects = { .has_opacity = true, .opacity = 128 }
        },
    },
    .panel_opa_100 = {
        .style = {
            .box = { .has_bg = true, .bg = 0x3F51B5 }
        },
    },
    .btn_opa_50 = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 },
            .effects = { .has_opacity = true, .opacity = 128 }
        },
        .label_text = "Half",
    },
    .text_opa_50 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 16 },
            .effects = { .has_opacity = true, .opacity = 128 }
        },
        .text = "Half text",
    },
    .panel_fill_50 = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE91E63, .has_bg_opa = true, .bg_opa = 128 }
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
void ui_f_opacity_btn_opa_50_set_label(const char *text)
{
    if (!s_f_opacity.btn_opa_50.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_opacity.btn_opa_50.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_opacity_text_opa_50_set_text(const char *text)
{
    if (s_f_opacity.text_opa_50.lv_obj)
        lv_label_set_text(s_f_opacity.text_opa_50.lv_obj, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_opacity_load(void)
{
    lv_scr_load(s_f_opacity.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_f_opacity_init(void)
{
    s_f_opacity.lv_screen = lv_obj_create(NULL);

    /* panel_opa_0 (panel) */
    s_f_opacity.panel_opa_0.lv_obj = lv_obj_create(s_f_opacity.lv_screen);
    lv_obj_remove_style_all(s_f_opacity.panel_opa_0.lv_obj);
    lv_obj_set_pos(s_f_opacity.panel_opa_0.lv_obj, 10, 10);
    lv_obj_set_size(s_f_opacity.panel_opa_0.lv_obj, 90, 60);
    lv_obj_clear_flag(s_f_opacity.panel_opa_0.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_opacity.panel_opa_0.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_f_opacity.panel_opa_0.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_opacity.panel_opa_0.lv_obj, UI_CHILD_PANEL, &s_f_opacity.panel_opa_0.style);

    /* panel_opa_50 (panel) */
    s_f_opacity.panel_opa_50.lv_obj = lv_obj_create(s_f_opacity.lv_screen);
    lv_obj_remove_style_all(s_f_opacity.panel_opa_50.lv_obj);
    lv_obj_set_pos(s_f_opacity.panel_opa_50.lv_obj, 115, 10);
    lv_obj_set_size(s_f_opacity.panel_opa_50.lv_obj, 90, 60);
    lv_obj_clear_flag(s_f_opacity.panel_opa_50.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_opacity.panel_opa_50.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_f_opacity.panel_opa_50.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_opacity.panel_opa_50.lv_obj, UI_CHILD_PANEL, &s_f_opacity.panel_opa_50.style);

    /* panel_opa_100 (panel) */
    s_f_opacity.panel_opa_100.lv_obj = lv_obj_create(s_f_opacity.lv_screen);
    lv_obj_remove_style_all(s_f_opacity.panel_opa_100.lv_obj);
    lv_obj_set_pos(s_f_opacity.panel_opa_100.lv_obj, 220, 10);
    lv_obj_set_size(s_f_opacity.panel_opa_100.lv_obj, 90, 60);
    lv_obj_clear_flag(s_f_opacity.panel_opa_100.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_opacity.panel_opa_100.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_f_opacity.panel_opa_100.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_opacity.panel_opa_100.lv_obj, UI_CHILD_PANEL, &s_f_opacity.panel_opa_100.style);

    /* btn_opa_50 (button) */
    s_f_opacity.btn_opa_50.lv_obj = lv_button_create(s_f_opacity.lv_screen);
    lv_obj_set_pos(s_f_opacity.btn_opa_50.lv_obj, 10, 90);
    lv_obj_set_size(s_f_opacity.btn_opa_50.lv_obj, 140, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_opacity.btn_opa_50.lv_obj);
        lv_label_set_text(lbl, s_f_opacity.btn_opa_50.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_opacity.btn_opa_50.style);
    }
    lv_obj_add_event_cb(s_f_opacity.btn_opa_50.lv_obj, ui_f_opacity_on_btn_opa_50_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_opacity.btn_opa_50.lv_obj, UI_CHILD_BUTTON, &s_f_opacity.btn_opa_50.style);

    /* text_opa_50 (label) */
    s_f_opacity.text_opa_50.lv_obj = lv_label_create(s_f_opacity.lv_screen);
    lv_obj_set_pos(s_f_opacity.text_opa_50.lv_obj, 170, 100);
    lv_obj_set_width(s_f_opacity.text_opa_50.lv_obj, 68);
    lv_label_set_long_mode(s_f_opacity.text_opa_50.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_opacity.text_opa_50.lv_obj, s_f_opacity.text_opa_50.text);
    ui_apply_style(s_f_opacity.text_opa_50.lv_obj, UI_CHILD_LABEL, &s_f_opacity.text_opa_50.style);

    /* panel_fill_50 (panel) */
    s_f_opacity.panel_fill_50.lv_obj = lv_obj_create(s_f_opacity.lv_screen);
    lv_obj_remove_style_all(s_f_opacity.panel_fill_50.lv_obj);
    lv_obj_set_pos(s_f_opacity.panel_fill_50.lv_obj, 10, 150);
    lv_obj_set_size(s_f_opacity.panel_fill_50.lv_obj, 140, 50);
    lv_obj_clear_flag(s_f_opacity.panel_fill_50.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_opacity.panel_fill_50.lv_obj, LV_SCROLLBAR_MODE_OFF);
    ui_apply_style(s_f_opacity.panel_fill_50.lv_obj, UI_CHILD_PANEL, &s_f_opacity.panel_fill_50.style);
}
