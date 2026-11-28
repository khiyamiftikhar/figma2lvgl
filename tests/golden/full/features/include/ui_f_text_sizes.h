#ifndef UI_F_TEXT_SIZES_H
#define UI_F_TEXT_SIZES_H
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
void ui_f_text_sizes_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_f_text_sizes_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_f_text_sizes_deinit(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_f_text_sizes_size_8_set_text(const char *text);
void ui_f_text_sizes_size_10_set_text(const char *text);
void ui_f_text_sizes_size_12_set_text(const char *text);
void ui_f_text_sizes_size_14_set_text(const char *text);
void ui_f_text_sizes_size_16_set_text(const char *text);
void ui_f_text_sizes_size_18_set_text(const char *text);
void ui_f_text_sizes_size_20_set_text(const char *text);
void ui_f_text_sizes_size_22_set_text(const char *text);
void ui_f_text_sizes_size_24_set_text(const char *text);
void ui_f_text_sizes_size_26_set_text(const char *text);
void ui_f_text_sizes_size_28_set_text(const char *text);
void ui_f_text_sizes_size_30_set_text(const char *text);
void ui_f_text_sizes_size_32_set_text(const char *text);
void ui_f_text_sizes_size_34_set_text(const char *text);
void ui_f_text_sizes_size_36_set_text(const char *text);
void ui_f_text_sizes_size_38_set_text(const char *text);
void ui_f_text_sizes_size_40_set_text(const char *text);
void ui_f_text_sizes_size_42_set_text(const char *text);
void ui_f_text_sizes_size_44_set_text(const char *text);
void ui_f_text_sizes_size_46_set_text(const char *text);
void ui_f_text_sizes_size_48_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
