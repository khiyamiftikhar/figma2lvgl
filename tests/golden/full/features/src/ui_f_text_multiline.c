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
        char        text[UI_TEXT_SIZE(29)];
    } lines;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(63)];
    } wrapped;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(14)];
    } centered;
} s_f_text_multiline = {
    .lines = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 16 }
        },
        .text = "First line\nSecond line\nThird",
    },
    .wrapped = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 14 }
        },
        .text = "This sentence is long enough to wrap inside a fixed width box.",
    },
    .centered = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 18, .has_align = true, .align = LV_TEXT_ALIGN_CENTER }
        },
        .text = "Centered\ntext",
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
    ui_copy_text(s_f_text_multiline.lines.text, sizeof s_f_text_multiline.lines.text, text);
    if (!s_f_text_multiline.lines.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_multiline_lines_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_multiline.lines.lv_obj, s_f_text_multiline.lines.text);
}

void ui_f_text_multiline_wrapped_set_text(const char *text)
{
    ui_copy_text(s_f_text_multiline.wrapped.text, sizeof s_f_text_multiline.wrapped.text, text);
    if (!s_f_text_multiline.wrapped.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_multiline_wrapped_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_multiline.wrapped.lv_obj, s_f_text_multiline.wrapped.text);
}

void ui_f_text_multiline_centered_set_text(const char *text)
{
    ui_copy_text(s_f_text_multiline.centered.text, sizeof s_f_text_multiline.centered.text, text);
    if (!s_f_text_multiline.centered.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_multiline_centered_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_multiline.centered.lv_obj, s_f_text_multiline.centered.text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_text_multiline_load(void)
{
    if (!s_f_text_multiline.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_text_multiline_load: screen not built");
        return;
    }
    lv_scr_load(s_f_text_multiline.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_text_multiline_init(void)
{
    if (s_f_text_multiline.lv_screen)
        return;
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
    lv_label_set_long_mode(s_f_text_multiline.wrapped.lv_obj, LV_LABEL_LONG_WRAP);
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

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_text_multiline_deinit(void)
{
    if (!s_f_text_multiline.lv_screen)
        return;
    if (lv_screen_active() == s_f_text_multiline.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_text_multiline_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_f_text_multiline.lv_screen);   /* frees every child, callback and animation */
    s_f_text_multiline.lv_screen = NULL;
    s_f_text_multiline.lines.lv_obj = NULL;
    s_f_text_multiline.wrapped.lv_obj = NULL;
    s_f_text_multiline.centered.lv_obj = NULL;
}
