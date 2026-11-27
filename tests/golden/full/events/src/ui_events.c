#include "ui_events.h"
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
    } btn_plain;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_a;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_b;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_c;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_d;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_h;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_v;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_rev;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_vrev;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_under_panel;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_over;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_under_button;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_over_button;
} s_events = {
    .btn_plain = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Plain",
    },
    .btn_a = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "A Press",
    },
    .btn_b = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "B Release",
    },
    .btn_c = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "C Lp",
    },
    .btn_d = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "D Lpr",
    },
    .slider_h = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .slider_v = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .slider_rev = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 100,
        .max   = 0,
    },
    .slider_vrev = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 50,
        .max   = -50,
    },
    .btn_under_panel = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Under Panel",
    },
    .panel_over = {
        .style = {
            .box = { .has_bg = true, .bg = 0xCCCCCC }
        },
    },
    .btn_under_button = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Under Button",
    },
    .btn_over_button = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE53935 }
        },
        .label_text = "Over Button",
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
void ui_events_btn_plain_set_label(const char *text)
{
    if (!s_events.btn_plain.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_events.btn_plain.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_events_btn_a_set_label(const char *text)
{
    if (!s_events.btn_a.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_events.btn_a.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_events_btn_b_set_label(const char *text)
{
    if (!s_events.btn_b.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_events.btn_b.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_events_btn_c_set_label(const char *text)
{
    if (!s_events.btn_c.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_events.btn_c.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_events_btn_d_set_label(const char *text)
{
    if (!s_events.btn_d.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_events.btn_d.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_events_slider_h_set_value(int32_t value)
{
    if (s_events.slider_h.lv_obj)
        lv_slider_set_value(s_events.slider_h.lv_obj, value, LV_ANIM_OFF);
}

void ui_events_slider_v_set_value(int32_t value)
{
    if (s_events.slider_v.lv_obj)
        lv_slider_set_value(s_events.slider_v.lv_obj, value, LV_ANIM_OFF);
}

void ui_events_slider_rev_set_value(int32_t value)
{
    if (s_events.slider_rev.lv_obj)
        lv_slider_set_value(s_events.slider_rev.lv_obj, value, LV_ANIM_OFF);
}

void ui_events_slider_vrev_set_value(int32_t value)
{
    if (s_events.slider_vrev.lv_obj)
        lv_slider_set_value(s_events.slider_vrev.lv_obj, value, LV_ANIM_OFF);
}

void ui_events_btn_under_panel_set_label(const char *text)
{
    if (!s_events.btn_under_panel.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_events.btn_under_panel.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_events_btn_under_button_set_label(const char *text)
{
    if (!s_events.btn_under_button.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_events.btn_under_button.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_events_btn_over_button_set_label(const char *text)
{
    if (!s_events.btn_over_button.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_events.btn_over_button.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_events_load(void)
{
    lv_scr_load(s_events.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_events_init(void)
{
    s_events.lv_screen = lv_obj_create(NULL);

    /* btn_plain (button) */
    s_events.btn_plain.lv_obj = lv_button_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.btn_plain.lv_obj, 10, 10);
    lv_obj_set_size(s_events.btn_plain.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_events.btn_plain.lv_obj);
        lv_label_set_text(lbl, s_events.btn_plain.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_events.btn_plain.style);
    }
    lv_obj_add_event_cb(s_events.btn_plain.lv_obj, ui_events_on_btn_plain_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_events.btn_plain.lv_obj, UI_CHILD_BUTTON, &s_events.btn_plain.style);

    /* btn_a (button) */
    s_events.btn_a.lv_obj = lv_button_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.btn_a.lv_obj, 110, 10);
    lv_obj_set_size(s_events.btn_a.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_events.btn_a.lv_obj);
        lv_label_set_text(lbl, s_events.btn_a.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_events.btn_a.style);
    }
    lv_obj_add_event_cb(s_events.btn_a.lv_obj, ui_events_on_btn_a_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_events.btn_a.lv_obj, ui_events_on_btn_a_pressed,
                        LV_EVENT_PRESSED, NULL);
    ui_apply_style(s_events.btn_a.lv_obj, UI_CHILD_BUTTON, &s_events.btn_a.style);

    /* btn_b (button) */
    s_events.btn_b.lv_obj = lv_button_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.btn_b.lv_obj, 210, 10);
    lv_obj_set_size(s_events.btn_b.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_events.btn_b.lv_obj);
        lv_label_set_text(lbl, s_events.btn_b.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_events.btn_b.style);
    }
    lv_obj_add_event_cb(s_events.btn_b.lv_obj, ui_events_on_btn_b_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_events.btn_b.lv_obj, ui_events_on_btn_b_released,
                        LV_EVENT_RELEASED, NULL);
    ui_apply_style(s_events.btn_b.lv_obj, UI_CHILD_BUTTON, &s_events.btn_b.style);

    /* btn_c (button) */
    s_events.btn_c.lv_obj = lv_button_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.btn_c.lv_obj, 10, 60);
    lv_obj_set_size(s_events.btn_c.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_events.btn_c.lv_obj);
        lv_label_set_text(lbl, s_events.btn_c.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_events.btn_c.style);
    }
    lv_obj_add_event_cb(s_events.btn_c.lv_obj, ui_events_on_btn_c_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_events.btn_c.lv_obj, ui_events_on_btn_c_long_pressed,
                        LV_EVENT_LONG_PRESSED, NULL);
    ui_apply_style(s_events.btn_c.lv_obj, UI_CHILD_BUTTON, &s_events.btn_c.style);

    /* btn_d (button) */
    s_events.btn_d.lv_obj = lv_button_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.btn_d.lv_obj, 110, 60);
    lv_obj_set_size(s_events.btn_d.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_events.btn_d.lv_obj);
        lv_label_set_text(lbl, s_events.btn_d.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_events.btn_d.style);
    }
    lv_obj_add_event_cb(s_events.btn_d.lv_obj, ui_events_on_btn_d_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_events.btn_d.lv_obj, ui_events_on_btn_d_long_pressed_repeat,
                        LV_EVENT_LONG_PRESSED_REPEAT, NULL);
    ui_apply_style(s_events.btn_d.lv_obj, UI_CHILD_BUTTON, &s_events.btn_d.style);

    /* slider_h (slider) */
    s_events.slider_h.lv_obj = lv_slider_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.slider_h.lv_obj, 10, 120);
    lv_obj_set_size(s_events.slider_h.lv_obj, 200, 20);
    lv_slider_set_range(s_events.slider_h.lv_obj, s_events.slider_h.min, s_events.slider_h.max);
    lv_slider_set_value(s_events.slider_h.lv_obj, s_events.slider_h.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_events.slider_h.lv_obj, ui_events_on_slider_h,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_events.slider_h.lv_obj, UI_CHILD_SLIDER, &s_events.slider_h.style);

    /* slider_v (slider) */
    s_events.slider_v.lv_obj = lv_slider_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.slider_v.lv_obj, 250, 120);
    lv_obj_set_size(s_events.slider_v.lv_obj, 20, 200);
    lv_slider_set_range(s_events.slider_v.lv_obj, s_events.slider_v.min, s_events.slider_v.max);
    lv_slider_set_value(s_events.slider_v.lv_obj, s_events.slider_v.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_events.slider_v.lv_obj, ui_events_on_slider_v,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_events.slider_v.lv_obj, UI_CHILD_SLIDER, &s_events.slider_v.style);

    /* slider_rev (slider) */
    s_events.slider_rev.lv_obj = lv_slider_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.slider_rev.lv_obj, 10, 160);
    lv_obj_set_size(s_events.slider_rev.lv_obj, 200, 20);
    lv_slider_set_range(s_events.slider_rev.lv_obj, s_events.slider_rev.min, s_events.slider_rev.max);
    lv_slider_set_value(s_events.slider_rev.lv_obj, s_events.slider_rev.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_events.slider_rev.lv_obj, ui_events_on_slider_rev,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_events.slider_rev.lv_obj, UI_CHILD_SLIDER, &s_events.slider_rev.style);

    /* slider_vrev (slider) */
    s_events.slider_vrev.lv_obj = lv_slider_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.slider_vrev.lv_obj, 290, 120);
    lv_obj_set_size(s_events.slider_vrev.lv_obj, 20, 200);
    lv_slider_set_range(s_events.slider_vrev.lv_obj, s_events.slider_vrev.min, s_events.slider_vrev.max);
    lv_slider_set_value(s_events.slider_vrev.lv_obj, s_events.slider_vrev.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_events.slider_vrev.lv_obj, ui_events_on_slider_vrev,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_events.slider_vrev.lv_obj, UI_CHILD_SLIDER, &s_events.slider_vrev.style);

    /* btn_under_panel (button) */
    s_events.btn_under_panel.lv_obj = lv_button_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.btn_under_panel.lv_obj, 10, 220);
    lv_obj_set_size(s_events.btn_under_panel.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_events.btn_under_panel.lv_obj);
        lv_label_set_text(lbl, s_events.btn_under_panel.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_events.btn_under_panel.style);
    }
    lv_obj_add_event_cb(s_events.btn_under_panel.lv_obj, ui_events_on_btn_under_panel_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_events.btn_under_panel.lv_obj, UI_CHILD_BUTTON, &s_events.btn_under_panel.style);

    /* panel_over (panel) */
    s_events.panel_over.lv_obj = lv_obj_create(s_events.lv_screen);
    lv_obj_remove_style_all(s_events.panel_over.lv_obj);
    lv_obj_set_pos(s_events.panel_over.lv_obj, 0, 210);
    lv_obj_set_size(s_events.panel_over.lv_obj, 120, 60);
    lv_obj_clear_flag(s_events.panel_over.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_events.panel_over.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_events.panel_over.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_events.panel_over.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_events.panel_over.lv_obj, UI_CHILD_PANEL, &s_events.panel_over.style);

    /* btn_under_button (button) */
    s_events.btn_under_button.lv_obj = lv_button_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.btn_under_button.lv_obj, 130, 220);
    lv_obj_set_size(s_events.btn_under_button.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_events.btn_under_button.lv_obj);
        lv_label_set_text(lbl, s_events.btn_under_button.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_events.btn_under_button.style);
    }
    lv_obj_add_event_cb(s_events.btn_under_button.lv_obj, ui_events_on_btn_under_button_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_events.btn_under_button.lv_obj, UI_CHILD_BUTTON, &s_events.btn_under_button.style);

    /* btn_over_button (button) */
    s_events.btn_over_button.lv_obj = lv_button_create(s_events.lv_screen);
    lv_obj_set_pos(s_events.btn_over_button.lv_obj, 120, 210);
    lv_obj_set_size(s_events.btn_over_button.lv_obj, 110, 60);
    {
        lv_obj_t *lbl = lv_label_create(s_events.btn_over_button.lv_obj);
        lv_label_set_text(lbl, s_events.btn_over_button.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_events.btn_over_button.style);
    }
    lv_obj_add_event_cb(s_events.btn_over_button.lv_obj, ui_events_on_btn_over_button_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_events.btn_over_button.lv_obj, UI_CHILD_BUTTON, &s_events.btn_over_button.style);
}
