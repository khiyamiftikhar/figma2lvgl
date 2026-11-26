#ifndef UI_SETTINGS_H
#define UI_SETTINGS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_settings_init(void);
void ui_settings_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_settings_btn_ok_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_settings_on_btn_ok_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
