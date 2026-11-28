#ifndef UI_TESTSCREEN_H
#define UI_TESTSCREEN_H
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
void ui_testscreen_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_testscreen_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_testscreen_deinit(void);

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
