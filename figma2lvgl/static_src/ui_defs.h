// ui_defs.h  (v0.4.0)
//
// Contains only the types shared across all generated files:
//   - ui_child_type_t enum
//   - ui_style_t and sub-structs
//
// REMOVED in v0.4.0:
//   - ui_child_t  (replaced by per-screen generated structs)
//   - ui_screen_t (replaced by per-screen generated structs)
//   - UI_MAX_CHILDREN (no longer needed — struct is always exact size)
//
// ui_config.h (auto-generated) still provides:
//   - UI_MAX_STRING_LENGTH
//   - UI_MAX_ID_LENGTH

#ifndef UI_DEFS_H
#define UI_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "stdint.h"
#include "stdbool.h"

// NOTE: ui_style.c (priv_src/) also includes ui_defs.h.
// Ensure priv_include/ is on the include path for ALL ui_src/ source files.
//   ESP-IDF / Zephyr : handled automatically.
//   Bare-metal Makefile: add -Iui_src/priv_include to CFLAGS.
#include "ui_config.h"


// ── Widget type enum ──────────────────────────────────────────────────────────
// Used by ui_apply_style() to apply type-specific style properties.

typedef enum {
    UI_CHILD_LABEL,
    UI_CHILD_BAR,
    UI_CHILD_IMAGE,
    UI_CHILD_BUTTON,
    UI_CHILD_SLIDER,
    UI_CHILD_PANEL,
    UI_CHILD_DYNAMIC,
} ui_child_type_t;


// ── Style structs ─────────────────────────────────────────────────────────────
// Embedded in every generated widget struct field.
// has_* guards: zero-initialised ({0}) safely means "no style applied".

typedef struct {
    bool        has_bg;
    uint32_t    bg;              // raw hex e.g. 0xFFFFFF
    bool        has_bg_opa;
    uint8_t     bg_opa;          // 0-255
    bool        has_border_color;
    uint32_t    border_color;
    bool        has_border_width;
    lv_coord_t  border_width;
    bool        has_radius;
    lv_coord_t  radius;
} ui_style_box_t;

typedef struct {
    bool            has_color;
    uint32_t        color;
    bool            has_size;
    uint16_t        size;
    bool            has_align;
    lv_text_align_t align;
} ui_style_text_t;

typedef struct {
    bool    has_opacity;
    uint8_t opacity;
} ui_style_effects_t;

typedef struct {
    ui_style_box_t     box;
    ui_style_text_t    text;
    ui_style_effects_t effects;
} ui_style_t;


#ifdef __cplusplus
}
#endif

#endif // UI_DEFS_H
