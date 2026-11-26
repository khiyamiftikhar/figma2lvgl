#include "ui_settings.h"
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
    } btn_ok;
} s_settings = {
    .btn_ok = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3, .has_radius = true, .radius = 8 }
        },
        .label_text = "Ok",
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
void ui_settings_btn_ok_set_label(const char *text)
{
    if (!s_settings.btn_ok.lv_obj) return;
    lv_obj_t *_lbl = lv_obj_get_child(s_settings.btn_ok.lv_obj, 0);
    if (_lbl) lv_label_set_text(_lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_settings_load(void)
{
    lv_scr_load(s_settings.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_settings_init(void)
{
    s_settings.lv_screen = lv_obj_create(NULL);

    /* btn_ok (button) */
    s_settings.btn_ok.lv_obj = lv_button_create(s_settings.lv_screen);
    lv_obj_set_pos(s_settings.btn_ok.lv_obj, 100, 380);
    lv_obj_set_size(s_settings.btn_ok.lv_obj, 120, 44);
    {
        lv_obj_t *_lbl = lv_label_create(s_settings.btn_ok.lv_obj);
        lv_label_set_text(_lbl, s_settings.btn_ok.label_text);
        lv_obj_center(_lbl);
        /* text color/font applied to the label directly, not the container */
        ui_apply_style(_lbl, UI_CHILD_LABEL, &s_settings.btn_ok.style);
    }
    lv_obj_add_event_cb(s_settings.btn_ok.lv_obj, ui_settings_on_btn_ok_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_settings.btn_ok.lv_obj, UI_CHILD_BUTTON, &s_settings.btn_ok.style);
}
