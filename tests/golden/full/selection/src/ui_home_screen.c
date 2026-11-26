#include "ui_home_screen.h"
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
    } label_ap_ssid;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_main;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } icon_wifi;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_discovery;
} s_home_screen = {
    .label_ap_ssid = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 8 }
        },
        .text = "WIFI AP SSID",
    },
    .label_main = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "Home Screen Message",
    },
    .icon_wifi = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
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
void ui_home_screen_label_ap_ssid_set_text(const char *text)
{
    if (s_home_screen.label_ap_ssid.lv_obj)
        lv_label_set_text(s_home_screen.label_ap_ssid.lv_obj, text);
}

void ui_home_screen_label_main_set_text(const char *text)
{
    if (s_home_screen.label_main.lv_obj)
        lv_label_set_text(s_home_screen.label_main.lv_obj, text);
}

void ui_home_screen_icon_wifi_display(void)
{
    if (!s_home_screen.icon_wifi.lv_obj) return;
    s_home_screen.icon_wifi.src = &icon_wifi;
    lv_image_set_src(s_home_screen.icon_wifi.lv_obj, s_home_screen.icon_wifi.src);
}

void ui_home_screen_label_discovery_set_text(const char *text)
{
    if (s_home_screen.label_discovery.lv_obj)
        lv_label_set_text(s_home_screen.label_discovery.lv_obj, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_home_screen_load(void)
{
    lv_scr_load(s_home_screen.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_home_screen_init(void)
{
    s_home_screen.lv_screen = lv_obj_create(NULL);

    /* label_ap_ssid (label) */
    s_home_screen.label_ap_ssid.lv_obj = lv_label_create(s_home_screen.lv_screen);
    lv_obj_set_pos(s_home_screen.label_ap_ssid.lv_obj, 50, 2);
    lv_obj_set_width(s_home_screen.label_ap_ssid.lv_obj, 55);
    lv_label_set_long_mode(s_home_screen.label_ap_ssid.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_home_screen.label_ap_ssid.lv_obj, s_home_screen.label_ap_ssid.text);
    ui_apply_style(s_home_screen.label_ap_ssid.lv_obj, UI_CHILD_LABEL, &s_home_screen.label_ap_ssid.style);

    /* label_main (label) */
    s_home_screen.label_main.lv_obj = lv_label_create(s_home_screen.lv_screen);
    lv_obj_set_pos(s_home_screen.label_main.lv_obj, 8, 14);
    lv_obj_set_width(s_home_screen.label_main.lv_obj, 112);
    lv_label_set_long_mode(s_home_screen.label_main.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_home_screen.label_main.lv_obj, s_home_screen.label_main.text);
    ui_apply_style(s_home_screen.label_main.lv_obj, UI_CHILD_LABEL, &s_home_screen.label_main.style);

    /* icon_wifi (image) */
    s_home_screen.icon_wifi.lv_obj = lv_image_create(s_home_screen.lv_screen);
    lv_obj_set_pos(s_home_screen.icon_wifi.lv_obj, 108, -1);
    lv_obj_set_size(s_home_screen.icon_wifi.lv_obj, 14, 14);
    if (s_home_screen.icon_wifi.src)
        lv_image_set_src(s_home_screen.icon_wifi.lv_obj, s_home_screen.icon_wifi.src);
    ui_apply_style(s_home_screen.icon_wifi.lv_obj, UI_CHILD_IMAGE, &s_home_screen.icon_wifi.style);

    /* label_discovery (label) */
    s_home_screen.label_discovery.lv_obj = lv_label_create(s_home_screen.lv_screen);
    lv_obj_set_pos(s_home_screen.label_discovery.lv_obj, 2, 2);
    lv_obj_set_width(s_home_screen.label_discovery.lv_obj, 38);
    lv_label_set_long_mode(s_home_screen.label_discovery.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_home_screen.label_discovery.lv_obj, s_home_screen.label_discovery.text);
    ui_apply_style(s_home_screen.label_discovery.lv_obj, UI_CHILD_LABEL, &s_home_screen.label_discovery.style);
}
