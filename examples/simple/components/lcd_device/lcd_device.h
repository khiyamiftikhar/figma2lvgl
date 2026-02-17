#ifndef LCD_DEVICE_H
#define LCD_DEVICE_H


#include "esp_err.h"

#ifdef __cplusplus
    extern "C" {
 #endif



//The name is misleading as if it is some gui event.
//Actually it is some system event for gui to display info about



esp_err_t lcd_init();


#ifdef __cplusplus
    }
    #endif

#endif