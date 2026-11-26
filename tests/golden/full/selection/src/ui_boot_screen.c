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
        char        text[UI_MAX_STRING_LENGTH];
    } label_main;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_channel_no;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
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
    if (s_boot_screen.label_main.lv_obj)
        lv_label_set_text(s_boot_screen.label_main.lv_obj, text);
}

void ui_boot_screen_label_channel_no_set_text(const char *text)
{
    if (s_boot_screen.label_channel_no.lv_obj)
        lv_label_set_text(s_boot_screen.label_channel_no.lv_obj, text);
}

void ui_boot_screen_label_discovery_set_text(const char *text)
{
    if (s_boot_screen.label_discovery.lv_obj)
        lv_label_set_text(s_boot_screen.label_discovery.lv_obj, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_boot_screen_load(void)
{
    lv_scr_load(s_boot_screen.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_boot_screen_init(void)
{
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
