#pragma once

#include "esp_err.h"
#include "lvgl.h"
#include "esp_lcd_touch.h"   // <-- add this

/**
 * @brief Initialize XPT2046 touch controller and register it with LVGL.
 *
 * Must be called AFTER display_init() and AFTER esp_lvgl_port is running.
 *
 * @param disp  The lv_display_t* returned by your display driver
 *              (from display_get_lvgl_handle() or similar).
 * @return ESP_OK on success
 */
esp_err_t touch_init(lv_display_t *disp);
esp_lcd_touch_handle_t touch_get_handle(void);