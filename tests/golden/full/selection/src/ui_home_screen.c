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
        char        text[UI_TEXT_SIZE(13)];
    } label_ap_ssid;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(20)];
    } label_main;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } icon_wifi;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(10)];
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
    ui_copy_text(s_home_screen.label_ap_ssid.text, sizeof s_home_screen.label_ap_ssid.text, text);
    if (!s_home_screen.label_ap_ssid.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_screen_label_ap_ssid_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_home_screen.label_ap_ssid.lv_obj, s_home_screen.label_ap_ssid.text);
}

void ui_home_screen_label_main_set_text(const char *text)
{
    ui_copy_text(s_home_screen.label_main.text, sizeof s_home_screen.label_main.text, text);
    if (!s_home_screen.label_main.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_screen_label_main_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_home_screen.label_main.lv_obj, s_home_screen.label_main.text);
}

void ui_home_screen_icon_wifi_display(void)
{
    s_home_screen.icon_wifi.src = &icon_wifi;
    if (!s_home_screen.icon_wifi.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_screen_icon_wifi_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_home_screen.icon_wifi.lv_obj, s_home_screen.icon_wifi.src);
}

void ui_home_screen_label_discovery_set_text(const char *text)
{
    ui_copy_text(s_home_screen.label_discovery.text, sizeof s_home_screen.label_discovery.text, text);
    if (!s_home_screen.label_discovery.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_screen_label_discovery_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_home_screen.label_discovery.lv_obj, s_home_screen.label_discovery.text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_home_screen_load(void)
{
    if (!s_home_screen.lv_screen) {
        UI_LIFECYCLE_WARN("ui_home_screen_load: screen not built");
        return;
    }
    lv_scr_load(s_home_screen.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_home_screen_init(void)
{
    if (s_home_screen.lv_screen)
        return;
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

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_home_screen_deinit(void)
{
    if (!s_home_screen.lv_screen)
        return;
    if (lv_screen_active() == s_home_screen.lv_screen) {
        UI_LIFECYCLE_WARN("ui_home_screen_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_home_screen.lv_screen);   /* frees every child, callback and animation */
    s_home_screen.lv_screen = NULL;
    s_home_screen.label_ap_ssid.lv_obj = NULL;
    s_home_screen.label_main.lv_obj = NULL;
    s_home_screen.icon_wifi.lv_obj = NULL;
    s_home_screen.label_discovery.lv_obj = NULL;
}
