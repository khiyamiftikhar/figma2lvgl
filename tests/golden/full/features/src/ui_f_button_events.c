#include "ui_f_button_events.h"
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
        const char *label_text;
    } btn_tap;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_hold;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_repeat;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_down;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_up;
} s_f_button_events = {
    .btn_tap = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Tap",
    },
    .btn_hold = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Hold",
    },
    .btn_repeat = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Repeat",
    },
    .btn_down = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Down",
    },
    .btn_up = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Up",
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
void ui_f_button_events_btn_tap_set_label(const char *text)
{
    if (!s_f_button_events.btn_tap.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_events.btn_tap.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_events_btn_hold_set_label(const char *text)
{
    if (!s_f_button_events.btn_hold.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_events.btn_hold.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_events_btn_repeat_set_label(const char *text)
{
    if (!s_f_button_events.btn_repeat.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_events.btn_repeat.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_events_btn_down_set_label(const char *text)
{
    if (!s_f_button_events.btn_down.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_events.btn_down.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_events_btn_up_set_label(const char *text)
{
    if (!s_f_button_events.btn_up.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_events.btn_up.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_button_events_load(void)
{
    lv_scr_load(s_f_button_events.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_f_button_events_init(void)
{
    s_f_button_events.lv_screen = lv_obj_create(NULL);

    /* btn_tap (button) */
    s_f_button_events.btn_tap.lv_obj = lv_button_create(s_f_button_events.lv_screen);
    lv_obj_set_pos(s_f_button_events.btn_tap.lv_obj, 10, 10);
    lv_obj_set_size(s_f_button_events.btn_tap.lv_obj, 140, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_events.btn_tap.lv_obj);
        lv_label_set_text(lbl, s_f_button_events.btn_tap.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_events.btn_tap.style);
    }
    lv_obj_add_event_cb(s_f_button_events.btn_tap.lv_obj, ui_f_button_events_on_btn_tap_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_button_events.btn_tap.lv_obj, UI_CHILD_BUTTON, &s_f_button_events.btn_tap.style);

    /* btn_hold (button) */
    s_f_button_events.btn_hold.lv_obj = lv_button_create(s_f_button_events.lv_screen);
    lv_obj_set_pos(s_f_button_events.btn_hold.lv_obj, 170, 10);
    lv_obj_set_size(s_f_button_events.btn_hold.lv_obj, 140, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_events.btn_hold.lv_obj);
        lv_label_set_text(lbl, s_f_button_events.btn_hold.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_events.btn_hold.style);
    }
    lv_obj_add_event_cb(s_f_button_events.btn_hold.lv_obj, ui_f_button_events_on_btn_hold_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_f_button_events.btn_hold.lv_obj, ui_f_button_events_on_btn_hold_long_pressed,
                        LV_EVENT_LONG_PRESSED, NULL);
    ui_apply_style(s_f_button_events.btn_hold.lv_obj, UI_CHILD_BUTTON, &s_f_button_events.btn_hold.style);

    /* btn_repeat (button) */
    s_f_button_events.btn_repeat.lv_obj = lv_button_create(s_f_button_events.lv_screen);
    lv_obj_set_pos(s_f_button_events.btn_repeat.lv_obj, 10, 70);
    lv_obj_set_size(s_f_button_events.btn_repeat.lv_obj, 140, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_events.btn_repeat.lv_obj);
        lv_label_set_text(lbl, s_f_button_events.btn_repeat.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_events.btn_repeat.style);
    }
    lv_obj_add_event_cb(s_f_button_events.btn_repeat.lv_obj, ui_f_button_events_on_btn_repeat_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_f_button_events.btn_repeat.lv_obj, ui_f_button_events_on_btn_repeat_long_pressed_repeat,
                        LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    ui_apply_style(s_f_button_events.btn_repeat.lv_obj, UI_CHILD_BUTTON, &s_f_button_events.btn_repeat.style);

    /* btn_down (button) */
    s_f_button_events.btn_down.lv_obj = lv_button_create(s_f_button_events.lv_screen);
    lv_obj_set_pos(s_f_button_events.btn_down.lv_obj, 170, 70);
    lv_obj_set_size(s_f_button_events.btn_down.lv_obj, 140, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_events.btn_down.lv_obj);
        lv_label_set_text(lbl, s_f_button_events.btn_down.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_events.btn_down.style);
    }
    lv_obj_add_event_cb(s_f_button_events.btn_down.lv_obj, ui_f_button_events_on_btn_down_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_f_button_events.btn_down.lv_obj, ui_f_button_events_on_btn_down_pressed,
                        LV_EVENT_PRESSED, NULL);
    ui_apply_style(s_f_button_events.btn_down.lv_obj, UI_CHILD_BUTTON, &s_f_button_events.btn_down.style);

    /* btn_up (button) */
    s_f_button_events.btn_up.lv_obj = lv_button_create(s_f_button_events.lv_screen);
    lv_obj_set_pos(s_f_button_events.btn_up.lv_obj, 10, 130);
    lv_obj_set_size(s_f_button_events.btn_up.lv_obj, 140, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_events.btn_up.lv_obj);
        lv_label_set_text(lbl, s_f_button_events.btn_up.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_events.btn_up.style);
    }
    lv_obj_add_event_cb(s_f_button_events.btn_up.lv_obj, ui_f_button_events_on_btn_up_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_f_button_events.btn_up.lv_obj, ui_f_button_events_on_btn_up_released,
                        LV_EVENT_RELEASED, NULL);
    ui_apply_style(s_f_button_events.btn_up.lv_obj, UI_CHILD_BUTTON, &s_f_button_events.btn_up.style);
}
