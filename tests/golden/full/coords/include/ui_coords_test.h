#ifndef UI_COORDS_TEST_H
#define UI_COORDS_TEST_H
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
void ui_coords_test_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_coords_test_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_coords_test_deinit(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_coords_test_btn_a_set_label(const char *text);
void ui_coords_test_btn_b_set_label(const char *text);
void ui_coords_test_btn_c_set_label(const char *text);
void ui_coords_test_btn_d_set_label(const char *text);
void ui_coords_test_btn_ghost_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_coords_test_on_btn_a_clicked(lv_event_t *e);
void ui_coords_test_on_btn_b_clicked(lv_event_t *e);
void ui_coords_test_on_btn_c_clicked(lv_event_t *e);
void ui_coords_test_on_btn_d_clicked(lv_event_t *e);
void ui_coords_test_on_btn_ghost_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
