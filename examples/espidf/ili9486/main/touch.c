#include "touch.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch_xpt2046.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

static const char *TAG = "touch";

#define TOUCH_SPI_HOST  SPI2_HOST
#define TOUCH_PIN_CS    GPIO_NUM_25
#define TOUCH_H_RES     CONFIG_ILI9486_H_RES
#define TOUCH_V_RES     CONFIG_ILI9486_V_RES

static esp_lcd_touch_handle_t s_tp = NULL;

esp_lcd_touch_handle_t touch_get_handle(void) { return s_tp; }

esp_err_t touch_init(lv_display_t *disp)
{
    ESP_LOGI(TAG, "Initializing XPT2046 on SPI2, CS=GPIO%d", TOUCH_PIN_CS);

    // 1. SPI IO handle for touch
    esp_lcd_panel_io_handle_t tp_io = NULL;
    esp_lcd_panel_io_spi_config_t tp_io_cfg =
        ESP_LCD_TOUCH_IO_SPI_XPT2046_CONFIG(TOUCH_PIN_CS);

    esp_err_t ret = esp_lcd_new_panel_io_spi(
        (esp_lcd_spi_bus_handle_t)TOUCH_SPI_HOST,
        &tp_io_cfg,
        &tp_io);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI IO failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // 2. XPT2046 touch config
    esp_lcd_touch_config_t tp_cfg = {
        .x_max        = TOUCH_H_RES,
        .y_max        = TOUCH_V_RES,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = GPIO_NUM_NC,   // polling mode
        .flags = {
            .swap_xy  = 0,
            .mirror_x = 0,
            .mirror_y = 1,
        },
    };

    ret = esp_lcd_touch_new_spi_xpt2046(tp_io, &tp_cfg, &s_tp);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "XPT2046 init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    // inside touch_init(), after esp_lcd_touch_new_spi_xpt2046 succeeds
    // and BEFORE lvgl_port_add_touch()
    ESP_LOGI(TAG, "Raw read test — press screen");
    for (int i = 0; i < 30; i++) {
        esp_lcd_touch_read_data(s_tp);
        esp_lcd_touch_point_data_t points[1];
        uint8_t count = 0;
        esp_lcd_touch_get_data(s_tp, points, &count, 1);
        if (count > 0) {
            ESP_LOGI(TAG, "TOUCH x=%d y=%d strength=%d",
                    points[0].x, points[0].y, points[0].strength);
        } else {
            ESP_LOGI(TAG, "no touch");
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
// then lvgl_port_add_touch() after
    // 3. Register with LVGL port
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp   = disp,
        .handle = s_tp,
    };

    lv_indev_t *indev = lvgl_port_add_touch(&touch_cfg);
    if (indev == NULL) {
        ESP_LOGE(TAG, "Failed to register touch with LVGL port");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Touch ready");
    return ESP_OK;
}