#ifndef UI_HOME_H
#define UI_HOME_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_home_init(void);
void ui_home_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_home_time_set_text(const char *text);
void ui_home_welcome_set_text(const char *text);
void ui_home_icon_wifi_display(void);
void ui_home_bar_set_value(int value, uint32_t duration_ms);
void ui_home_wifi_off_display(void);
void ui_home_btn_test_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_home_on_btn_test_clicked(lv_event_t *e);
void ui_home_on_btn_test_long_pressed(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
