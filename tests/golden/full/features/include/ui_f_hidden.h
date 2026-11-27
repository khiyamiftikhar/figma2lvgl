#ifndef UI_F_HIDDEN_H
#define UI_F_HIDDEN_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_hidden_init(void);
void ui_f_hidden_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_hidden_btn_visible_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_f_hidden_on_btn_visible_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
