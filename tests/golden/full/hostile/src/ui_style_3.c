#include "ui_style_3.h"
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
        char        text[UI_TEXT_SIZE(2)];
    } caption;
} s_style_3 = {
    .caption = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
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
void ui_style_3_caption_set_text(const char *text)
{
    ui_copy_text(s_style_3.caption.text, sizeof s_style_3.caption.text, text);
    if (!s_style_3.caption.lv_obj) {
        UI_LIFECYCLE_WARN("ui_style_3_caption_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_style_3.caption.lv_obj, s_style_3.caption.text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_style_3_load(void)
{
    if (!s_style_3.lv_screen) {
        UI_LIFECYCLE_WARN("ui_style_3_load: screen not built");
        return;
    }
    lv_scr_load(s_style_3.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_style_3_init(void)
{
    if (s_style_3.lv_screen)
        return;
    s_style_3.lv_screen = lv_obj_create(NULL);

    /* caption (label) */
    s_style_3.caption.lv_obj = lv_label_create(s_style_3.lv_screen);
    lv_obj_set_pos(s_style_3.caption.lv_obj, 4, 4);
    lv_obj_set_width(s_style_3.caption.lv_obj, 60);
    lv_label_set_long_mode(s_style_3.caption.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_style_3.caption.lv_obj, s_style_3.caption.text);
    ui_apply_style(s_style_3.caption.lv_obj, UI_CHILD_LABEL, &s_style_3.caption.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_style_3_deinit(void)
{
    if (!s_style_3.lv_screen)
        return;
    if (lv_screen_active() == s_style_3.lv_screen) {
        UI_LIFECYCLE_WARN("ui_style_3_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_style_3.lv_screen);   /* frees every child, callback and animation */
    s_style_3.lv_screen = NULL;
    s_style_3.caption.lv_obj = NULL;
}
