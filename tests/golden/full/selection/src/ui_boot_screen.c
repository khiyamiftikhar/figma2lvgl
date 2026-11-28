#include "ui_boot_screen.h"
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
        char        text[UI_TEXT_SIZE(19)];
    } label_main;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(11)];
    } label_channel_no;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(10)];
    } label_discovery;
} s_boot_screen = {
    .label_main = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "Scanning Home node",
    },
    .label_channel_no = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 8 }
        },
        .text = "Channel no",
    },
    .label_discovery = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 8 }
        },
        .text = "Discovery",
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
void ui_boot_screen_label_main_set_text(const char *text)
{
    ui_copy_text(s_boot_screen.label_main.text, sizeof s_boot_screen.label_main.text, text);
    if (!s_boot_screen.label_main.lv_obj) {
        UI_LIFECYCLE_WARN("ui_boot_screen_label_main_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_boot_screen.label_main.lv_obj, s_boot_screen.label_main.text);
}

void ui_boot_screen_label_channel_no_set_text(const char *text)
{
    ui_copy_text(s_boot_screen.label_channel_no.text, sizeof s_boot_screen.label_channel_no.text, text);
    if (!s_boot_screen.label_channel_no.lv_obj) {
        UI_LIFECYCLE_WARN("ui_boot_screen_label_channel_no_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_boot_screen.label_channel_no.lv_obj, s_boot_screen.label_channel_no.text);
}

void ui_boot_screen_label_discovery_set_text(const char *text)
{
    ui_copy_text(s_boot_screen.label_discovery.text, sizeof s_boot_screen.label_discovery.text, text);
    if (!s_boot_screen.label_discovery.lv_obj) {
        UI_LIFECYCLE_WARN("ui_boot_screen_label_discovery_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_boot_screen.label_discovery.lv_obj, s_boot_screen.label_discovery.text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_boot_screen_load(void)
{
    if (!s_boot_screen.lv_screen) {
        UI_LIFECYCLE_WARN("ui_boot_screen_load: screen not built");
        return;
    }
    lv_scr_load(s_boot_screen.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_boot_screen_init(void)
{
    if (s_boot_screen.lv_screen)
        return;
    s_boot_screen.lv_screen = lv_obj_create(NULL);

    /* label_main (label) */
    s_boot_screen.label_main.lv_obj = lv_label_create(s_boot_screen.lv_screen);
    lv_obj_set_pos(s_boot_screen.label_main.lv_obj, 8, 14);
    lv_obj_set_width(s_boot_screen.label_main.lv_obj, 112);
    lv_label_set_long_mode(s_boot_screen.label_main.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_boot_screen.label_main.lv_obj, s_boot_screen.label_main.text);
    ui_apply_style(s_boot_screen.label_main.lv_obj, UI_CHILD_LABEL, &s_boot_screen.label_main.style);

    /* label_channel_no (label) */
    s_boot_screen.label_channel_no.lv_obj = lv_label_create(s_boot_screen.lv_screen);
    lv_obj_set_pos(s_boot_screen.label_channel_no.lv_obj, 55, 2);
    lv_obj_set_width(s_boot_screen.label_channel_no.lv_obj, 55);
    lv_label_set_long_mode(s_boot_screen.label_channel_no.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_boot_screen.label_channel_no.lv_obj, s_boot_screen.label_channel_no.text);
    ui_apply_style(s_boot_screen.label_channel_no.lv_obj, UI_CHILD_LABEL, &s_boot_screen.label_channel_no.style);

    /* label_discovery (label) */
    s_boot_screen.label_discovery.lv_obj = lv_label_create(s_boot_screen.lv_screen);
    lv_obj_set_pos(s_boot_screen.label_discovery.lv_obj, 3, 2);
    lv_obj_set_width(s_boot_screen.label_discovery.lv_obj, 38);
    lv_label_set_long_mode(s_boot_screen.label_discovery.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_boot_screen.label_discovery.lv_obj, s_boot_screen.label_discovery.text);
    ui_apply_style(s_boot_screen.label_discovery.lv_obj, UI_CHILD_LABEL, &s_boot_screen.label_discovery.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_boot_screen_deinit(void)
{
    if (!s_boot_screen.lv_screen)
        return;
    if (lv_screen_active() == s_boot_screen.lv_screen) {
        UI_LIFECYCLE_WARN("ui_boot_screen_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_boot_screen.lv_screen);   /* frees every child, callback and animation */
    s_boot_screen.lv_screen = NULL;
    s_boot_screen.label_main.lv_obj = NULL;
    s_boot_screen.label_channel_no.lv_obj = NULL;
    s_boot_screen.label_discovery.lv_obj = NULL;
}
