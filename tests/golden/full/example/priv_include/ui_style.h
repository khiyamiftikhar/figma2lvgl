#ifndef UI_STYLE_H
#define UI_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "ui_defs.h"

// Public API
void ui_apply_style(lv_obj_t *obj, ui_child_type_t type, const ui_style_t *s);
/* Text colour, font and alignment only (a button's internal label). */
void ui_apply_text_style(lv_obj_t *obj, const ui_style_t *s);
/* Copies text into a buffer of size bytes: at most size - 1 bytes, never
 * half a UTF-8 character, always NUL-terminated. NULL copies "". */
void ui_copy_text(char *dst, size_t size, const char *text);

#ifdef __cplusplus
}
#endif

#endif // UI_STYLE_H