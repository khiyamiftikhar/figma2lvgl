#ifndef UI_HOME_SCREEN_H
#define UI_HOME_SCREEN_H
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
void ui_home_screen_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_home_screen_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_home_screen_deinit(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_home_screen_label_ap_ssid_set_text(const char *text);
void ui_home_screen_label_main_set_text(const char *text);
void ui_home_screen_icon_wifi_display(void);
void ui_home_screen_icon_wifi_hide(void);
void ui_home_screen_label_discovery_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
