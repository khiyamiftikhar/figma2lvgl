#include "ui_f_text_multiline.h"
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
    } lines;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } wrapped;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } centered;
} s_f_text_multiline = {
    .lines = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 16 }
        },
        .text = "First line Second line Third",
    },
    .wrapped = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 14 }
        },
        .text = "This sentence is long enough ",
    },
    .centered = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 18 }
        },
        .text = "Centered text",
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
void ui_f_text_multiline_lines_set_text(const char *text)
{
    if (s_f_text_multiline.lines.lv_obj)
        lv_label_set_text(s_f_text_multiline.lines.lv_obj, text);
}

void ui_f_text_multiline_wrapped_set_text(const char *text)
{
    if (s_f_text_multiline.wrapped.lv_obj)
        lv_label_set_text(s_f_text_multiline.wrapped.lv_obj, text);
}

void ui_f_text_multiline_centered_set_text(const char *text)
{
    if (s_f_text_multiline.centered.lv_obj)
        lv_label_set_text(s_f_text_multiline.centered.lv_obj, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_text_multiline_load(void)
{
    lv_scr_load(s_f_text_multiline.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_f_text_multiline_init(void)
{
    s_f_text_multiline.lv_screen = lv_obj_create(NULL);

    /* lines (label) */
    s_f_text_multiline.lines.lv_obj = lv_label_create(s_f_text_multiline.lv_screen);
    lv_obj_set_pos(s_f_text_multiline.lines.lv_obj, 10, 10);
    lv_obj_set_width(s_f_text_multiline.lines.lv_obj, 94);
    lv_label_set_long_mode(s_f_text_multiline.lines.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_multiline.lines.lv_obj, s_f_text_multiline.lines.text);
    ui_apply_style(s_f_text_multiline.lines.lv_obj, UI_CHILD_LABEL, &s_f_text_multiline.lines.style);

    /* wrapped (label) */
    s_f_text_multiline.wrapped.lv_obj = lv_label_create(s_f_text_multiline.lv_screen);
    lv_obj_set_pos(s_f_text_multiline.wrapped.lv_obj, 10, 100);
    lv_obj_set_width(s_f_text_multiline.wrapped.lv_obj, 180);
    lv_label_set_long_mode(s_f_text_multiline.wrapped.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_multiline.wrapped.lv_obj, s_f_text_multiline.wrapped.text);
    ui_apply_style(s_f_text_multiline.wrapped.lv_obj, UI_CHILD_LABEL, &s_f_text_multiline.wrapped.style);

    /* centered (label) */
    s_f_text_multiline.centered.lv_obj = lv_label_create(s_f_text_multiline.lv_screen);
    lv_obj_set_pos(s_f_text_multiline.centered.lv_obj, 10, 190);
    lv_obj_set_width(s_f_text_multiline.centered.lv_obj, 300);
    lv_label_set_long_mode(s_f_text_multiline.centered.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_multiline.centered.lv_obj, s_f_text_multiline.centered.text);
    ui_apply_style(s_f_text_multiline.centered.lv_obj, UI_CHILD_LABEL, &s_f_text_multiline.centered.style);
}
