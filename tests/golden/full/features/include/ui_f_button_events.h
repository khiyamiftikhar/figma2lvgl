#ifndef UI_F_BUTTON_EVENTS_H
#define UI_F_BUTTON_EVENTS_H
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
void ui_f_button_events_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_f_button_events_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_f_button_events_deinit(void);

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
