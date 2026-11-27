#include "ui_coords_test.h"
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
        const char *label_text;
    } btn_ghost;
} s_coords_test = {
    .btn_a = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE53935 }
        },
        .label_text = "A",
    },
    .btn_b = {
        .style = {
            .box = { .has_bg = true, .bg = 0x43A047 }
        },
        .label_text = "B",
    },
    .btn_c = {
        .style = {
            .box = { .has_bg = true, .bg = 0x1E88E5 }
        },
        .label_text = "C",
    },
    .btn_d = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFB8C00 }
        },
        .label_text = "D",
    },
    .btn_ghost = {
        .style = {
            .box = { .has_bg = true, .bg = 0x00897B },
            .effects = { .has_opacity = true, .opacity = 0 }
        },
        .label_text = "Ghost",
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
void ui_coords_test_btn_a_set_label(const char *text)
{
    if (!s_coords_test.btn_a.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_a.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_btn_b_set_label(const char *text)
{
    if (!s_coords_test.btn_b.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_b.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_btn_c_set_label(const char *text)
{
    if (!s_coords_test.btn_c.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_c.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_btn_d_set_label(const char *text)
{
    if (!s_coords_test.btn_d.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_d.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_btn_ghost_set_label(const char *text)
{
    if (!s_coords_test.btn_ghost.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_ghost.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_coords_test_load(void)
{
    lv_scr_load(s_coords_test.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_coords_test_init(void)
{
    s_coords_test.lv_screen = lv_obj_create(NULL);

    /* btn_a (button) */
    s_coords_test.btn_a.lv_obj = lv_button_create(s_coords_test.lv_screen);
    lv_obj_set_pos(s_coords_test.btn_a.lv_obj, 20, 30);
    lv_obj_set_size(s_coords_test.btn_a.lv_obj, 60, 30);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test.btn_a.lv_obj);
        lv_label_set_text(lbl, s_coords_test.btn_a.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_coords_test.btn_a.style);
    }
    lv_obj_add_event_cb(s_coords_test.btn_a.lv_obj, ui_coords_test_on_btn_a_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test.btn_a.lv_obj, UI_CHILD_BUTTON, &s_coords_test.btn_a.style);

    /* btn_b (button) */
    s_coords_test.btn_b.lv_obj = lv_button_create(s_coords_test.lv_screen);
    lv_obj_set_pos(s_coords_test.btn_b.lv_obj, 175, 45);
    lv_obj_set_size(s_coords_test.btn_b.lv_obj, 60, 30);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test.btn_b.lv_obj);
        lv_label_set_text(lbl, s_coords_test.btn_b.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_coords_test.btn_b.style);
    }
    lv_obj_add_event_cb(s_coords_test.btn_b.lv_obj, ui_coords_test_on_btn_b_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test.btn_b.lv_obj, UI_CHILD_BUTTON, &s_coords_test.btn_b.style);

    /* btn_c (button) */
    s_coords_test.btn_c.lv_obj = lv_button_create(s_coords_test.lv_screen);
    lv_obj_set_pos(s_coords_test.btn_c.lv_obj, 40, 160);
    lv_obj_set_size(s_coords_test.btn_c.lv_obj, 60, 30);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test.btn_c.lv_obj);
        lv_label_set_text(lbl, s_coords_test.btn_c.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_coords_test.btn_c.style);
    }
    lv_obj_add_event_cb(s_coords_test.btn_c.lv_obj, ui_coords_test_on_btn_c_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test.btn_c.lv_obj, UI_CHILD_BUTTON, &s_coords_test.btn_c.style);

    /* btn_d (button) */
    s_coords_test.btn_d.lv_obj = lv_button_create(s_coords_test.lv_screen);
    lv_obj_set_pos(s_coords_test.btn_d.lv_obj, 195, 175);
    lv_obj_set_size(s_coords_test.btn_d.lv_obj, 60, 30);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test.btn_d.lv_obj);
        lv_label_set_text(lbl, s_coords_test.btn_d.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_coords_test.btn_d.style);
    }
    lv_obj_add_event_cb(s_coords_test.btn_d.lv_obj, ui_coords_test_on_btn_d_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test.btn_d.lv_obj, UI_CHILD_BUTTON, &s_coords_test.btn_d.style);

    /* btn_ghost (button) */
    s_coords_test.btn_ghost.lv_obj = lv_button_create(s_coords_test.lv_screen);
    lv_obj_set_pos(s_coords_test.btn_ghost.lv_obj, 180, 360);
    lv_obj_set_size(s_coords_test.btn_ghost.lv_obj, 60, 30);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test.btn_ghost.lv_obj);
        lv_label_set_text(lbl, s_coords_test.btn_ghost.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_coords_test.btn_ghost.style);
    }
    lv_obj_add_event_cb(s_coords_test.btn_ghost.lv_obj, ui_coords_test_on_btn_ghost_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test.btn_ghost.lv_obj, UI_CHILD_BUTTON, &s_coords_test.btn_ghost.style);
}
