#ifndef UI_F_COVERING_H
#define UI_F_COVERING_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_covering_init(void);
void ui_f_covering_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_covering_btn_under_panel_set_label(const char *text);
void ui_f_covering_btn_under_button_set_label(const char *text);
void ui_f_covering_btn_on_top_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_f_covering_on_btn_under_panel_clicked(lv_event_t *e);
void ui_f_covering_on_btn_under_button_clicked(lv_event_t *e);
void ui_f_covering_on_btn_on_top_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
