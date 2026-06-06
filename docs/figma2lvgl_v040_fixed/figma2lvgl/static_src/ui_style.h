#ifndef UI_STYLE_H
#define UI_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_defs.h"

// Public API
void ui_apply_style(lv_obj_t *obj, ui_child_type_t type, const ui_style_t *s);

#ifdef __cplusplus
}
#endif

#endif // UI_STYLE_H