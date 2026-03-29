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


typedef struct {
    bool        has_bg;
    uint32_t    bg;              // raw hex e.g. 0xFFFFFF
    bool        has_bg_opa;
    uint8_t     bg_opa;
    bool        has_border_color;
    uint32_t    border_color;    // raw hex
    bool        has_border_width;
    lv_coord_t  border_width;
    bool        has_radius;
    lv_coord_t  radius;
} ui_style_box_t;

typedef struct {
    bool        has_color;
    uint32_t    color;           // raw hex
    bool        has_size;
    uint16_t    size;
    bool        has_align;
    lv_text_align_t align;
} ui_style_text_t;

typedef struct {
    bool        has_opacity;
    uint8_t     opacity;
} ui_style_effects_t;


typedef struct {
    ui_style_box_t      box;
    ui_style_text_t     text;
    ui_style_effects_t  effects;
} ui_style_t;


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
    ui_style_t style;       // ← add this, zero-initialized = "no styles"
    union {

        struct {    // LABEL
            char text[UI_MAX_STRING_LENGTH];
        } label;

        struct {    // BAR
            int32_t value;
            //int32_t min;
            //int32_t max;
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





#ifdef __cplusplus
}
#endif


#endif