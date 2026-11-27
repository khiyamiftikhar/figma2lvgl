#ifndef UI_F_BARS_H
#define UI_F_BARS_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_bars_init(void);
void ui_f_bars_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_bars_level_bar_set_value(int value, uint32_t duration_ms);
void ui_f_bars_volume_bar_set_value(int value, uint32_t duration_ms);
void ui_f_bars_drain_bar_set_value(int value, uint32_t duration_ms);
void ui_f_bars_temp_bar_set_value(int value, uint32_t duration_ms);
void ui_f_bars_tall_bar_set_value(int value, uint32_t duration_ms);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
