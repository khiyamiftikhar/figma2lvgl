#ifndef UI_F_TINY_H
#define UI_F_TINY_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_tiny_init(void);
void ui_f_tiny_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_tiny_btn_4x4_set_label(const char *text);
void ui_f_tiny_btn_1x1_set_label(const char *text);
void ui_f_tiny_slider_tiny_set_value(int32_t value);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_f_tiny_on_btn_4x4_clicked(lv_event_t *e);
void ui_f_tiny_on_btn_1x1_clicked(lv_event_t *e);
void ui_f_tiny_on_slider_tiny(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
