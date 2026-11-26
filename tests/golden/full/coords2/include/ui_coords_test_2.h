#ifndef UI_COORDS_TEST_2_H
#define UI_COORDS_TEST_2_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_coords_test_2_init(void);
void ui_coords_test_2_load(void);

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
