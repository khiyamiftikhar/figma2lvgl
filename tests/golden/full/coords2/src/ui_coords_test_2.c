#include "ui_coords_test_2.h"
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
    } btn_g2;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_f2;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_mix;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_al1;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_al2;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_al3;
    } auto_box;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_card;
    } card;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_pg1;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_pg2;
    } panel_grp;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_overflow;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_rot;
} s_coords_test_2 = {
    .btn_g2 = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE53935 }
        },
        .label_text = "G2",
    },
    .btn_f2 = {
        .style = {
            .box = { .has_bg = true, .bg = 0x43A047 }
        },
        .label_text = "F2",
    },
    .btn_mix = {
        .style = {
            .box = { .has_bg = true, .bg = 0x1E88E5 }
        },
        .label_text = "Mix",
    },
    .auto_box = {
        .style = {
            .box = { .has_bg = true, .bg = 0xECEFF1 }
        },
        .btn_al1 = {
            .style = {
                .box = { .has_bg = true, .bg = 0x8E24AA }
            },
            .label_text = "Al1",
        },
        .btn_al2 = {
            .style = {
                .box = { .has_bg = true, .bg = 0x8E24AA }
            },
            .label_text = "Al2",
        },
        .btn_al3 = {
            .style = {
                .box = { .has_bg = true, .bg = 0x8E24AA }
            },
            .label_text = "Al3",
        },
    },
    .card = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFFF3E0 }
        },
        .btn_card = {
            .style = {
                .box = { .has_bg = true, .bg = 0xFB8C00 }
            },
            .label_text = "Card",
        },
    },
    .panel_grp = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
        .btn_pg1 = {
            .style = {
                .box = { .has_bg = true, .bg = 0x00897B }
            },
            .label_text = "Pg1",
        },
        .btn_pg2 = {
            .style = {
                .box = { .has_bg = true, .bg = 0x00897B }
            },
            .label_text = "Pg2",
        },
    },
    .btn_overflow = {
        .style = {
            .box = { .has_bg = true, .bg = 0x6D4C41 }
        },
        .label_text = "Overflow",
    },
    .btn_rot = {
        .style = {
            .box = { .has_bg = true, .bg = 0xC0CA33 }
        },
        .label_text = "Rot",
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
void ui_coords_test_2_btn_g2_set_label(const char *text)
{
    if (!s_coords_test_2.btn_g2.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.btn_g2.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_btn_f2_set_label(const char *text)
{
    if (!s_coords_test_2.btn_f2.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.btn_f2.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_btn_mix_set_label(const char *text)
{
    if (!s_coords_test_2.btn_mix.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.btn_mix.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_btn_overflow_set_label(const char *text)
{
    if (!s_coords_test_2.btn_overflow.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.btn_overflow.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_btn_rot_set_label(const char *text)
{
    if (!s_coords_test_2.btn_rot.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.btn_rot.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_auto_box_btn_al1_set_label(const char *text)
{
    if (!s_coords_test_2.auto_box.btn_al1.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.auto_box.btn_al1.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_auto_box_btn_al2_set_label(const char *text)
{
    if (!s_coords_test_2.auto_box.btn_al2.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.auto_box.btn_al2.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_auto_box_btn_al3_set_label(const char *text)
{
    if (!s_coords_test_2.auto_box.btn_al3.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.auto_box.btn_al3.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_card_btn_card_set_label(const char *text)
{
    if (!s_coords_test_2.card.btn_card.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.card.btn_card.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_panel_grp_btn_pg1_set_label(const char *text)
{
    if (!s_coords_test_2.panel_grp.btn_pg1.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.panel_grp.btn_pg1.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_coords_test_2_panel_grp_btn_pg2_set_label(const char *text)
{
    if (!s_coords_test_2.panel_grp.btn_pg2.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_coords_test_2.panel_grp.btn_pg2.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_coords_test_2_load(void)
{
    lv_scr_load(s_coords_test_2.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_coords_test_2_init(void)
{
    s_coords_test_2.lv_screen = lv_obj_create(NULL);

    /* btn_g2 (button) */
    s_coords_test_2.btn_g2.lv_obj = lv_button_create(s_coords_test_2.lv_screen);
    lv_obj_set_pos(s_coords_test_2.btn_g2.lv_obj, 20, 20);
    lv_obj_set_size(s_coords_test_2.btn_g2.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.btn_g2.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.btn_g2.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.btn_g2.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.btn_g2.lv_obj, ui_coords_test_2_on_btn_g2_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.btn_g2.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.btn_g2.style);

    /* btn_f2 (button) */
    s_coords_test_2.btn_f2.lv_obj = lv_button_create(s_coords_test_2.lv_screen);
    lv_obj_set_pos(s_coords_test_2.btn_f2.lv_obj, 120, 33);
    lv_obj_set_size(s_coords_test_2.btn_f2.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.btn_f2.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.btn_f2.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.btn_f2.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.btn_f2.lv_obj, ui_coords_test_2_on_btn_f2_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.btn_f2.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.btn_f2.style);

    /* btn_mix (button) */
    s_coords_test_2.btn_mix.lv_obj = lv_button_create(s_coords_test_2.lv_screen);
    lv_obj_set_pos(s_coords_test_2.btn_mix.lv_obj, 236, 47);
    lv_obj_set_size(s_coords_test_2.btn_mix.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.btn_mix.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.btn_mix.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.btn_mix.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.btn_mix.lv_obj, ui_coords_test_2_on_btn_mix_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.btn_mix.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.btn_mix.style);

    /* auto_box (panel) */
    s_coords_test_2.auto_box.lv_obj = lv_obj_create(s_coords_test_2.lv_screen);
    lv_obj_remove_style_all(s_coords_test_2.auto_box.lv_obj);
    lv_obj_set_pos(s_coords_test_2.auto_box.lv_obj, 10, 110);
    lv_obj_set_size(s_coords_test_2.auto_box.lv_obj, 130, 120);
    lv_obj_clear_flag(s_coords_test_2.auto_box.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_coords_test_2.auto_box.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_coords_test_2.auto_box.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_coords_test_2.auto_box.lv_obj, UI_CHILD_PANEL, &s_coords_test_2.auto_box.style);

    /* card (panel) */
    s_coords_test_2.card.lv_obj = lv_obj_create(s_coords_test_2.lv_screen);
    lv_obj_remove_style_all(s_coords_test_2.card.lv_obj);
    lv_obj_set_pos(s_coords_test_2.card.lv_obj, 160, 120);
    lv_obj_set_size(s_coords_test_2.card.lv_obj, 140, 70);
    lv_obj_clear_flag(s_coords_test_2.card.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_coords_test_2.card.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_coords_test_2.card.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_coords_test_2.card.lv_obj, UI_CHILD_PANEL, &s_coords_test_2.card.style);

    /* panel_grp (panel) */
    s_coords_test_2.panel_grp.lv_obj = lv_obj_create(s_coords_test_2.lv_screen);
    lv_obj_remove_style_all(s_coords_test_2.panel_grp.lv_obj);
    lv_obj_set_pos(s_coords_test_2.panel_grp.lv_obj, 30, 250);
    lv_obj_set_size(s_coords_test_2.panel_grp.lv_obj, 110, 54);
    lv_obj_clear_flag(s_coords_test_2.panel_grp.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_coords_test_2.panel_grp.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_coords_test_2.panel_grp.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    ui_apply_style(s_coords_test_2.panel_grp.lv_obj, UI_CHILD_PANEL, &s_coords_test_2.panel_grp.style);

    /* btn_overflow (button) */
    s_coords_test_2.btn_overflow.lv_obj = lv_button_create(s_coords_test_2.lv_screen);
    lv_obj_set_pos(s_coords_test_2.btn_overflow.lv_obj, 210, 315);
    lv_obj_set_size(s_coords_test_2.btn_overflow.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.btn_overflow.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.btn_overflow.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.btn_overflow.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.btn_overflow.lv_obj, ui_coords_test_2_on_btn_overflow_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.btn_overflow.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.btn_overflow.style);

    /* btn_rot (button) */
    s_coords_test_2.btn_rot.lv_obj = lv_button_create(s_coords_test_2.lv_screen);
    lv_obj_set_pos(s_coords_test_2.btn_rot.lv_obj, 60, 380);
    lv_obj_set_size(s_coords_test_2.btn_rot.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.btn_rot.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.btn_rot.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.btn_rot.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.btn_rot.lv_obj, ui_coords_test_2_on_btn_rot_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.btn_rot.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.btn_rot.style);

    /* btn_al1 (button) */
    s_coords_test_2.auto_box.btn_al1.lv_obj = lv_button_create(s_coords_test_2.auto_box.lv_obj);
    lv_obj_set_pos(s_coords_test_2.auto_box.btn_al1.lv_obj, 10, 10);
    lv_obj_set_size(s_coords_test_2.auto_box.btn_al1.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.auto_box.btn_al1.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.auto_box.btn_al1.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.auto_box.btn_al1.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.auto_box.btn_al1.lv_obj, ui_coords_test_2_on_btn_al1_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.auto_box.btn_al1.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.auto_box.btn_al1.style);

    /* btn_al2 (button) */
    s_coords_test_2.auto_box.btn_al2.lv_obj = lv_button_create(s_coords_test_2.auto_box.lv_obj);
    lv_obj_set_pos(s_coords_test_2.auto_box.btn_al2.lv_obj, 10, 40);
    lv_obj_set_size(s_coords_test_2.auto_box.btn_al2.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.auto_box.btn_al2.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.auto_box.btn_al2.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.auto_box.btn_al2.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.auto_box.btn_al2.lv_obj, ui_coords_test_2_on_btn_al2_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.auto_box.btn_al2.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.auto_box.btn_al2.style);

    /* btn_al3 (button) */
    s_coords_test_2.auto_box.btn_al3.lv_obj = lv_button_create(s_coords_test_2.auto_box.lv_obj);
    lv_obj_set_pos(s_coords_test_2.auto_box.btn_al3.lv_obj, 10, 70);
    lv_obj_set_size(s_coords_test_2.auto_box.btn_al3.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.auto_box.btn_al3.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.auto_box.btn_al3.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.auto_box.btn_al3.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.auto_box.btn_al3.lv_obj, ui_coords_test_2_on_btn_al3_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.auto_box.btn_al3.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.auto_box.btn_al3.style);

    /* btn_card (button) */
    s_coords_test_2.card.btn_card.lv_obj = lv_button_create(s_coords_test_2.card.lv_obj);
    lv_obj_set_pos(s_coords_test_2.card.btn_card.lv_obj, 30, 25);
    lv_obj_set_size(s_coords_test_2.card.btn_card.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.card.btn_card.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.card.btn_card.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.card.btn_card.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.card.btn_card.lv_obj, ui_coords_test_2_on_btn_card_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.card.btn_card.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.card.btn_card.style);

    /* btn_pg1 (button) */
    s_coords_test_2.panel_grp.btn_pg1.lv_obj = lv_button_create(s_coords_test_2.panel_grp.lv_obj);
    lv_obj_set_pos(s_coords_test_2.panel_grp.btn_pg1.lv_obj, 0, 0);
    lv_obj_set_size(s_coords_test_2.panel_grp.btn_pg1.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.panel_grp.btn_pg1.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.panel_grp.btn_pg1.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.panel_grp.btn_pg1.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.panel_grp.btn_pg1.lv_obj, ui_coords_test_2_on_btn_pg1_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.panel_grp.btn_pg1.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.panel_grp.btn_pg1.style);

    /* btn_pg2 (button) */
    s_coords_test_2.panel_grp.btn_pg2.lv_obj = lv_button_create(s_coords_test_2.panel_grp.lv_obj);
    lv_obj_set_pos(s_coords_test_2.panel_grp.btn_pg2.lv_obj, 60, 30);
    lv_obj_set_size(s_coords_test_2.panel_grp.btn_pg2.lv_obj, 50, 24);
    {
        lv_obj_t *lbl = lv_label_create(s_coords_test_2.panel_grp.btn_pg2.lv_obj);
        lv_label_set_text(lbl, s_coords_test_2.panel_grp.btn_pg2.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_coords_test_2.panel_grp.btn_pg2.style);
    }
    lv_obj_add_event_cb(s_coords_test_2.panel_grp.btn_pg2.lv_obj, ui_coords_test_2_on_btn_pg2_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_coords_test_2.panel_grp.btn_pg2.lv_obj, UI_CHILD_BUTTON, &s_coords_test_2.panel_grp.btn_pg2.style);
}
