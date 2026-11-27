#ifndef UI_F_OPACITY_H
#define UI_F_OPACITY_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_opacity_init(void);
void ui_f_opacity_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_opacity_btn_opa_50_set_label(const char *text);
void ui_f_opacity_text_opa_50_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_f_opacity_on_btn_opa_50_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
