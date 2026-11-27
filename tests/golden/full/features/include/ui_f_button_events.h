#ifndef UI_F_BUTTON_EVENTS_H
#define UI_F_BUTTON_EVENTS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_button_events_init(void);
void ui_f_button_events_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_button_events_btn_tap_set_label(const char *text);
void ui_f_button_events_btn_hold_set_label(const char *text);
void ui_f_button_events_btn_repeat_set_label(const char *text);
void ui_f_button_events_btn_down_set_label(const char *text);
void ui_f_button_events_btn_up_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_f_button_events_on_btn_tap_clicked(lv_event_t *e);
void ui_f_button_events_on_btn_hold_clicked(lv_event_t *e);
void ui_f_button_events_on_btn_hold_long_pressed(lv_event_t *e);
void ui_f_button_events_on_btn_repeat_clicked(lv_event_t *e);
void ui_f_button_events_on_btn_repeat_long_pressed_repeat(lv_event_t *e);
void ui_f_button_events_on_btn_down_clicked(lv_event_t *e);
void ui_f_button_events_on_btn_down_pressed(lv_event_t *e);
void ui_f_button_events_on_btn_up_clicked(lv_event_t *e);
void ui_f_button_events_on_btn_up_released(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
