

#include "display.h"
#include "ui_home.h"
//#include "ui_boot.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define DELAY_MS 3000


void app_main(void)
{
    esp_err_t ret;
    ret = display_init();

    if (ret != ESP_OK) {
        //ESP_LOGE("main", "Failed to initialize LCD");
        return;
    }

   
    ui_home_init();
    ui_home_load();

    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_home_set_time("time is 12:34");
    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_home_set_welcome("Hello welcome");
    vTaskDelay(pdMS_TO_TICKS(DELAY_MS)); // Simulate some delay for the boot screen
    ui_home_display_icon_wifi();
    

    while(1){
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    
}