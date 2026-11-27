#ifndef UI_EVENTS_H
#define UI_EVENTS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_events_init(void);
void ui_events_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_events_btn_plain_set_label(const char *text);
void ui_events_btn_a_set_label(const char *text);
void ui_events_btn_b_set_label(const char *text);
void ui_events_btn_c_set_label(const char *text);
void ui_events_btn_d_set_label(const char *text);
void ui_events_slider_h_set_value(int32_t value);
void ui_events_slider_v_set_value(int32_t value);
void ui_events_slider_rev_set_value(int32_t value);
void ui_events_slider_vrev_set_value(int32_t value);
void ui_events_btn_under_panel_set_label(const char *text);
void ui_events_btn_under_button_set_label(const char *text);
void ui_events_btn_over_button_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_events_on_btn_plain_clicked(lv_event_t *e);
void ui_events_on_btn_a_clicked(lv_event_t *e);
void ui_events_on_btn_a_pressed(lv_event_t *e);
void ui_events_on_btn_b_clicked(lv_event_t *e);
void ui_events_on_btn_b_released(lv_event_t *e);
void ui_events_on_btn_c_clicked(lv_event_t *e);
void ui_events_on_btn_c_long_pressed(lv_event_t *e);
void ui_events_on_btn_d_clicked(lv_event_t *e);
void ui_events_on_btn_d_long_pressed_repeat(lv_event_t *e);
void ui_events_on_slider_h(lv_event_t *e);
void ui_events_on_slider_v(lv_event_t *e);
void ui_events_on_slider_rev(lv_event_t *e);
void ui_events_on_slider_vrev(lv_event_t *e);
void ui_events_on_btn_under_panel_clicked(lv_event_t *e);
void ui_events_on_btn_under_button_clicked(lv_event_t *e);
void ui_events_on_btn_over_button_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
