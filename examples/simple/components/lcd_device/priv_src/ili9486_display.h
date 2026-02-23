#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_ili9486_panel.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t ili9486_display_init(void);

esp_lcd_panel_handle_t ili9486_display_get_panel(void);
#ifdef __cplusplus
}
#endif