#include "ui_testscreen.h"
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
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } time;
    } panel_top;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_ok;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
    } battery_bar;
} s_testscreen = {
    .panel_top = {
        .time = {
            .text = "16:30",
        },
    },
    .btn_ok = {
        .label_text = "Ok",
    },
};

// ------------------------------
// BAR ANIMATION HELPER
// ------------------------------
static void _bar_anim_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value((lv_obj_t *)obj, v, LV_ANIM_OFF);
}

// ------------------------------
// EVENT CALLBACKS
// (override in your application .c)
// ------------------------------
__attribute__((weak)) void ui_testscreen_on_btn_ok(lv_event_t *e)
{
    (void)e;
    /* override: navigate, update state, etc. */
}

// ------------------------------
// SETTERS
// ------------------------------
void ui_testscreen_btn_ok_set_label(const char *text)
{
    if (!s_testscreen.btn_ok.lv_obj) return;
    lv_obj_t *_lbl = lv_obj_get_child(s_testscreen.btn_ok.lv_obj, 0);
    if (_lbl) lv_label_set_text(_lbl, text);
}

void ui_testscreen_battery_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_testscreen.battery_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_testscreen.battery_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t _a;
    lv_anim_init(&_a);
    lv_anim_set_var(&_a, s_testscreen.battery_bar.lv_obj);
    lv_anim_set_exec_cb(&_a, _bar_anim_exec_cb);
    lv_anim_set_values(&_a, lv_bar_get_value(s_testscreen.battery_bar.lv_obj), value);
    lv_anim_set_time(&_a, duration_ms);
    lv_anim_start(&_a);
}

void ui_testscreen_panel_top_time_set_text(const char *text)
{
    if (s_testscreen.panel_top.time.lv_obj)
        lv_label_set_text(s_testscreen.panel_top.time.lv_obj, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_testscreen_load(void)
{
    lv_scr_load(s_testscreen.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_testscreen_init(void)
{
    s_testscreen.lv_screen = lv_obj_create(NULL);

    /* panel_top (panel) */
    s_testscreen.panel_top.lv_obj = lv_obj_create(s_testscreen.lv_screen);
    lv_obj_set_pos(s_testscreen.panel_top.lv_obj, 0, 0);
    lv_obj_set_size(s_testscreen.panel_top.lv_obj, 320, 80);
    lv_obj_clear_flag(s_testscreen.panel_top.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_testscreen.panel_top.lv_obj, LV_SCROLLBAR_MODE_OFF);
    ui_apply_style(s_testscreen.panel_top.lv_obj, UI_CHILD_PANEL, &s_testscreen.panel_top.style);

    /* btn_ok (button) */
    s_testscreen.btn_ok.lv_obj = lv_button_create(s_testscreen.lv_screen);
    lv_obj_set_pos(s_testscreen.btn_ok.lv_obj, 100, 380);
    lv_obj_set_size(s_testscreen.btn_ok.lv_obj, 120, 44);
    {
        lv_obj_t *_lbl = lv_label_create(s_testscreen.btn_ok.lv_obj);
        lv_label_set_text(_lbl, s_testscreen.btn_ok.label_text);
        lv_obj_center(_lbl);
    }
    lv_obj_add_event_cb(s_testscreen.btn_ok.lv_obj, ui_testscreen_on_btn_ok,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_testscreen.btn_ok.lv_obj, UI_CHILD_BUTTON, &s_testscreen.btn_ok.style);

    /* battery_bar (bar) */
    s_testscreen.battery_bar.lv_obj = lv_bar_create(s_testscreen.lv_screen);
    lv_obj_set_pos(s_testscreen.battery_bar.lv_obj, 10, 300);
    lv_obj_set_size(s_testscreen.battery_bar.lv_obj, 200, 20);
    lv_bar_set_range(s_testscreen.battery_bar.lv_obj, 0, 100); /* TODO: adjust range if needed */
    lv_bar_set_value(s_testscreen.battery_bar.lv_obj, s_testscreen.battery_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_testscreen.battery_bar.lv_obj, UI_CHILD_BAR, &s_testscreen.battery_bar.style);

    /* time (label) */
    s_testscreen.panel_top.time.lv_obj = lv_label_create(s_testscreen.panel_top.lv_obj);
    lv_obj_set_pos(s_testscreen.panel_top.time.lv_obj, 10, 20);
    lv_obj_set_width(s_testscreen.panel_top.time.lv_obj, 100);
    lv_label_set_long_mode(s_testscreen.panel_top.time.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_testscreen.panel_top.time.lv_obj, s_testscreen.panel_top.time.text);
    ui_apply_style(s_testscreen.panel_top.time.lv_obj, UI_CHILD_LABEL, &s_testscreen.panel_top.time.style);
}
