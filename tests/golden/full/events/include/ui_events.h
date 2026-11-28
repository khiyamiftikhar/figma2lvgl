#ifndef UI_EVENTS_H
#define UI_EVENTS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// The screen's values live in RAM whether or not it is built: setters store
// them, init builds the screen from them, deinit frees the LVGL objects and
// keeps them. Call these from the thread that runs LVGL, or with LVGL locked.
// ------------------------------

/* Builds the screen from its values. Does nothing if it is already built. */
void ui_events_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_events_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_events_deinit(void);

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
