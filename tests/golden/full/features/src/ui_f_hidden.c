#include "ui_f_hidden.h"
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
    } btn_visible;
} s_f_hidden = {
    .btn_visible = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Visible",
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
void ui_f_hidden_btn_visible_set_label(const char *text)
{
    if (!s_f_hidden.btn_visible.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_f_hidden.btn_visible.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_hidden_load(void)
{
    lv_scr_load(s_f_hidden.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_f_hidden_init(void)
{
    s_f_hidden.lv_screen = lv_obj_create(NULL);

    /* btn_visible (button) */
    s_f_hidden.btn_visible.lv_obj = lv_button_create(s_f_hidden.lv_screen);
    lv_obj_set_pos(s_f_hidden.btn_visible.lv_obj, 10, 150);
    lv_obj_set_size(s_f_hidden.btn_visible.lv_obj, 120, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_hidden.btn_visible.lv_obj);
        lv_label_set_text(lbl, s_f_hidden.btn_visible.label_text);
        lv_obj_center(lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(lbl, UI_CHILD_LABEL, &s_f_hidden.btn_visible.style);
    }
    lv_obj_add_event_cb(s_f_hidden.btn_visible.lv_obj, ui_f_hidden_on_btn_visible_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_hidden.btn_visible.lv_obj, UI_CHILD_BUTTON, &s_f_hidden.btn_visible.style);
}
