#ifndef UI_F_LIST_H
#define UI_F_LIST_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "lvgl.h"

// ------------------------------
// LIFECYCLE
// ------------------------------
void ui_f_list_init(void);
void ui_f_list_load(void);

// ------------------------------
// SETTERS
// ------------------------------
lv_obj_t *ui_f_list_get_list_items(void);
lv_obj_t *ui_f_list_get_grid_cells(void);

// ------------------------------
// EVENT CALLBACKS
// ------------------------------


#ifdef __cplusplus
}
#endif
#endif
