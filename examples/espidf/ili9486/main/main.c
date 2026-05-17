
#include "ili9486.h"

#include "touch.h"
#include "ui_home.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"

static const char *TAG = "main";

#define MS(x) pdMS_TO_TICKS(x)

void ui_home_on_btn_test_clicked(lv_event_t* e)
{
    (void)e;

    ui_home_welcome_set_text("Button clicked!");
}

void ui_home_on_btn_test_long_pressed(lv_event_t* e)
{
    (void)e;
    ui_home_welcome_set_text("Long pressed!");
}

void app_main(void)
{
    // --------------------------------------------------------
    // 1. Display — must come first (sets up SPI bus + LVGL port)
    // --------------------------------------------------------
    lv_display_t *disp=NULL;
    if (ili9486_display_init(&disp) != ESP_OK) {
        ESP_LOGE(TAG, "Display init failed");
        return;
    }

    // --------------------------------------------------------
    // 2. Touch — shares SPI bus, registers LVGL indev
    //    display_get_lvgl_handle() must be exposed by display.h
    // --------------------------------------------------------
    
    if (touch_init(disp) != ESP_OK) {
        ESP_LOGE(TAG, "Touch init failed");
        return;
    }

    // --------------------------------------------------------
    // 3. UI — init and load home screen
    // --------------------------------------------------------
    lvgl_port_lock(portMAX_DELAY);
    ui_home_init();
    ui_home_load();
    lvgl_port_unlock();

    // --------------------------------------------------------
    // 4. State machine (mirrors the simulator project)
    // --------------------------------------------------------
    uint8_t state   = 0;
    bool    settled = false;



    
    while (1) {
        switch (state) {

        case 0:
            // Wait 3 s, then show the time
            vTaskDelay(MS(3000));
            lvgl_port_lock(portMAX_DELAY);
            ui_home_time_set_text("time is 12:34");
            lvgl_port_unlock();
            state   = 1;
            settled = false;
            break;

        case 1:
            // Animate bar once, wait 5 s
            if (!settled) {
                lvgl_port_lock(portMAX_DELAY);
                ui_home_bar_set_value(100, 2000);
                lvgl_port_unlock();
                settled = true;
            }
            vTaskDelay(MS(5000));
            state   = 2;
            settled = false;
            break;

        case 2:
            // Show welcome text + wifi icon once, wait 1 s
            if (!settled) {
                lvgl_port_lock(portMAX_DELAY);
                ui_home_welcome_set_text("Good afternoon");
                ui_home_icon_wifi_display();
                ui_home_wifi_off_display();
                lvgl_port_unlock();
                settled = true;
            }
            vTaskDelay(MS(1000));
            state   = 3;
            settled = false;
            break;

        case 3:
            // Label the button — touch events drive everything after
            if (!settled) {
                lvgl_port_lock(portMAX_DELAY);
                ui_home_btn_test_set_label("Press me");
                lvgl_port_unlock();
                settled = true;
            }
            vTaskDelay(MS(1000));   // idle, LVGL task handles touch
            break;

        default:
            vTaskDelay(MS(1000));
            break;
        }
    }
}