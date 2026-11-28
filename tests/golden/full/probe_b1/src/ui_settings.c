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
        char        label_text[UI_TEXT_SIZE(3)];
    } btn_ok;
} s_settings = {
    .btn_ok = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3, .has_radius = true, .radius = 8 },
            .text = { .has_color = true, .color = 0xFFFFFF }
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
    ui_copy_text(s_settings.btn_ok.label_text, sizeof s_settings.btn_ok.label_text, text);
    if (!s_settings.btn_ok.lv_obj) {
        UI_LIFECYCLE_WARN("ui_settings_btn_ok_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_settings.btn_ok.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_settings.btn_ok.label_text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_settings_load(void)
{
    if (!s_settings.lv_screen) {
        UI_LIFECYCLE_WARN("ui_settings_load: screen not built");
        return;
    }
    lv_scr_load(s_settings.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_settings_init(void)
{
    if (s_settings.lv_screen)
        return;
    s_settings.lv_screen = lv_obj_create(NULL);

    /* btn_ok (button) */
    s_settings.btn_ok.lv_obj = lv_button_create(s_settings.lv_screen);
    lv_obj_set_pos(s_settings.btn_ok.lv_obj, 100, 380);
    lv_obj_set_size(s_settings.btn_ok.lv_obj, 120, 44);
    {
        lv_obj_t *lbl = lv_label_create(s_settings.btn_ok.lv_obj);
        lv_label_set_text(lbl, s_settings.btn_ok.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_settings.btn_ok.style);
    }
    lv_obj_add_event_cb(s_settings.btn_ok.lv_obj, ui_settings_on_btn_ok_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_settings.btn_ok.lv_obj, UI_CHILD_BUTTON, &s_settings.btn_ok.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_settings_deinit(void)
{
    if (!s_settings.lv_screen)
        return;
    if (lv_screen_active() == s_settings.lv_screen) {
        UI_LIFECYCLE_WARN("ui_settings_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_settings.lv_screen);   /* frees every child, callback and animation */
    s_settings.lv_screen = NULL;
    s_settings.btn_ok.lv_obj = NULL;
}
