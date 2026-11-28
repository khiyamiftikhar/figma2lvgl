#include "ui_f_list.h"
#include "assets.h"
#include "ui_defs.h"
#include "ui_style.h"
#include <stdio.h>

// ------------------------------
// SCREEN STRUCT (file-static)
// ------------------------------
static struct {
    lv_obj_t *lv_screen;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } list_items;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } grid_cells;
} s_f_list = {
    .list_items = {
        .style = {
            .box = { .has_bg = true, .bg = 0xECEFF1 }
        },
    },
    .grid_cells = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE8EAF6 }
        },
    },
};

// ------------------------------
// BAR ANIMATION HELPER
// ------------------------------


// ------------------------------
// EVENT CALLBACKS
// Implement these in your application .c file.
// Linker will report undefined reference if not implemented.
// ------------------------------


// ------------------------------
// SETTERS
// ------------------------------
lv_obj_t *ui_f_list_get_list_items(void)
{
    return s_f_list.list_items.lv_obj;
}

lv_obj_t *ui_f_list_get_grid_cells(void)
{
    return s_f_list.grid_cells.lv_obj;
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_list_load(void)
{
    if (!s_f_list.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_list_load: screen not built");
        return;
    }
    lv_scr_load(s_f_list.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_list_init(void)
{
    if (s_f_list.lv_screen)
        return;
    s_f_list.lv_screen = lv_obj_create(NULL);

    /* list_items (dynamic container — firmware fills at runtime) */
    s_f_list.list_items.lv_obj = lv_obj_create(s_f_list.lv_screen);
    lv_obj_set_pos(s_f_list.list_items.lv_obj, 10, 10);
    lv_obj_set_size(s_f_list.list_items.lv_obj, 200, 200);
    lv_obj_set_scrollbar_mode(s_f_list.list_items.lv_obj, LV_SCROLLBAR_MODE_AUTO);
    ui_apply_style(s_f_list.list_items.lv_obj, UI_CHILD_PANEL, &s_f_list.list_items.style);

    /* grid_cells (dynamic container — firmware fills at runtime) */
    s_f_list.grid_cells.lv_obj = lv_obj_create(s_f_list.lv_screen);
    lv_obj_set_pos(s_f_list.grid_cells.lv_obj, 220, 10);
    lv_obj_set_size(s_f_list.grid_cells.lv_obj, 90, 200);
    lv_obj_set_scrollbar_mode(s_f_list.grid_cells.lv_obj, LV_SCROLLBAR_MODE_AUTO);
    ui_apply_style(s_f_list.grid_cells.lv_obj, UI_CHILD_PANEL, &s_f_list.grid_cells.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_list_deinit(void)
{
    if (!s_f_list.lv_screen)
        return;
    if (lv_screen_active() == s_f_list.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_list_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_f_list.lv_screen);   /* frees every child, callback and animation */
    s_f_list.lv_screen = NULL;
    s_f_list.list_items.lv_obj = NULL;
    s_f_list.grid_cells.lv_obj = NULL;
}
