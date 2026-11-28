#ifndef UI_HOSTILE_H
#define UI_HOSTILE_H
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
void ui_hostile_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_hostile_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_hostile_deinit(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_hostile_label_2nd_row_set_text(const char *text);
void ui_hostile_label_default_set_text(const char *text);
void ui_hostile_label_class_set_text(const char *text);
void ui_hostile_label_1_set_text(const char *text);
void ui_hostile_label_2_set_text(const char *text);
void ui_hostile_label_style_set_text(const char *text);
void ui_hostile_label_lv_obj_set_text(const char *text);
void ui_hostile_title_set_text(const char *text);
void ui_hostile_title_3_set_text(const char *text);
void ui_hostile_title_2_set_text(const char *text);
void ui_hostile_title_4_set_text(const char *text);
void ui_hostile_ui_style_display(void);
void ui_hostile_icon_wifi_display(void);
void ui_hostile_icon_wifi_2_display(void);
void ui_hostile_panel_a_btn_ok_set_label(const char *text);
void ui_hostile_panel_a_title_set_text(const char *text);
void ui_hostile_panel_a_label_text_set_text(const char *text);
void ui_hostile_panel_b_btn_ok_set_label(const char *text);
void ui_hostile_panel_d_e_label_set_text(const char *text);
void ui_hostile_panel_d_e_label_set_text_2(const char *text);
void ui_hostile_x_panel_c_btn_go_set_label(const char *text);
void ui_hostile_y_panel_c_btn_go_set_label(const char *text);
void ui_hostile_a_b_c_deep_set_text(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_hostile_on_panel_a_btn_ok_clicked(lv_event_t *e);
void ui_hostile_on_panel_b_btn_ok_clicked(lv_event_t *e);
void ui_hostile_on_x_panel_c_btn_go_clicked(lv_event_t *e);
void ui_hostile_on_y_panel_c_btn_go_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
