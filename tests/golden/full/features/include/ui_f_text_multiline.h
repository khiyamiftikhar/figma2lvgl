#ifndef UI_F_TEXT_MULTILINE_H
#define UI_F_TEXT_MULTILINE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_text_multiline_init(void);
void ui_f_text_multiline_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_text_multiline_lines_set_text(const char *text);
void ui_f_text_multiline_wrapped_set_text(const char *text);
void ui_f_text_multiline_centered_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
