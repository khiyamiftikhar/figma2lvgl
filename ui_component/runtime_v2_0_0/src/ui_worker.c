// lvgl_port_notify.c
#include "ui_worker.h"
#include "esp_lvgl_port.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

typedef struct {
    void (*cb)(void* args);
    void *user_data;
} notify_msg_t;

static QueueHandle_t notify_queue = NULL;

static void ui_worker_task(void *arg)
{
    ui_job_t job;

    while (1) {

        if (xQueueReceive(notify_queue, &job, portMAX_DELAY)) {

            // SAFELY perform UI update
            if (lvgl_port_lock(portMAX_DELAY)) {

                if (job.cb != NULL) {
                    job.cb(&job);
                }

                lvgl_port_unlock();
            }

            // If still using esp_lvgl_port and needed:
            // lvgl_port_task_wake();
        }
    }
}

// --------------------------------------------------------
// PUBLIC API
// --------------------------------------------------------
void ui_worker_init(void)
{
    notify_queue = xQueueCreate(16, sizeof(ui_job_t));
    ESP_ERROR_CHECK(notify_queue==NULL);

    BaseType_t  ret=xTaskCreate(ui_worker_task, "lvgl_notify", 4096, NULL, 5, NULL);
    ESP_ERROR_CHECK(ret!=pdTRUE);
}

bool ui_worker_post_job(const ui_job_t *job)
{
    return xQueueSend(notify_queue, job, 0) == pdTRUE;
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
