#include "ui_f_sliders.h"
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
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_h;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_h_2;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_v;
} s_f_sliders = {
    .slider_h = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .slider_h_2 = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 10,
    },
    .slider_v = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
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
void ui_f_sliders_slider_h_set_value(int32_t value)
{
    if (s_f_sliders.slider_h.lv_obj)
        lv_slider_set_value(s_f_sliders.slider_h.lv_obj, value, LV_ANIM_OFF);
}

void ui_f_sliders_slider_h_2_set_value(int32_t value)
{
    if (s_f_sliders.slider_h_2.lv_obj)
        lv_slider_set_value(s_f_sliders.slider_h_2.lv_obj, value, LV_ANIM_OFF);
}

void ui_f_sliders_slider_v_set_value(int32_t value)
{
    if (s_f_sliders.slider_v.lv_obj)
        lv_slider_set_value(s_f_sliders.slider_v.lv_obj, value, LV_ANIM_OFF);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_sliders_load(void)
{
    lv_scr_load(s_f_sliders.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_f_sliders_init(void)
{
    s_f_sliders.lv_screen = lv_obj_create(NULL);

    /* slider_h (slider) */
    s_f_sliders.slider_h.lv_obj = lv_slider_create(s_f_sliders.lv_screen);
    lv_obj_set_pos(s_f_sliders.slider_h.lv_obj, 20, 30);
    lv_obj_set_size(s_f_sliders.slider_h.lv_obj, 220, 16);
    lv_slider_set_range(s_f_sliders.slider_h.lv_obj, s_f_sliders.slider_h.min, s_f_sliders.slider_h.max);
    lv_slider_set_value(s_f_sliders.slider_h.lv_obj, s_f_sliders.slider_h.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_f_sliders.slider_h.lv_obj, ui_f_sliders_on_slider_h,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_f_sliders.slider_h.lv_obj, UI_CHILD_SLIDER, &s_f_sliders.slider_h.style);

    /* slider_h_2 (slider) */
    s_f_sliders.slider_h_2.lv_obj = lv_slider_create(s_f_sliders.lv_screen);
    lv_obj_set_pos(s_f_sliders.slider_h_2.lv_obj, 20, 80);
    lv_obj_set_size(s_f_sliders.slider_h_2.lv_obj, 220, 16);
    lv_slider_set_range(s_f_sliders.slider_h_2.lv_obj, s_f_sliders.slider_h_2.min, s_f_sliders.slider_h_2.max);
    lv_slider_set_value(s_f_sliders.slider_h_2.lv_obj, s_f_sliders.slider_h_2.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_f_sliders.slider_h_2.lv_obj, ui_f_sliders_on_slider_h_2,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_f_sliders.slider_h_2.lv_obj, UI_CHILD_SLIDER, &s_f_sliders.slider_h_2.style);

    /* slider_v (slider) */
    s_f_sliders.slider_v.lv_obj = lv_slider_create(s_f_sliders.lv_screen);
    lv_obj_set_pos(s_f_sliders.slider_v.lv_obj, 270, 20);
    lv_obj_set_size(s_f_sliders.slider_v.lv_obj, 16, 200);
    lv_slider_set_range(s_f_sliders.slider_v.lv_obj, s_f_sliders.slider_v.min, s_f_sliders.slider_v.max);
    lv_slider_set_value(s_f_sliders.slider_v.lv_obj, s_f_sliders.slider_v.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_f_sliders.slider_v.lv_obj, ui_f_sliders_on_slider_v,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_f_sliders.slider_v.lv_obj, UI_CHILD_SLIDER, &s_f_sliders.slider_v.style);
}
