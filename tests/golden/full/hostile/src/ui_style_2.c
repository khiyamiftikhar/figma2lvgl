#include "ui_style_2.h"
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
        char        text[UI_MAX_STRING_LENGTH];
    } caption;
} s_style_2 = {
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
void ui_style_2_caption_set_text(const char *text)
{
    if (s_style_2.caption.lv_obj)
        lv_label_set_text(s_style_2.caption.lv_obj, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_style_2_load(void)
{
    lv_scr_load(s_style_2.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_style_2_init(void)
{
    s_style_2.lv_screen = lv_obj_create(NULL);

    /* caption (label) */
    s_style_2.caption.lv_obj = lv_label_create(s_style_2.lv_screen);
    lv_obj_set_pos(s_style_2.caption.lv_obj, 4, 4);
    lv_obj_set_width(s_style_2.caption.lv_obj, 60);
    lv_label_set_long_mode(s_style_2.caption.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_style_2.caption.lv_obj, s_style_2.caption.text);
    ui_apply_style(s_style_2.caption.lv_obj, UI_CHILD_LABEL, &s_style_2.caption.style);
}
