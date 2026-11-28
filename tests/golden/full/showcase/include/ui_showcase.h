#ifndef UI_SHOWCASE_H
#define UI_SHOWCASE_H
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
void ui_showcase_init(void);

/* Makes it the active screen. Does nothing if it isn't built. */
void ui_showcase_load(void);

/* Frees the screen's LVGL objects; its values are kept for the next init.
 * Load another screen first: it does nothing if this is the active screen.
 * Pointers from the accessors, and anything the application added to the
 * screen's objects (children, styles), are gone afterwards. */
void ui_showcase_deinit(void);

// ------------------------------
// SETTERS
// ------------------------------
void ui_showcase_btn_ok_set_label(const char *text);
void ui_showcase_btn_cancel_set_label(const char *text);
void ui_showcase_battery_bar_set_value(int value, uint32_t duration_ms);
void ui_showcase_slider_volume_set_value(int32_t value);
void ui_showcase_slider_level_set_value(int32_t value);
lv_obj_t *ui_showcase_get_list_devices(void);
void ui_showcase_btn_shadow_set_label(const char *text);
void ui_showcase_custom_font_set_text(const char *text);
void ui_showcase_btn_icon_set_label(const char *text);
void ui_showcase_panel_top_title_set_text(const char *text);
void ui_showcase_panel_top_icon_star_display(void);
void ui_showcase_panel_top_icon_star_hide(void);
void ui_showcase_panel_card_card_title_set_text(const char *text);
void ui_showcase_panel_card_card_body_set_text(const char *text);
void ui_showcase_panel_card_icon_heart_display(void);
void ui_showcase_panel_card_icon_heart_hide(void);
void ui_showcase_panel_row_btn_fill_a_set_label(const char *text);
void ui_showcase_panel_row_btn_fill_b_set_label(const char *text);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------
void ui_showcase_on_btn_ok_clicked(lv_event_t *e);
void ui_showcase_on_btn_cancel_clicked(lv_event_t *e);
void ui_showcase_on_btn_cancel_long_pressed(lv_event_t *e);
void ui_showcase_on_slider_volume(lv_event_t *e);
void ui_showcase_on_slider_level(lv_event_t *e);
void ui_showcase_on_btn_shadow_clicked(lv_event_t *e);
void ui_showcase_on_btn_icon_clicked(lv_event_t *e);
void ui_showcase_on_btn_fill_a_clicked(lv_event_t *e);
void ui_showcase_on_btn_fill_b_clicked(lv_event_t *e);

#ifdef __cplusplus
}
#endif
#endif
