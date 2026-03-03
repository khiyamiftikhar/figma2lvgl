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
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
    
} ui_child_type_t;

// Multi-state icon description
typedef struct {
    uint8_t total_states;
    const void* state_src[UI_MAX_ICON_STATES];   // pointer to arrays OR file paths
} ui_icon_t;

typedef struct {

    ui_child_type_t type;
    char id[UI_MAX_ID_LENGTH];

    lv_obj_t *lv_obj;

    int x;
    int y;
    int w;
    int h;

    union {

        struct {    // LABEL
            char text[UI_MAX_STRING_LENGTH];
        } label;

        struct {    // BAR
            int32_t initial_value;
            int32_t min;
            int32_t max;
        } bar;

        struct {
            const lv_image_dsc_t *src;
        } image;


    } data;

} ui_child_t;
// A complete screen
typedef struct {
    const char *name;
    ui_child_t children[UI_MAX_CHILDREN];
    uint8_t child_count;

    lv_obj_t *lv_screen;     // created at runtime
} ui_screen_t;




typedef struct ui_job_t ui_job_t;
typedef void (*ui_job_cb_t)(ui_job_t *job);

typedef enum {
    UI_JOB_SET_LABEL,
    UI_JOB_LOAD_SCREEN,
    UI_JOB_SET_BAR,
    UI_JOB_SET_IMAGE,
} ui_job_type_t;

typedef struct {
    
    char text[UI_MAX_STRING_LENGTH];
} ui_label_job_t;


typedef struct {
    int32_t value;        // Target value
    uint32_t duration_ms; // Animation duration
} ui_bar_job_t;

typedef struct {
    const lv_image_dsc_t *src;   // pointer to compiled image
} ui_image_job_t;


typedef struct ui_job_t {
    ui_job_cb_t cb;
    uint8_t child_index;
    ui_job_type_t type;

    union {
        ui_label_job_t label;
        ui_bar_job_t   bar;
        ui_image_job_t image;
    } data;

} ui_job_t;

#ifdef __cplusplus
}
#endif


#endif