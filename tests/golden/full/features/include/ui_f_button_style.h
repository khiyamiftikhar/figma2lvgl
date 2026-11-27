#ifndef UI_F_BUTTON_STYLE_H
#define UI_F_BUTTON_STYLE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_button_style_init(void);
void ui_f_button_style_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_button_style_btn_square_set_label(const char *text);
void ui_f_button_style_btn_round_set_label(const char *text);
void ui_f_button_style_btn_pill_set_label(const char *text);
void ui_f_button_style_btn_stroke_set_label(const char *text);
void ui_f_button_style_btn_both_set_label(const char *text);
void ui_f_button_style_btn_thick_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_f_button_style_on_btn_square_clicked(lv_event_t *e);
void ui_f_button_style_on_btn_round_clicked(lv_event_t *e);
void ui_f_button_style_on_btn_pill_clicked(lv_event_t *e);
void ui_f_button_style_on_btn_stroke_clicked(lv_event_t *e);
void ui_f_button_style_on_btn_both_clicked(lv_event_t *e);
void ui_f_button_style_on_btn_thick_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
