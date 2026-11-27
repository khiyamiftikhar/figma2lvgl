#ifndef UI_F_SLIDERS_H
#define UI_F_SLIDERS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_sliders_init(void);
void ui_f_sliders_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_sliders_slider_h_set_value(int32_t value);
void ui_f_sliders_slider_h_2_set_value(int32_t value);
void ui_f_sliders_slider_v_set_value(int32_t value);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_f_sliders_on_slider_h(lv_event_t *e);
void ui_f_sliders_on_slider_h_2(lv_event_t *e);
void ui_f_sliders_on_slider_v(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
