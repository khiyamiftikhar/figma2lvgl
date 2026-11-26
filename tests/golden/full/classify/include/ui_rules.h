#ifndef UI_RULES_H
#define UI_RULES_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_rules_init(void);
void ui_rules_load(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_rules_caption_set_text(const char *text);
void ui_rules_btn_ok_set_label(const char *text);
void ui_rules_btn_next_set_label(const char *text);
void ui_rules_btnback_set_label(const char *text);
void ui_rules_btn_press_set_label(const char *text);
void ui_rules_btn_save_set_label(const char *text);
void ui_rules_slider_vol_set_value(int32_t value);
void ui_rules_volume_slider_set_value(int32_t value);
void ui_rules_brightness_slider_set_value(int32_t value);
void ui_rules_slider_rev_set_value(int32_t value);
void ui_rules_slider_flat_set_value(int32_t value);
lv_obj_t *ui_rules_get_list_items(void);
void ui_rules_battery_bar_set_value(int value, uint32_t duration_ms);
void ui_rules_batterybar_set_value(int value, uint32_t duration_ms);
void ui_rules_icon_wifi_display(void);
void ui_rules_wifi_off_display(void);
void ui_rules_btn_inner_set_label(const char *text);
void ui_rules_btn_slider_set_label(const char *text);
void ui_rules_button_icon_set_label(const char *text);
void ui_rules_bar_volume_slider_set_value(int32_t value);
void ui_rules_volume_slider_bar_set_value(int value, uint32_t duration_ms);
lv_obj_t *ui_rules_get_list_icon(void);
lv_obj_t *ui_rules_get_grid_bar(void);
void ui_rules_bar_icon_set_value(int value, uint32_t duration_ms);
void ui_rules_icon_bar_set_value(int value, uint32_t duration_ms);
void ui_rules_slider_panel_set_value(int32_t value);
void ui_rules_time_click_set_text(const char *text);
void ui_rules_header_title_set_text(const char *text);
void ui_rules_box_with_hidden_child_btn_shown_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_rules_on_btn_ok_clicked(lv_event_t *e);
void ui_rules_on_btn_next_clicked(lv_event_t *e);
void ui_rules_on_btnback_clicked(lv_event_t *e);
void ui_rules_on_btn_press_clicked(lv_event_t *e);
void ui_rules_on_btn_save_clicked(lv_event_t *e);
void ui_rules_on_btn_save_long_pressed(lv_event_t *e);
void ui_rules_on_slider_vol(lv_event_t *e);
void ui_rules_on_volume_slider(lv_event_t *e);
void ui_rules_on_brightness_slider(lv_event_t *e);
void ui_rules_on_slider_rev(lv_event_t *e);
void ui_rules_on_slider_flat(lv_event_t *e);
void ui_rules_on_btn_inner_clicked(lv_event_t *e);
void ui_rules_on_btn_slider_clicked(lv_event_t *e);
void ui_rules_on_button_icon_clicked(lv_event_t *e);
void ui_rules_on_bar_volume_slider(lv_event_t *e);
void ui_rules_on_slider_panel(lv_event_t *e);
void ui_rules_on_btn_shown_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
