#ifndef UI_F_ROTATED_H
#define UI_F_ROTATED_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_rotated_init(void);
void ui_f_rotated_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_rotated_btn_rotated_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_f_rotated_on_btn_rotated_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
