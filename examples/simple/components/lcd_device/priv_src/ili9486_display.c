#include "display.h"

#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"

#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_lcd_panel_io.h"
#include "esp_lvgl_port.h"
#include "lvgl.h"

static const char *TAG = "ili9486_display";

/* ------------------------- */
/* ---- USER CONFIG AREA --- */
/* ------------------------- */

#define LCD_HOST           SPI2_HOST

#define PIN_NUM_MOSI       23
#define PIN_NUM_MISO       -1
#define PIN_NUM_CLK        18
#define PIN_NUM_CS         5
#define PIN_NUM_DC         21
#define PIN_NUM_RST        22
#define PIN_NUM_BK_LIGHT   4

#define LCD_PIXEL_CLOCK_HZ (5 * 1000 * 1000)

#define LCD_H_RES          320
#define LCD_V_RES          480

/* ------------------------- */

// ─── ili9486_display.c ──────────────────────────────────────────────────────
#include "esp_ili9486_panel.h"

static esp_lcd_panel_io_handle_t s_io_handle = NULL;
static esp_lcd_panel_handle_t   s_panel      = NULL;

esp_err_t ili9486_display_init(void)
{
    esp_err_t ret;

    /* ── SPI bus ───────────────────────────────────────────────────────────── */
    ESP_LOGI(TAG, "Initialize SPI bus");
    spi_bus_config_t buscfg = {
        .mosi_io_num     = PIN_NUM_MOSI,
        .miso_io_num     = PIN_NUM_MISO,
        .sclk_io_num     = PIN_NUM_CLK,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = LCD_H_RES * 80 * sizeof(uint16_t),
    };
    ESP_RETURN_ON_ERROR(
        spi_bus_initialize(LCD_HOST, &buscfg, SPI_DMA_CH_AUTO),
        TAG, "SPI bus init failed");

    /* ── Panel IO ──────────────────────────────────────────────────────────── */
    ESP_LOGI(TAG, "Install panel IO");
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num      = PIN_NUM_DC,
        .cs_gpio_num      = PIN_NUM_CS,
        .pclk_hz          = LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits     = 8,
        .lcd_param_bits   = 8,
        .spi_mode         = 0,
        .trans_queue_depth = 10,
    };
    ESP_RETURN_ON_ERROR(
        esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_HOST,
                                  &io_config, &s_io_handle),
        TAG, "Panel IO init failed");

    /* ── Backlight GPIO ────────────────────────────────────────────────────── */
    gpio_config_t bk_conf = {
        .mode         = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << PIN_NUM_BK_LIGHT,
    };
    gpio_config(&bk_conf);
    gpio_set_level(PIN_NUM_BK_LIGHT, 0);   // keep off during init

    /* ── Create ILI9486 panel (reset pin handled inside) ──────────────────── */
    ESP_LOGI(TAG, "Install ILI9486 panel driver");
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num  = PIN_NUM_RST,
        .bits_per_pixel  = 16,
    };
    ESP_RETURN_ON_ERROR(
        esp_lcd_new_panel_ili9486(s_io_handle, &panel_config, &s_panel),
        TAG, "Panel create failed");

    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_panel, true));

    /* Backlight ON */
    gpio_set_level(PIN_NUM_BK_LIGHT, 1);

    /* ── LVGL ──────────────────────────────────────────────────────────────── */
    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle     = s_io_handle,
        .panel_handle  = s_panel,           // ← valid handle, no crash
        .buffer_size   = LCD_H_RES * 40,
        .double_buffer = true,
        .hres          = LCD_H_RES,
        .vres          = LCD_V_RES,
        .monochrome    = false,
#if LVGL_VERSION_MAJOR >= 9
        .color_format  = LV_COLOR_FORMAT_RGB565,
#endif
        .rotation = {
            .swap_xy  = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = true,   // ILI9486 is big-endian; LVGL is little-endian
#endif
            .sw_rotate  = false,
        }
    };
    lvgl_port_add_disp(&disp_cfg);


    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(255, 0, 0), 0);
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello ILI9486");
    lv_obj_center(label);

    vTaskDelay(pdMS_TO_TICKS(3000));   // wait for LVGL to flush the first frame before setting resolution

    display_set_resolution(LCD_H_RES, LCD_V_RES);
    ESP_LOGI(TAG, "ILI9486 initialization complete");
    return ESP_OK;
}