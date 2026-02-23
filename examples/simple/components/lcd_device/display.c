
#include "display.h"
#include <stdint.h>

static uint16_t s_width = 0;
static uint16_t s_height = 0;

void display_set_resolution(uint16_t w, uint16_t h)
{
    s_width = w;
    s_height = h;
}

uint16_t display_get_width(void)
{
    return s_width;
}

uint16_t display_get_height(void)
{
    return s_height;
}

#if CONFIG_LCD_CONTROLLER_SSD1306
    extern esp_err_t ssd1306_display_init(void);
#elif CONFIG_LCD_CONTROLLER_ILI9486
    extern esp_err_t ili9486_display_init(void);
#endif

esp_err_t display_init(void)
{
#if CONFIG_LCD_CONTROLLER_SSD1306
    return ssd1306_display_init();
#elif CONFIG_LCD_CONTROLLER_ILI9486
    return ili9486_display_init();
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}
