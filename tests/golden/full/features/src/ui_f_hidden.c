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
        char        label_text[UI_TEXT_SIZE(8)];
    } btn_visible;
} s_f_hidden = {
    .btn_visible = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
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
    ui_copy_text(s_f_hidden.btn_visible.label_text, sizeof s_f_hidden.btn_visible.label_text, text);
    if (!s_f_hidden.btn_visible.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_hidden_btn_visible_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_f_hidden.btn_visible.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_f_hidden.btn_visible.label_text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_hidden_load(void)
{
    if (!s_f_hidden.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_hidden_load: screen not built");
        return;
    }
    lv_scr_load(s_f_hidden.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_hidden_init(void)
{
    if (s_f_hidden.lv_screen)
        return;
    s_f_hidden.lv_screen = lv_obj_create(NULL);

    /* btn_visible (button) */
    s_f_hidden.btn_visible.lv_obj = lv_button_create(s_f_hidden.lv_screen);
    lv_obj_set_pos(s_f_hidden.btn_visible.lv_obj, 10, 150);
    lv_obj_set_size(s_f_hidden.btn_visible.lv_obj, 120, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_f_hidden.btn_visible.lv_obj);
        lv_label_set_text(lbl, s_f_hidden.btn_visible.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_f_hidden.btn_visible.style);
    }
    lv_obj_add_event_cb(s_f_hidden.btn_visible.lv_obj, ui_f_hidden_on_btn_visible_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_f_hidden.btn_visible.lv_obj, UI_CHILD_BUTTON, &s_f_hidden.btn_visible.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_hidden_deinit(void)
{
    if (!s_f_hidden.lv_screen)
        return;
    if (lv_screen_active() == s_f_hidden.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_hidden_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_f_hidden.lv_screen);   /* frees every child, callback and animation */
    s_f_hidden.lv_screen = NULL;
    s_f_hidden.btn_visible.lv_obj = NULL;
}
