#ifndef UI_STYLE_2_H
#define UI_STYLE_2_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_style_2_init(void);
void ui_style_2_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_style_2_caption_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
