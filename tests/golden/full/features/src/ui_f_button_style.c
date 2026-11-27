#include "ui_f_button_style.h"
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
    } btn_square;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_round;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_pill;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_stroke;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_both;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_thick;
} s_f_button_style = {
    .btn_square = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Square",
    },
    .btn_round = {
        .style = {
            .box = { .has_bg = true, .bg = 0x43A047, .has_radius = true, .radius = 8 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Round",
    },
    .btn_pill = {
        .style = {
            .box = { .has_bg = true, .bg = 0x8E24AA, .has_radius = true, .radius = 20 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Pill",
    },
    .btn_stroke = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFFFFFF, .has_border_color = true, .border_color = 0x1E88E5, .has_border_width = true, .border_width = 2 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Stroke",
    },
    .btn_both = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFB8C00, .has_border_color = true, .border_color = 0x5D4037, .has_border_width = true, .border_width = 3, .has_radius = true, .radius = 10 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Both",
    },
    .btn_thick = {
        .style = {
            .box = { .has_bg = true, .bg = 0x00897B, .has_border_color = true, .border_color = 0x004D40, .has_border_width = true, .border_width = 6, .has_radius = true, .radius = 4 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Thick",
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
void ui_f_button_style_btn_square_set_label(const char *text)
{
    if (!s_f_button_style.btn_square.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_style.btn_square.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_style_btn_round_set_label(const char *text)
{
    if (!s_f_button_style.btn_round.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_style.btn_round.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_style_btn_pill_set_label(const char *text)
{
    if (!s_f_button_style.btn_pill.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_style.btn_pill.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_style_btn_stroke_set_label(const char *text)
{
    if (!s_f_button_style.btn_stroke.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_style.btn_stroke.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_style_btn_both_set_label(const char *text)
{
    if (!s_f_button_style.btn_both.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_style.btn_both.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_f_button_style_btn_thick_set_label(const char *text)
{
    if (!s_f_button_style.btn_thick.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_button_style.btn_thick.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_button_style_load(void)
{
    lv_scr_load(s_f_button_style.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_f_button_style_init(void)
{
    s_f_button_style.lv_screen = lv_obj_create(NULL);

    /* btn_square (button) */
    s_f_button_style.btn_square.lv_obj = lv_button_create(s_f_button_style.lv_screen);
    lv_obj_set_pos(s_f_button_style.btn_square.lv_obj, 10, 10);
    lv_obj_set_size(s_f_button_style.btn_square.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_style.btn_square.lv_obj);
        lv_label_set_text(lbl, s_f_button_style.btn_square.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_style.btn_square.style);
    }
    lv_obj_add_event_cb(s_f_button_style.btn_square.lv_obj, ui_f_button_style_on_btn_square_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_button_style.btn_square.lv_obj, UI_CHILD_BUTTON, &s_f_button_style.btn_square.style);

    /* btn_round (button) */
    s_f_button_style.btn_round.lv_obj = lv_button_create(s_f_button_style.lv_screen);
    lv_obj_set_pos(s_f_button_style.btn_round.lv_obj, 110, 10);
    lv_obj_set_size(s_f_button_style.btn_round.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_style.btn_round.lv_obj);
        lv_label_set_text(lbl, s_f_button_style.btn_round.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_style.btn_round.style);
    }
    lv_obj_add_event_cb(s_f_button_style.btn_round.lv_obj, ui_f_button_style_on_btn_round_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_button_style.btn_round.lv_obj, UI_CHILD_BUTTON, &s_f_button_style.btn_round.style);

    /* btn_pill (button) */
    s_f_button_style.btn_pill.lv_obj = lv_button_create(s_f_button_style.lv_screen);
    lv_obj_set_pos(s_f_button_style.btn_pill.lv_obj, 210, 10);
    lv_obj_set_size(s_f_button_style.btn_pill.lv_obj, 100, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_style.btn_pill.lv_obj);
        lv_label_set_text(lbl, s_f_button_style.btn_pill.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_style.btn_pill.style);
    }
    lv_obj_add_event_cb(s_f_button_style.btn_pill.lv_obj, ui_f_button_style_on_btn_pill_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_button_style.btn_pill.lv_obj, UI_CHILD_BUTTON, &s_f_button_style.btn_pill.style);

    /* btn_stroke (button) */
    s_f_button_style.btn_stroke.lv_obj = lv_button_create(s_f_button_style.lv_screen);
    lv_obj_set_pos(s_f_button_style.btn_stroke.lv_obj, 10, 70);
    lv_obj_set_size(s_f_button_style.btn_stroke.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_style.btn_stroke.lv_obj);
        lv_label_set_text(lbl, s_f_button_style.btn_stroke.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_style.btn_stroke.style);
    }
    lv_obj_add_event_cb(s_f_button_style.btn_stroke.lv_obj, ui_f_button_style_on_btn_stroke_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_button_style.btn_stroke.lv_obj, UI_CHILD_BUTTON, &s_f_button_style.btn_stroke.style);

    /* btn_both (button) */
    s_f_button_style.btn_both.lv_obj = lv_button_create(s_f_button_style.lv_screen);
    lv_obj_set_pos(s_f_button_style.btn_both.lv_obj, 110, 70);
    lv_obj_set_size(s_f_button_style.btn_both.lv_obj, 90, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_style.btn_both.lv_obj);
        lv_label_set_text(lbl, s_f_button_style.btn_both.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_style.btn_both.style);
    }
    lv_obj_add_event_cb(s_f_button_style.btn_both.lv_obj, ui_f_button_style_on_btn_both_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_button_style.btn_both.lv_obj, UI_CHILD_BUTTON, &s_f_button_style.btn_both.style);

    /* btn_thick (button) */
    s_f_button_style.btn_thick.lv_obj = lv_button_create(s_f_button_style.lv_screen);
    lv_obj_set_pos(s_f_button_style.btn_thick.lv_obj, 210, 70);
    lv_obj_set_size(s_f_button_style.btn_thick.lv_obj, 100, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_button_style.btn_thick.lv_obj);
        lv_label_set_text(lbl, s_f_button_style.btn_thick.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_button_style.btn_thick.style);
    }
    lv_obj_add_event_cb(s_f_button_style.btn_thick.lv_obj, ui_f_button_style_on_btn_thick_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_button_style.btn_thick.lv_obj, UI_CHILD_BUTTON, &s_f_button_style.btn_thick.style);
}
