#ifndef UI_F_IMAGES_H
#define UI_F_IMAGES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_images_init(void);
void ui_f_images_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_images_icon_star_display(void);
void ui_f_images_icon_star_2_display(void);
void ui_f_images_icon_heart_display(void);
void ui_f_images_panel_icons_icon_heart_display(void);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
