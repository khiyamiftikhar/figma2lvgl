// ─── ili9486_panel.c ────────────────────────────────────────────────────────
// ─── esp_ili9486_panel.c (top includes) ──────────────────────────────────────
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_heap_caps.h"
#include "esp_lcd_types.h"
#include "esp_lcd_panel_io.h"         // ← esp_lcd_panel_io_tx_param()  ✓
                                      // ← esp_lcd_panel_io_tx_color()  ✓
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_interface.h"
#include "esp_ili9486_panel.h"

static const char *TAG = "ili9486";

// ── ILI9486 command constants ────────────────────────────────────────────────
#define ILI9486_CMD_SWRESET  0x01
#define ILI9486_CMD_SLPOUT   0x11
#define ILI9486_CMD_COLMOD   0x3A
#define ILI9486_CMD_MADCTL   0x36
#define ILI9486_CMD_DISPON   0x29
#define ILI9486_CMD_CASET    0x2A
#define ILI9486_CMD_RASET    0x2B
#define ILI9486_CMD_RAMWR    0x2C
#define ILI9486_CMD_INVON    0x21
#define ILI9486_CMD_INVOFF   0x20

// ── Internal panel object ────────────────────────────────────────────────────
typedef struct {
    esp_lcd_panel_t base;          // MUST be first – lvgl_port casts to this
    esp_lcd_panel_io_handle_t io;
    int reset_gpio_num;
    int x_gap;
    int y_gap;
    uint8_t madctl;                // mirror / rotation state
    bool invert_color;
} ili9486_panel_t;

#define LCD_H_RES 320
#define CONV_BUF_PIXELS (LCD_H_RES * 80)
static uint8_t s_conv_buf[CONV_BUF_PIXELS * 3];

static void rgb565_to_rgb666(
    const uint16_t *src,
    uint8_t *dst,
    size_t pixels)
{
    for (size_t i = 0; i < pixels; i++) {
        uint16_t p = src[i];

        dst[3*i + 0] = ((p >> 11) & 0x1F) << 3;
        dst[3*i + 1] = ((p >> 5)  & 0x3F) << 2;
        dst[3*i + 2] = ( p        & 0x1F) << 3;
    }
}


// ── Forward declarations ─────────────────────────────────────────────────────
static esp_err_t panel_ili9486_del(esp_lcd_panel_t *panel);
static esp_err_t panel_ili9486_reset(esp_lcd_panel_t *panel);
static esp_err_t panel_ili9486_init(esp_lcd_panel_t *panel);
static esp_err_t panel_ili9486_draw_bitmap(esp_lcd_panel_t *panel,
                                            int x_start, int y_start,
                                            int x_end,   int y_end,
                                            const void *color_data);
static esp_err_t panel_ili9486_invert_color(esp_lcd_panel_t *panel, bool invert);
static esp_err_t panel_ili9486_mirror(esp_lcd_panel_t *panel, bool mx, bool my);
static esp_err_t panel_ili9486_swap_xy(esp_lcd_panel_t *panel, bool swap);
static esp_err_t panel_ili9486_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap);
static esp_err_t panel_ili9486_disp_on_off(esp_lcd_panel_t *panel, bool on);

// ── Helper: send a command with 0-N data bytes ───────────────────────────────
static esp_err_t ili9486_send(esp_lcd_panel_io_handle_t io,
                               int cmd, const uint8_t *data, size_t len)
{
    return esp_lcd_panel_io_tx_param(io, cmd, data, len);
}

// ── Init sequence ────────────────────────────────────────────────────────────
static void ili9486_send_init_sequence(esp_lcd_panel_io_handle_t io)
{
    /* Software reset – give it 120 ms */
    ili9486_send(io, ILI9486_CMD_SWRESET, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(120));

    ili9486_send(io, ILI9486_CMD_SLPOUT, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(20));

    /* Power / gamma registers (trimmed from datasheet defaults) */
    ili9486_send(io, 0xB0, (uint8_t[]){0x00}, 1);           // Interface mode
    ili9486_send(io, 0xB1, (uint8_t[]){0xB0, 0x11}, 2);     // Frame rate ~70 Hz
    ili9486_send(io, 0xB4, (uint8_t[]){0x02}, 1);           // Inversion: 2-dot
    ili9486_send(io, 0xB6, (uint8_t[]){0x02, 0x22}, 2);     // Display function
    ili9486_send(io, 0xB7, (uint8_t[]){0xC6}, 1);           // Entry mode
    ili9486_send(io, 0xC0, (uint8_t[]){0x0D, 0x0D}, 2);     // Power control 1
    ili9486_send(io, 0xC1, (uint8_t[]){0x41}, 1);           // Power control 2
    ili9486_send(io, 0xC5, (uint8_t[]){0x00, 0x18}, 2);     // VCOM

    /* Positive gamma */
    ili9486_send(io, 0xE0,
        (uint8_t[]){0x0F,0x1F,0x1C,0x0C,0x0F,0x08,0x48,0x98,
                    0x37,0x0A,0x13,0x04,0x11,0x0D,0x00}, 15);
    /* Negative gamma */
    ili9486_send(io, 0xE1,
        (uint8_t[]){0x0F,0x32,0x2E,0x0B,0x0D,0x05,0x47,0x75,
                    0x37,0x06,0x10,0x03,0x24,0x20,0x00}, 15);

    /* Pixel format: 16-bit (RGB666) */
    ili9486_send(io, ILI9486_CMD_COLMOD, (uint8_t[]){0x66}, 1);


    ili9486_send(io, ILI9486_CMD_MADCTL, (uint8_t[]){0x48}, 1);

    ili9486_send(io, ILI9486_CMD_DISPON, NULL, 0);
    vTaskDelay(pdMS_TO_TICKS(20));
}

// ── Public constructor ───────────────────────────────────────────────────────
esp_err_t esp_lcd_new_panel_ili9486(esp_lcd_panel_io_handle_t io,
                                    const esp_lcd_panel_dev_config_t *cfg,
                                    esp_lcd_panel_handle_t *ret_panel)
{
    ESP_RETURN_ON_FALSE(io && cfg && ret_panel, ESP_ERR_INVALID_ARG, TAG, "invalid arg");

    ili9486_panel_t *ili = heap_caps_calloc(1, sizeof(*ili), MALLOC_CAP_DEFAULT);
    ESP_RETURN_ON_FALSE(ili, ESP_ERR_NO_MEM, TAG, "no memory for panel");

    ili->io             = io;
    ili->reset_gpio_num = cfg->reset_gpio_num;
    ili->madctl         = 0x48;   // default: BGR
    ili->invert_color   = false;

    if (cfg->reset_gpio_num >= 0) {
        gpio_config_t rst_conf = {
            .mode         = GPIO_MODE_OUTPUT,
            .pin_bit_mask = 1ULL << cfg->reset_gpio_num,
        };
        gpio_config(&rst_conf);
    }

    /* Wire up the ops table */
    ili->base.del          = panel_ili9486_del;
    ili->base.reset        = panel_ili9486_reset;
    ili->base.init         = panel_ili9486_init;
    ili->base.draw_bitmap  = panel_ili9486_draw_bitmap;
    ili->base.invert_color = panel_ili9486_invert_color;
    ili->base.mirror       = panel_ili9486_mirror;
    ili->base.swap_xy      = panel_ili9486_swap_xy;
    ili->base.set_gap      = panel_ili9486_set_gap;
    ili->base.disp_on_off  = panel_ili9486_disp_on_off;

    *ret_panel = &ili->base;
    
    return ESP_OK;
}

// ── Op implementations ───────────────────────────────────────────────────────
static esp_err_t panel_ili9486_del(esp_lcd_panel_t *panel)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    free(ili);
    return ESP_OK;
}

static esp_err_t panel_ili9486_reset(esp_lcd_panel_t *panel)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    if (ili->reset_gpio_num >= 0) {
        gpio_set_level(ili->reset_gpio_num, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
        gpio_set_level(ili->reset_gpio_num, 1);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    return ESP_OK;
}

static esp_err_t panel_ili9486_init(esp_lcd_panel_t *panel)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    ili9486_send_init_sequence(ili->io);
    return ESP_OK;
}

static int flush_count = 0;
static esp_err_t panel_ili9486_draw_bitmap(
    esp_lcd_panel_t *panel,
    int x_start, int y_start,
    int x_end,   int y_end,
    const void *color_data)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    esp_lcd_panel_io_handle_t io = ili->io;

    x_start += ili->x_gap;
    x_end   += ili->x_gap;
    y_start += ili->y_gap;
    y_end   += ili->y_gap;

    /* CASET — 8 bytes, each coordinate byte padded with 0x00 */
    uint8_t caset[] = {
        0x00, (uint8_t)((x_start >> 8) & 0xFF),
        0x00, (uint8_t)(x_start & 0xFF),
        0x00, (uint8_t)(((x_end - 1) >> 8) & 0xFF),
        0x00, (uint8_t)((x_end - 1) & 0xFF),
    };

    /* RASET — 8 bytes, same padding */
    uint8_t raset[] = {
        0x00, (uint8_t)((y_start >> 8) & 0xFF),
        0x00, (uint8_t)(y_start & 0xFF),
        0x00, (uint8_t)(((y_end - 1) >> 8) & 0xFF),
        0x00, (uint8_t)((y_end - 1) & 0xFF),
    };

    esp_lcd_panel_io_tx_param(io, ILI9486_CMD_CASET, NULL, 0);
    esp_lcd_panel_io_tx_color(io, -1, caset, 8);

    esp_lcd_panel_io_tx_param(io, ILI9486_CMD_RASET, NULL, 0);
    esp_lcd_panel_io_tx_color(io, -1, raset, 8);

    /* Pixel data */
    size_t pixels = (x_end - x_start) * (y_end - y_start);

    if (pixels > CONV_BUF_PIXELS) {
        ESP_LOGE(TAG, "Flush too large! pixels=%u max=%u",
                 (unsigned)pixels, (unsigned)CONV_BUF_PIXELS);
        return ESP_ERR_INVALID_SIZE;
    }

    rgb565_to_rgb666((const uint16_t *)color_data, s_conv_buf, pixels);

    esp_lcd_panel_io_tx_param(io, ILI9486_CMD_RAMWR, NULL, 0);
    return esp_lcd_panel_io_tx_color(io, -1, s_conv_buf, pixels * 3);
}

static esp_err_t panel_ili9486_invert_color(esp_lcd_panel_t *panel, bool invert)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    int cmd = invert ? ILI9486_CMD_INVON : ILI9486_CMD_INVOFF;
    return esp_lcd_panel_io_tx_param(ili->io, cmd, NULL, 0);
}

static esp_err_t panel_ili9486_mirror(esp_lcd_panel_t *panel, bool mx, bool my)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    if (mx) ili->madctl |=  0x40; else ili->madctl &= ~0x40;
    if (my) ili->madctl |=  0x80; else ili->madctl &= ~0x80;
    return ili9486_send(ili->io, ILI9486_CMD_MADCTL, &ili->madctl, 1);
}

static esp_err_t panel_ili9486_swap_xy(esp_lcd_panel_t *panel, bool swap)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    if (swap) ili->madctl |=  0x20; else ili->madctl &= ~0x20;
    return ili9486_send(ili->io, ILI9486_CMD_MADCTL, &ili->madctl, 1);
}

static esp_err_t panel_ili9486_set_gap(esp_lcd_panel_t *panel, int x_gap, int y_gap)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    ili->x_gap = x_gap;
    ili->y_gap = y_gap;
    return ESP_OK;
}

static esp_err_t panel_ili9486_disp_on_off(esp_lcd_panel_t *panel, bool on)
{
    ili9486_panel_t *ili = __containerof(panel, ili9486_panel_t, base);
    int cmd = on ? ILI9486_CMD_DISPON : 0x28;
    return esp_lcd_panel_io_tx_param(ili->io, cmd, NULL, 0);
}



