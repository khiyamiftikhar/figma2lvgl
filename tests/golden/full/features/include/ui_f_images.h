#ifndef UI_F_IMAGES_H
#define UI_F_IMAGES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// The screen's values live in RAM whether or not it is built: setters store
// them, init builds the screen from them, deinit frees the LVGL objects and
// keeps them. Call these from the thread that runs LVGL, or with LVGL locked.
// ------------------------------

/* Builds the screen from its values. Does nothing if it is already built. */
void ui_f_images_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_f_images_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_f_images_deinit(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_images_icon_star_display(void);
void ui_f_images_icon_star_hide(void);
void ui_f_images_icon_star_2_display(void);
void ui_f_images_icon_star_2_hide(void);
void ui_f_images_icon_heart_display(void);
void ui_f_images_icon_heart_hide(void);
void ui_f_images_panel_icons_icon_heart_display(void);
void ui_f_images_panel_icons_icon_heart_hide(void);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
