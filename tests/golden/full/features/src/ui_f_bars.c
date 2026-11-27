#include "ui_f_bars.h"
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
    } level_bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } volume_bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } drain_bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } temp_bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } tall_bar;
} s_f_bars = {
    .level_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x4CAF50 }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .volume_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .value = 0,
        .min   = 0,
        .max   = 255,
    },
    .drain_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFF9800 }
        },
        .value = 0,
        .min   = 100,
        .max   = 0,
    },
    .temp_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE91E63 }
        },
        .value = 0,
        .min   = -20,
        .max   = 50,
    },
    .tall_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x795548 }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
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
void ui_f_bars_level_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_f_bars.level_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_f_bars.level_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_f_bars.level_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_f_bars.level_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_f_bars_volume_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_f_bars.volume_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_f_bars.volume_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_f_bars.volume_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_f_bars.volume_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_f_bars_drain_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_f_bars.drain_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_f_bars.drain_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_f_bars.drain_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_f_bars.drain_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_f_bars_temp_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_f_bars.temp_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_f_bars.temp_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_f_bars.temp_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_f_bars.temp_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_f_bars_tall_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_f_bars.tall_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_f_bars.tall_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_f_bars.tall_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_f_bars.tall_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_bars_load(void)
{
    lv_scr_load(s_f_bars.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_f_bars_init(void)
{
    s_f_bars.lv_screen = lv_obj_create(NULL);

    /* level_bar (bar) */
    s_f_bars.level_bar.lv_obj = lv_bar_create(s_f_bars.lv_screen);
    lv_obj_set_pos(s_f_bars.level_bar.lv_obj, 10, 20);
    lv_obj_set_size(s_f_bars.level_bar.lv_obj, 300, 20);
    lv_bar_set_range(s_f_bars.level_bar.lv_obj, s_f_bars.level_bar.min, s_f_bars.level_bar.max);
    lv_bar_set_value(s_f_bars.level_bar.lv_obj, s_f_bars.level_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_f_bars.level_bar.lv_obj, UI_CHILD_BAR, &s_f_bars.level_bar.style);

    /* volume_bar (bar) */
    s_f_bars.volume_bar.lv_obj = lv_bar_create(s_f_bars.lv_screen);
    lv_obj_set_pos(s_f_bars.volume_bar.lv_obj, 10, 60);
    lv_obj_set_size(s_f_bars.volume_bar.lv_obj, 300, 20);
    lv_bar_set_range(s_f_bars.volume_bar.lv_obj, s_f_bars.volume_bar.min, s_f_bars.volume_bar.max);
    lv_bar_set_value(s_f_bars.volume_bar.lv_obj, s_f_bars.volume_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_f_bars.volume_bar.lv_obj, UI_CHILD_BAR, &s_f_bars.volume_bar.style);

    /* drain_bar (bar) */
    s_f_bars.drain_bar.lv_obj = lv_bar_create(s_f_bars.lv_screen);
    lv_obj_set_pos(s_f_bars.drain_bar.lv_obj, 10, 100);
    lv_obj_set_size(s_f_bars.drain_bar.lv_obj, 300, 20);
    lv_bar_set_range(s_f_bars.drain_bar.lv_obj, s_f_bars.drain_bar.min, s_f_bars.drain_bar.max);
    lv_bar_set_value(s_f_bars.drain_bar.lv_obj, s_f_bars.drain_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_f_bars.drain_bar.lv_obj, UI_CHILD_BAR, &s_f_bars.drain_bar.style);

    /* temp_bar (bar) */
    s_f_bars.temp_bar.lv_obj = lv_bar_create(s_f_bars.lv_screen);
    lv_obj_set_pos(s_f_bars.temp_bar.lv_obj, 10, 140);
    lv_obj_set_size(s_f_bars.temp_bar.lv_obj, 300, 20);
    lv_bar_set_range(s_f_bars.temp_bar.lv_obj, s_f_bars.temp_bar.min, s_f_bars.temp_bar.max);
    lv_bar_set_value(s_f_bars.temp_bar.lv_obj, s_f_bars.temp_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_f_bars.temp_bar.lv_obj, UI_CHILD_BAR, &s_f_bars.temp_bar.style);

    /* tall_bar (bar) */
    s_f_bars.tall_bar.lv_obj = lv_bar_create(s_f_bars.lv_screen);
    lv_obj_set_pos(s_f_bars.tall_bar.lv_obj, 10, 180);
    lv_obj_set_size(s_f_bars.tall_bar.lv_obj, 20, 50);
    lv_bar_set_range(s_f_bars.tall_bar.lv_obj, s_f_bars.tall_bar.min, s_f_bars.tall_bar.max);
    lv_bar_set_value(s_f_bars.tall_bar.lv_obj, s_f_bars.tall_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_f_bars.tall_bar.lv_obj, UI_CHILD_BAR, &s_f_bars.tall_bar.style);
}
