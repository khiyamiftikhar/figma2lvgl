#ifndef UI_STYLE_3_H
#define UI_STYLE_3_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_style_3_init(void);
void ui_style_3_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_style_3_caption_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
