// ─── ili9486_panel.h ────────────────────────────────────────────────────────
#pragma once
#include "esp_lcd_types.h"
#include "esp_err.h"
#include "esp_lcd_types.h"
#include "esp_lcd_panel_ops.h"        // ← esp_lcd_panel_handle_t
#include "esp_lcd_panel_vendor.h"     // ← esp_lcd_panel_dev_config_t  ✓

esp_err_t esp_lcd_new_panel_ili9486(esp_lcd_panel_io_handle_t io,
                                    const esp_lcd_panel_dev_config_t *panel_dev_config,
                                    esp_lcd_panel_handle_t *ret_panel);