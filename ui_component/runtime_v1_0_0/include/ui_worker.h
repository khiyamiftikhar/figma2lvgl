// lvgl_port_notify.h

#ifdef ESP_PLATFORM


#ifndef UI_WORKER_H
#define UI_WORKER_H


#include <stdbool.h>
#include <stdint.h>

 #ifdef __cplusplus
    extern "C" {
 #endif



// Function pointer type for LVGL updates
//typedef void (*lvgl_port_task_callback_t)(void *user_data);

// Public API: enqueue a UI update
bool ui_worker_process_job(void (*cb)(void *),
                           const void *data,
                           uint16_t size);
bool ui_worker_process_job_sync(void (*cb)(void* args), void *user_data);

// Must be called once at startup
void ui_worker_init(void);


#ifdef __cplusplus
    }
    #endif

#endif

#endif