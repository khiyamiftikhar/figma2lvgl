#ifndef UI_COORDS_TEST_H
#define UI_COORDS_TEST_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_coords_test_init(void);
void ui_coords_test_load(void);

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
