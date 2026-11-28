#ifndef UI_COORDS_TEST_2_H
#define UI_COORDS_TEST_2_H
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
void ui_coords_test_2_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_coords_test_2_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_coords_test_2_deinit(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_coords_test_2_btn_g2_set_label(const char *text);
void ui_coords_test_2_btn_f2_set_label(const char *text);
void ui_coords_test_2_btn_mix_set_label(const char *text);
void ui_coords_test_2_btn_overflow_set_label(const char *text);
void ui_coords_test_2_btn_rot_set_label(const char *text);
void ui_coords_test_2_auto_box_btn_al1_set_label(const char *text);
void ui_coords_test_2_auto_box_btn_al2_set_label(const char *text);
void ui_coords_test_2_auto_box_btn_al3_set_label(const char *text);
void ui_coords_test_2_card_btn_card_set_label(const char *text);
void ui_coords_test_2_panel_grp_btn_pg1_set_label(const char *text);
void ui_coords_test_2_panel_grp_btn_pg2_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_coords_test_2_on_btn_g2_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_f2_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_mix_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_overflow_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_rot_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_al1_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_al2_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_al3_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_card_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_pg1_clicked(lv_event_t *e);
void ui_coords_test_2_on_btn_pg2_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
