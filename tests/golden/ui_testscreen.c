
#include "ui_testscreen.h"
#include "assets.h"
#include "ui_defs.h"
#include "ui_style.h"
#include <stdio.h>
// ------------------------------
// UI SCREEN STRUCTURE
// ------------------------------

    ui_screen_t testscreen = {
        .name = "TestScreen",
        .child_count = 2,
        .children = {
            
            {
                .type = UI_CHILD_LABEL,
                .id = "greeting",
                .lv_obj = NULL,
                .x = 10,
                .y = 20,
                .w = 100,
                .h = 30,
                .style = {
                .text = {
                    .has_color = true,
                    .color = 0xFFFFFF,
                    .has_size = true,
                    .size = 16
                }
            },
        
                .data.label = {
                    .text = "Hello"
                }
    
            },
        
            {
                .type = UI_CHILD_BAR,
                .id = "battery_bar",
                .lv_obj = NULL,
                .x = 10,
                .y = 100,
                .w = 200,
                .h = 20,
                .style = {
                .box = {
                    .has_bg = true,
                    .bg = 0x4CAF50
                }
            },
        
                .data.bar = {
                    .value = 0
                }
    
            },
        
        },
        .lv_screen = NULL
    };
    
// ------------------------------
// UI JOB DATA STRUCTS
// ------------------------------
//{job_structs}
// ------------------------------
// UI JOB CALLBACKS
// ------------------------------

static void ui_testscreen_bar_job_cb_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value(obj, v, LV_ANIM_OFF);
}

// ------------------------------
// UI SETTERS
// ------------------------------

void ui_testscreen_set_greeting(const char *text)
{
    ui_child_t *c = &testscreen.children[0];
    if (c->lv_obj) {
        lv_label_set_text(c->lv_obj, text);
    }
}


void ui_testscreen_set_battery_bar(int value, uint32_t duration_ms)
{
    ui_child_t *c = &testscreen.children[1];
    if (!c->lv_obj || c->type != UI_CHILD_BAR)
        return;
    if (duration_ms == 0)
    {
        lv_bar_set_value(c->lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, c->lv_obj);
    lv_anim_set_exec_cb(&a, ui_testscreen_bar_job_cb_exec_cb);
    lv_anim_set_values(&a, lv_bar_get_value(c->lv_obj), value);
    lv_anim_set_time(&a, duration_ms);
    lv_anim_start(&a);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_testscreen_load(void)
{
    lv_scr_load(testscreen.lv_screen);
}
// ------------------------------
// SCREEN INIT
// ------------------------------
/* TODO: Bar range is hardcoded to 0-100 in ui_testscreen_init() below.
 * Adjust lv_bar_set_range() for: battery_bar
 * If all bars share a range, consider making it a parameter. */
void ui_testscreen_init(void)
{
    testscreen.lv_screen = lv_obj_create(NULL);
    for (int i = 0; i < testscreen.child_count; i++)
    {
        ui_child_t *c = &testscreen.children[i];
        switch (c->type)
        {
            
    case UI_CHILD_LABEL:
        c->lv_obj = lv_label_create(testscreen.lv_screen);
        lv_obj_set_pos(c->lv_obj, c->x, c->y);
        lv_obj_set_width(c->lv_obj, c->w);
        lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
        lv_label_set_text(c->lv_obj, c->data.label.text);
        break;


    case UI_CHILD_BAR:
        c->lv_obj = lv_bar_create(testscreen.lv_screen);
        lv_obj_set_pos(c->lv_obj, c->x, c->y);
        lv_obj_set_size(c->lv_obj, c->w, c->h);
        lv_bar_set_range(c->lv_obj, 0, 100);
        lv_bar_set_value(c->lv_obj, c->data.bar.value, LV_ANIM_OFF);
        break;

            default:
                break;
        }

        /* apply styles — same call for every widget */
        if (c->lv_obj)
            ui_apply_style(c->lv_obj, c->type, &c->style);
    }
}
