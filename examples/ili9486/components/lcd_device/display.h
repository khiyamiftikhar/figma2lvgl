#pragma once

#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t display_init(void);
void display_set_resolution(uint16_t w, uint16_t h);
uint16_t display_get_width(void);
uint16_t display_get_height(void);

#ifdef __cplusplus
}
#endif
