// ui.h
#ifndef UI_DEFS_H
#define UI_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "stdint.h"

#define UI_MAX_CHILDREN         16
#define UI_MAX_ICON_STATES      8
#define UI_MAX_STRING_LENGTH    30
#define UI_MAX_ID_LENGTH        30

typedef enum
{
    UI_CHILD_ICON,
    UI_CHILD_LABEL,
    UI_CHILD_BAR
} ui_child_type_t;

// Multi-state icon description
typedef struct {
    uint8_t total_states;
    const void* state_src[UI_MAX_ICON_STATES];   // pointer to arrays OR file paths
} ui_icon_t;

// A child object of a screen
typedef struct {
    ui_child_type_t type;
    char id[UI_MAX_ID_LENGTH];      //To differentiate among elements of same type e.g labels, whether main or header

    // LVGL object handle created at runtime
    lv_obj_t *lv_obj;

    // Generic attributes
    int x;
    int y;
    int w;
    int h;

    // For ICON type
    ui_icon_t *icon;
    uint8_t current_state;

    // For LABEL type
    char text[UI_MAX_STRING_LENGTH];

    // For BAR type
    int initial_value;
} ui_child_t;

// A complete screen
typedef struct {
    const char *name;
    ui_child_t children[UI_MAX_CHILDREN];
    uint8_t child_count;

    lv_obj_t *lv_screen;     // created at runtime
} ui_screen_t;


#ifdef __cplusplus
}
#endif


#endif