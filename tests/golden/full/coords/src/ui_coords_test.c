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
        char        label_text[UI_TEXT_SIZE(2)];
    } btn_a;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(2)];
    } btn_b;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(2)];
    } btn_c;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(2)];
    } btn_d;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(6)];
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
    ui_copy_text(s_coords_test.btn_a.label_text, sizeof s_coords_test.btn_a.label_text, text);
    if (!s_coords_test.btn_a.lv_obj) {
        UI_LIFECYCLE_WARN("ui_coords_test_btn_a_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_a.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_coords_test.btn_a.label_text);
}

void ui_coords_test_btn_b_set_label(const char *text)
{
    ui_copy_text(s_coords_test.btn_b.label_text, sizeof s_coords_test.btn_b.label_text, text);
    if (!s_coords_test.btn_b.lv_obj) {
        UI_LIFECYCLE_WARN("ui_coords_test_btn_b_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_b.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_coords_test.btn_b.label_text);
}

void ui_coords_test_btn_c_set_label(const char *text)
{
    ui_copy_text(s_coords_test.btn_c.label_text, sizeof s_coords_test.btn_c.label_text, text);
    if (!s_coords_test.btn_c.lv_obj) {
        UI_LIFECYCLE_WARN("ui_coords_test_btn_c_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_c.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_coords_test.btn_c.label_text);
}

void ui_coords_test_btn_d_set_label(const char *text)
{
    ui_copy_text(s_coords_test.btn_d.label_text, sizeof s_coords_test.btn_d.label_text, text);
    if (!s_coords_test.btn_d.lv_obj) {
        UI_LIFECYCLE_WARN("ui_coords_test_btn_d_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_d.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_coords_test.btn_d.label_text);
}

void ui_coords_test_btn_ghost_set_label(const char *text)
{
    ui_copy_text(s_coords_test.btn_ghost.label_text, sizeof s_coords_test.btn_ghost.label_text, text);
    if (!s_coords_test.btn_ghost.lv_obj) {
        UI_LIFECYCLE_WARN("ui_coords_test_btn_ghost_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test.btn_ghost.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_coords_test.btn_ghost.label_text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_coords_test_load(void)
{
    if (!s_coords_test.lv_screen) {
        UI_LIFECYCLE_WARN("ui_coords_test_load: screen not built");
        return;
    }
    lv_scr_load(s_coords_test.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_coords_test_init(void)
{
    if (s_coords_test.lv_screen)
        return;
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

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_coords_test_deinit(void)
{
    if (!s_coords_test.lv_screen)
        return;
    if (lv_screen_active() == s_coords_test.lv_screen) {
        UI_LIFECYCLE_WARN("ui_coords_test_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_coords_test.lv_screen);   /* frees every child, callback and animation */
    s_coords_test.lv_screen = NULL;
    s_coords_test.btn_a.lv_obj = NULL;
    s_coords_test.btn_b.lv_obj = NULL;
    s_coords_test.btn_c.lv_obj = NULL;
    s_coords_test.btn_d.lv_obj = NULL;
    s_coords_test.btn_ghost.lv_obj = NULL;
}
