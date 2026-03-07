// lvgl_port_sim.h
#ifndef LVGL_PORT_SIM_H
#define LVGL_PORT_SIM_H

#ifdef ESP_PLATFORM
    #include "ui_worker.h"
#else
    #include "lvgl.h"
    #include <windows.h>
    #include <stdbool.h>
    #include <ui_defs.h>

    #ifdef __cplusplus
    extern "C" {
    #endif

    typedef int esp_err_t;
    #define ESP_OK 0
    #define ESP_FAIL -1

    typedef struct {
        int dummy;
    } lvgl_port_cfg_t;

    // Callback function type
    typedef void (*lvgl_port_task_callback_t)(void *user_data);

    // Initialize the port
    esp_err_t lvgl_port_init(const lvgl_port_cfg_t *cfg);

    //Declared here because the ui sources call the worked_init and visual studio sim gives error when compiles ui sources because it doesnt have such method
    esp_err_t ui_worker_init();
    
    // Lock/unlock LVGL (with timeout in ticks)
    bool lvgl_port_lock(uint32_t timeout_ms);
    void lvgl_port_unlock(void);
    
    // Notify LVGL task with callback
    bool ui_worker_post_job(const ui_job_t *job);

    
    // Process pending notifications (call in main loop)
    void lvgl_port_process_notifications(void);

    #ifdef __cplusplus
    }
    #endif

#endif // ESP_PLATFORM

#endif // LVGL_PORT_SIM_H