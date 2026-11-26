#ifndef UI_HOME_SCREEN_H
#define UI_HOME_SCREEN_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_home_screen_init(void);
void ui_home_screen_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_home_screen_label_ap_ssid_set_text(const char *text);
void ui_home_screen_label_main_set_text(const char *text);
void ui_home_screen_icon_wifi_display(void);
void ui_home_screen_label_discovery_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
