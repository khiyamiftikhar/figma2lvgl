#include "ui_f_tiny.h"
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
        char        label_text[UI_TEXT_SIZE(4)];
    } btn_4x4;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(4)];
    } btn_1x1;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_tiny;
} s_f_tiny = {
    .btn_4x4 = {
        .style = {
            .box = { .has_bg = true, .bg = 0xD32F2F }
        },
        .label_text = "4X4",
    },
    .btn_1x1 = {
        .style = {
            .box = { .has_bg = true, .bg = 0xD32F2F }
        },
        .label_text = "1X1",
    },
    .slider_tiny = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
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
void ui_f_tiny_btn_4x4_set_label(const char *text)
{
    ui_copy_text(s_f_tiny.btn_4x4.label_text, sizeof s_f_tiny.btn_4x4.label_text, text);
    if (!s_f_tiny.btn_4x4.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_tiny_btn_4x4_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_f_tiny.btn_4x4.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_f_tiny.btn_4x4.label_text);
}

void ui_f_tiny_btn_1x1_set_label(const char *text)
{
    ui_copy_text(s_f_tiny.btn_1x1.label_text, sizeof s_f_tiny.btn_1x1.label_text, text);
    if (!s_f_tiny.btn_1x1.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_tiny_btn_1x1_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_f_tiny.btn_1x1.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_f_tiny.btn_1x1.label_text);
}

void ui_f_tiny_slider_tiny_set_value(int32_t value)
{
    s_f_tiny.slider_tiny.value = value;
    if (!s_f_tiny.slider_tiny.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_tiny_slider_tiny_set_value: screen not built; value stored");
        return;
    }
    lv_slider_set_value(s_f_tiny.slider_tiny.lv_obj, value, LV_ANIM_OFF);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_tiny_load(void)
{
    if (!s_f_tiny.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_tiny_load: screen not built");
        return;
    }
    lv_scr_load(s_f_tiny.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_tiny_init(void)
{
    if (s_f_tiny.lv_screen)
        return;
    s_f_tiny.lv_screen = lv_obj_create(NULL);

    /* btn_4x4 (button) */
    s_f_tiny.btn_4x4.lv_obj = lv_button_create(s_f_tiny.lv_screen);
    lv_obj_set_pos(s_f_tiny.btn_4x4.lv_obj, 20, 20);
    lv_obj_set_size(s_f_tiny.btn_4x4.lv_obj, 4, 4);
    {
        lv_obj_t *lbl = lv_label_create(s_f_tiny.btn_4x4.lv_obj);
        lv_label_set_text(lbl, s_f_tiny.btn_4x4.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_tiny.btn_4x4.style);
    }
    lv_obj_add_event_cb(s_f_tiny.btn_4x4.lv_obj, ui_f_tiny_on_btn_4x4_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_tiny.btn_4x4.lv_obj, UI_CHILD_BUTTON, &s_f_tiny.btn_4x4.style);

    /* btn_1x1 (button) */
    s_f_tiny.btn_1x1.lv_obj = lv_button_create(s_f_tiny.lv_screen);
    lv_obj_set_pos(s_f_tiny.btn_1x1.lv_obj, 60, 20);
    lv_obj_set_size(s_f_tiny.btn_1x1.lv_obj, 1, 1);
    {
        lv_obj_t *lbl = lv_label_create(s_f_tiny.btn_1x1.lv_obj);
        lv_label_set_text(lbl, s_f_tiny.btn_1x1.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_tiny.btn_1x1.style);
    }
    lv_obj_add_event_cb(s_f_tiny.btn_1x1.lv_obj, ui_f_tiny_on_btn_1x1_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_tiny.btn_1x1.lv_obj, UI_CHILD_BUTTON, &s_f_tiny.btn_1x1.style);

    /* slider_tiny (slider) */
    s_f_tiny.slider_tiny.lv_obj = lv_slider_create(s_f_tiny.lv_screen);
    lv_obj_set_pos(s_f_tiny.slider_tiny.lv_obj, 20, 60);
    lv_obj_set_size(s_f_tiny.slider_tiny.lv_obj, 40, 4);
    lv_slider_set_range(s_f_tiny.slider_tiny.lv_obj, s_f_tiny.slider_tiny.min, s_f_tiny.slider_tiny.max);
    lv_slider_set_value(s_f_tiny.slider_tiny.lv_obj, s_f_tiny.slider_tiny.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_f_tiny.slider_tiny.lv_obj, ui_f_tiny_on_slider_tiny,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_f_tiny.slider_tiny.lv_obj, UI_CHILD_SLIDER, &s_f_tiny.slider_tiny.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_tiny_deinit(void)
{
    if (!s_f_tiny.lv_screen)
        return;
    if (lv_screen_active() == s_f_tiny.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_tiny_deinit: refused, it is the active screen; load another first");
        return;
    }
    s_f_tiny.slider_tiny.value = lv_slider_get_value(s_f_tiny.slider_tiny.lv_obj);
    lv_obj_delete(s_f_tiny.lv_screen);   /* frees every child, callback and animation */
    s_f_tiny.lv_screen = NULL;
    s_f_tiny.btn_4x4.lv_obj = NULL;
    s_f_tiny.btn_1x1.lv_obj = NULL;
    s_f_tiny.slider_tiny.lv_obj = NULL;
}
