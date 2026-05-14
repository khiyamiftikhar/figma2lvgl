#ifndef UI_TESTSCREEN_H
#define UI_TESTSCREEN_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_testscreen_init(void);
void ui_testscreen_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_testscreen_btn_ok_set_label(const char *text);
void ui_testscreen_battery_bar_set_value(int value, uint32_t duration_ms);
void ui_testscreen_panel_top_time_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_testscreen_on_btn_ok_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
