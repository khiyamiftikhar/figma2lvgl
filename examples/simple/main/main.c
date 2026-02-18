

#include "lcd_device.h"
#include "ui_home.h"
#include "ui_boot.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DELAY_MS 3000


void app_main(void)
{
    esp_err_t ret;
    ret = lcd_init();
    if (ret != ESP_OK) {
        //ESP_LOGE("main", "Failed to initialize LCD");
        return;
    }

    ui_boot_init();
    ui_home_init();

    ui_boot_screen_set_label_main("Booting Scr");
    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_boot_screen_set_label_channel_no("Ch 1");
    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_boot_screen_set_label_discovery("Active");

    ui_boot_screen_load();

    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_home_screen_set_label_ap_ssid("MyNet");
    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_home_screen_set_label_main("Home Screen");
    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_home_screen_set_icon_wifi(1);
    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_home_screen_set_label_discovery("Complete");
    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_home_screen_load();

    
}