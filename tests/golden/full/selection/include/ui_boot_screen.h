#ifndef UI_BOOT_SCREEN_H
#define UI_BOOT_SCREEN_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_boot_screen_init(void);
void ui_boot_screen_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_boot_screen_label_main_set_text(const char *text);
void ui_boot_screen_label_channel_no_set_text(const char *text);
void ui_boot_screen_label_discovery_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
