#include "ui_f_rotated.h"
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
    } box_rotated;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(7)];
    } btn_rotated;
} s_f_rotated = {
    .box_rotated = {
        .style = {
            .box = { .has_bg = true, .bg = 0xC0CA33 }
        },
    },
    .btn_rotated = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Tilted",
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
void ui_f_rotated_btn_rotated_set_label(const char *text)
{
    ui_copy_text(s_f_rotated.btn_rotated.label_text, sizeof s_f_rotated.btn_rotated.label_text, text);
    if (!s_f_rotated.btn_rotated.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_rotated_btn_rotated_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_f_rotated.btn_rotated.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_f_rotated.btn_rotated.label_text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_rotated_load(void)
{
    if (!s_f_rotated.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_rotated_load: screen not built");
        return;
    }
    lv_scr_load(s_f_rotated.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_rotated_init(void)
{
    if (s_f_rotated.lv_screen)
        return;
    s_f_rotated.lv_screen = lv_obj_create(NULL);

    /* box_rotated (panel) */
    s_f_rotated.box_rotated.lv_obj = lv_obj_create(s_f_rotated.lv_screen);
    lv_obj_remove_style_all(s_f_rotated.box_rotated.lv_obj);
    lv_obj_set_pos(s_f_rotated.box_rotated.lv_obj, 60, 40);
    lv_obj_set_size(s_f_rotated.box_rotated.lv_obj, 120, 40);
    lv_obj_clear_flag(s_f_rotated.box_rotated.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_rotated.box_rotated.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_f_rotated.box_rotated.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_f_rotated.box_rotated.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_rotated.box_rotated.lv_obj, UI_CHILD_PANEL, &s_f_rotated.box_rotated.style);

    /* btn_rotated (button) */
    s_f_rotated.btn_rotated.lv_obj = lv_button_create(s_f_rotated.lv_screen);
    lv_obj_set_pos(s_f_rotated.btn_rotated.lv_obj, 160, 140);
    lv_obj_set_size(s_f_rotated.btn_rotated.lv_obj, 120, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_rotated.btn_rotated.lv_obj);
        lv_label_set_text(lbl, s_f_rotated.btn_rotated.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_rotated.btn_rotated.style);
    }
    lv_obj_add_event_cb(s_f_rotated.btn_rotated.lv_obj, ui_f_rotated_on_btn_rotated_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_rotated.btn_rotated.lv_obj, UI_CHILD_BUTTON, &s_f_rotated.btn_rotated.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_rotated_deinit(void)
{
    if (!s_f_rotated.lv_screen)
        return;
    if (lv_screen_active() == s_f_rotated.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_rotated_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_f_rotated.lv_screen);   /* frees every child, callback and animation */
    s_f_rotated.lv_screen = NULL;
    s_f_rotated.box_rotated.lv_obj = NULL;
    s_f_rotated.btn_rotated.lv_obj = NULL;
}
