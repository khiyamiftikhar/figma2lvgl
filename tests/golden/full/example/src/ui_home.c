#include "ui_home.h"
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
        char        text[UI_TEXT_SIZE(14)];
    } time;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(20)];
    } welcome;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } icon_wifi;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } wifi_off;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        label_text[UI_TEXT_SIZE(5)];
    } btn_test;
} s_home = {
    .time = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 12 }
        },
        .text = "Time is\n15:00",
    },
    .welcome = {
        .style = {
            .text = { .has_color = true, .color = 0x5DED10, .has_size = true, .size = 12 }
        },
        .text = "Welcome to the test",
    },
    .icon_wifi = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x7AFF3C }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .wifi_off = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .btn_test = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFF686B, .has_radius = true, .radius = 20 },
            .text = { .has_color = true, .color = 0x3C1414, .has_size = true, .size = 12 }
        },
        .label_text = "Test",
    },
};

// ------------------------------
// BAR ANIMATION HELPER
// ------------------------------
static void bar_anim_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value((lv_obj_t *)obj, v, LV_ANIM_OFF);
}

// ------------------------------
// EVENT CALLBACKS
// Implement these in your application .c file.
// Linker will report undefined reference if not implemented.
// ------------------------------


// ------------------------------
// SETTERS
// ------------------------------
void ui_home_time_set_text(const char *text)
{
    ui_copy_text(s_home.time.text, sizeof s_home.time.text, text);
    if (!s_home.time.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_time_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_home.time.lv_obj, s_home.time.text);
}

void ui_home_welcome_set_text(const char *text)
{
    ui_copy_text(s_home.welcome.text, sizeof s_home.welcome.text, text);
    if (!s_home.welcome.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_welcome_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_home.welcome.lv_obj, s_home.welcome.text);
}

void ui_home_icon_wifi_display(void)
{
    s_home.icon_wifi.src = &icon_wifi;
    if (!s_home.icon_wifi.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_icon_wifi_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_home.icon_wifi.lv_obj, s_home.icon_wifi.src);
}

void ui_home_bar_set_value(int value, uint32_t duration_ms)
{
    s_home.bar.value = value;   /* the target: what a rebuild shows */
    if (!s_home.bar.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_bar_set_value: screen not built; value stored");
        return;
    }
    if (duration_ms == 0) {
        lv_bar_set_value(s_home.bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_home.bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_home.bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_home_wifi_off_display(void)
{
    s_home.wifi_off.src = &wifi_off;
    if (!s_home.wifi_off.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_wifi_off_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_home.wifi_off.lv_obj, s_home.wifi_off.src);
}

void ui_home_btn_test_set_label(const char *text)
{
    ui_copy_text(s_home.btn_test.label_text, sizeof s_home.btn_test.label_text, text);
    if (!s_home.btn_test.lv_obj) {
        UI_LIFECYCLE_WARN("ui_home_btn_test_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_home.btn_test.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_home.btn_test.label_text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_home_load(void)
{
    if (!s_home.lv_screen) {
        UI_LIFECYCLE_WARN("ui_home_load: screen not built");
        return;
    }
    lv_scr_load(s_home.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_home_init(void)
{
    if (s_home.lv_screen)
        return;
    s_home.lv_screen = lv_obj_create(NULL);

    /* time (label) */
    s_home.time.lv_obj = lv_label_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.time.lv_obj, 110, 246);
    lv_obj_set_width(s_home.time.lv_obj, 133);
    lv_label_set_long_mode(s_home.time.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_home.time.lv_obj, s_home.time.text);
    ui_apply_style(s_home.time.lv_obj, UI_CHILD_LABEL, &s_home.time.style);

    /* welcome (label) */
    s_home.welcome.lv_obj = lv_label_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.welcome.lv_obj, 88, 366);
    lv_obj_set_width(s_home.welcome.lv_obj, 165);
    lv_label_set_long_mode(s_home.welcome.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_home.welcome.lv_obj, s_home.welcome.text);
    ui_apply_style(s_home.welcome.lv_obj, UI_CHILD_LABEL, &s_home.welcome.style);

    /* icon_wifi (image) */
    s_home.icon_wifi.lv_obj = lv_image_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.icon_wifi.lv_obj, 19, 283);
    lv_obj_set_size(s_home.icon_wifi.lv_obj, 48, 48);
    if (s_home.icon_wifi.src)
        lv_image_set_src(s_home.icon_wifi.lv_obj, s_home.icon_wifi.src);
    ui_apply_style(s_home.icon_wifi.lv_obj, UI_CHILD_IMAGE, &s_home.icon_wifi.style);

    /* bar (bar) */
    s_home.bar.lv_obj = lv_bar_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.bar.lv_obj, 42, 77);
    lv_obj_set_size(s_home.bar.lv_obj, 241, 35);
    lv_bar_set_range(s_home.bar.lv_obj, s_home.bar.min, s_home.bar.max);
    lv_bar_set_value(s_home.bar.lv_obj, s_home.bar.value, LV_ANIM_OFF);
    ui_apply_style(s_home.bar.lv_obj, UI_CHILD_BAR, &s_home.bar.style);

    /* wifi_off (image) */
    s_home.wifi_off.lv_obj = lv_image_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.wifi_off.lv_obj, 237, 286);
    lv_obj_set_size(s_home.wifi_off.lv_obj, 48, 48);
    if (s_home.wifi_off.src)
        lv_image_set_src(s_home.wifi_off.lv_obj, s_home.wifi_off.src);
    ui_apply_style(s_home.wifi_off.lv_obj, UI_CHILD_IMAGE, &s_home.wifi_off.style);

    /* btn_test (button) */
    s_home.btn_test.lv_obj = lv_button_create(s_home.lv_screen);
    lv_obj_set_pos(s_home.btn_test.lv_obj, 127, 429);
    lv_obj_set_size(s_home.btn_test.lv_obj, 44, 37);
    {
        lv_obj_t *lbl = lv_label_create(s_home.btn_test.lv_obj);
        lv_label_set_text(lbl, s_home.btn_test.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_home.btn_test.style);
    }
    lv_obj_add_event_cb(s_home.btn_test.lv_obj, ui_home_on_btn_test_clicked,
                        LV_EVENT_SHORT_CLICKED, NULL);
    lv_obj_add_event_cb(s_home.btn_test.lv_obj, ui_home_on_btn_test_long_pressed,
                        LV_EVENT_LONG_PRESSED, NULL);
    ui_apply_style(s_home.btn_test.lv_obj, UI_CHILD_BUTTON, &s_home.btn_test.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_home_deinit(void)
{
    if (!s_home.lv_screen)
        return;
    if (lv_screen_active() == s_home.lv_screen) {
        UI_LIFECYCLE_WARN("ui_home_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_home.lv_screen);   /* frees every child, callback and animation */
    s_home.lv_screen = NULL;
    s_home.time.lv_obj = NULL;
    s_home.welcome.lv_obj = NULL;
    s_home.icon_wifi.lv_obj = NULL;
    s_home.bar.lv_obj = NULL;
    s_home.wifi_off.lv_obj = NULL;
    s_home.btn_test.lv_obj = NULL;
}
