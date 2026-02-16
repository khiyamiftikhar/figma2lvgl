// lvgl_port_notify.c

#include <string.h>
#include "ui_worker.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define UI_WORKER_MAX_JOB_SIZE    64

typedef struct {
    void (*cb)(void *args);
    uint16_t data_size;
    uint8_t data[UI_WORKER_MAX_JOB_SIZE];
} notify_msg_t;


static QueueHandle_t notify_queue = NULL;

static void ui_worker_task(void *arg)
{
    notify_msg_t msg;

    while (1) {
        if (xQueueReceive(notify_queue, &msg, portMAX_DELAY)) {

            if (lvgl_port_lock(portMAX_DELAY)) {

                msg.cb((void *)msg.data);

                lvgl_port_unlock();
            }
        }
    }
}

// --------------------------------------------------------
// PUBLIC API
// --------------------------------------------------------
void ui_worker_init(void)
{
    notify_queue = xQueueCreate(16, sizeof(notify_msg_t));
    ESP_ERROR_CHECK(notify_queue==NULL);

    BaseType_t  ret=xTaskCreate(ui_worker_task, "lvgl_notify", 2048, NULL, 5, NULL);
    ESP_ERROR_CHECK(ret!=pdTRUE);
}

bool ui_worker_process_job(void (*cb)(void *),
                           const void *data,
                           uint16_t size){
    notify_msg_t msg;

    msg.cb = cb;
    msg.data_size = size;

    if (size > UI_WORKER_MAX_JOB_SIZE) {
        // optionally assert / log
        return false;
    }

    memcpy(msg.data, data, size);

    return xQueueSend(notify_queue, &msg, portMAX_DELAY);
}

bool ui_worker_process_job_sync(void (*cb)(void* args), void *user_data)
{
    
    if (lvgl_port_lock(portMAX_DELAY)) {

                // run user callback INSIDE LVGL lock
                cb(user_data);
                lvgl_port_unlock();
                return true;
    }


    return false;


}
