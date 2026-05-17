#include "touch.h"

#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch_xpt2046.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

static const char *TAG = "touch";

// ----------------------------------------------------------------
// Pin definitions — must match sdkconfig.defaults / your wiring
// ----------------------------------------------------------------
#define TOUCH_SPI_HOST   SPI2_HOST       // same bus as ILI9486
#define TOUCH_PIN_CS     GPIO_NUM_25
#define TOUCH_PIN_IRQ    GPIO_NUM_27     // -1 to disable interrupt mode

// Screen resolution — reuse the display config values
#define TOUCH_H_RES      CONFIG_ILI9486_H_RES
#define TOUCH_V_RES      CONFIG_ILI9486_V_RES

// ----------------------------------------------------------------

esp_err_t touch_init(lv_display_t *disp)
{
    ESP_LOGI(TAG, "Initializing XPT2046 on SPI2, CS=GPIO%d, IRQ=GPIO%d",
             TOUCH_PIN_CS, TOUCH_PIN_IRQ);

    // ----------------------------------------------------------
    // 1. Create SPI IO for touch — shares bus with display,
    //    gets its own CS line.
    // ----------------------------------------------------------
    esp_lcd_panel_io_handle_t tp_io = NULL;
    esp_lcd_panel_io_spi_config_t tp_io_cfg =
        ESP_LCD_TOUCH_IO_SPI_XPT2046_CONFIG(TOUCH_PIN_CS);

    esp_err_t ret = esp_lcd_new_panel_io_spi(
        (esp_lcd_spi_bus_handle_t)TOUCH_SPI_HOST,
        &tp_io_cfg,
        &tp_io);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create touch SPI IO: %s", esp_err_to_name(ret));
        return ret;
    }

    // ----------------------------------------------------------
    // 2. Configure and create the XPT2046 touch handle.
    //
    //    swap_xy / mirror_x / mirror_y: leave at 0 for now.
    //    If touches are rotated/mirrored after first test,
    //    flip these and re-flash — no hardware change needed.
    // ----------------------------------------------------------
    esp_lcd_touch_config_t tp_cfg = {
        .x_max        = TOUCH_H_RES,
        .y_max        = TOUCH_V_RES,
        .rst_gpio_num = GPIO_NUM_NC,    // no RST pin wired
        .int_gpio_num = GPIO_NUM_NC,  // GPIO_NUM_NC to poll instead
        .flags = {
            .swap_xy  = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };

    esp_lcd_touch_handle_t tp = NULL;
    ret = esp_lcd_touch_new_spi_xpt2046(tp_io, &tp_cfg, &tp);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create XPT2046 touch driver: %s", esp_err_to_name(ret));
        return ret;
    }

    // ----------------------------------------------------------
    // 3. Register with esp_lvgl_port — it owns the read loop
    //    from here, no manual esp_lcd_touch_read_data() needed.
    // ----------------------------------------------------------
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp   = disp,
        .handle = tp,
    };

    lv_indev_t *indev = lvgl_port_add_touch(&touch_cfg);
    if (indev == NULL) {
        ESP_LOGE(TAG, "Failed to register touch with LVGL port");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Touch ready");
    return ESP_OK;
}