#include "ui_f_text_sizes.h"
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
        char        text[UI_TEXT_SIZE(5)];
    } size_8;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_10;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_12;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_14;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_16;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_18;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_20;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_22;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_24;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_26;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_28;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_30;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_32;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_34;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_36;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_38;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_40;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_42;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_44;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_46;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(6)];
    } size_48;
} s_f_text_sizes = {
    .size_8 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 8 }
        },
        .text = "Ag 8",
    },
    .size_10 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 10 }
        },
        .text = "Ag 10",
    },
    .size_12 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 12 }
        },
        .text = "Ag 12",
    },
    .size_14 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 14 }
        },
        .text = "Ag 14",
    },
    .size_16 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 16 }
        },
        .text = "Ag 16",
    },
    .size_18 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 18 }
        },
        .text = "Ag 18",
    },
    .size_20 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 20 }
        },
        .text = "Ag 20",
    },
    .size_22 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 22 }
        },
        .text = "Ag 22",
    },
    .size_24 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 24 }
        },
        .text = "Ag 24",
    },
    .size_26 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 26 }
        },
        .text = "Ag 26",
    },
    .size_28 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 28 }
        },
        .text = "Ag 28",
    },
    .size_30 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 30 }
        },
        .text = "Ag 30",
    },
    .size_32 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 32 }
        },
        .text = "Ag 32",
    },
    .size_34 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 34 }
        },
        .text = "Ag 34",
    },
    .size_36 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 36 }
        },
        .text = "Ag 36",
    },
    .size_38 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 38 }
        },
        .text = "Ag 38",
    },
    .size_40 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 40 }
        },
        .text = "Ag 40",
    },
    .size_42 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 42 }
        },
        .text = "Ag 42",
    },
    .size_44 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 44 }
        },
        .text = "Ag 44",
    },
    .size_46 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 46 }
        },
        .text = "Ag 46",
    },
    .size_48 = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 48 }
        },
        .text = "Ag 48",
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
void ui_f_text_sizes_size_8_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_8.text, sizeof s_f_text_sizes.size_8.text, text);
    if (!s_f_text_sizes.size_8.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_8_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_8.lv_obj, s_f_text_sizes.size_8.text);
}

void ui_f_text_sizes_size_10_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_10.text, sizeof s_f_text_sizes.size_10.text, text);
    if (!s_f_text_sizes.size_10.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_10_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_10.lv_obj, s_f_text_sizes.size_10.text);
}

void ui_f_text_sizes_size_12_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_12.text, sizeof s_f_text_sizes.size_12.text, text);
    if (!s_f_text_sizes.size_12.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_12_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_12.lv_obj, s_f_text_sizes.size_12.text);
}

void ui_f_text_sizes_size_14_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_14.text, sizeof s_f_text_sizes.size_14.text, text);
    if (!s_f_text_sizes.size_14.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_14_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_14.lv_obj, s_f_text_sizes.size_14.text);
}

void ui_f_text_sizes_size_16_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_16.text, sizeof s_f_text_sizes.size_16.text, text);
    if (!s_f_text_sizes.size_16.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_16_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_16.lv_obj, s_f_text_sizes.size_16.text);
}

void ui_f_text_sizes_size_18_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_18.text, sizeof s_f_text_sizes.size_18.text, text);
    if (!s_f_text_sizes.size_18.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_18_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_18.lv_obj, s_f_text_sizes.size_18.text);
}

void ui_f_text_sizes_size_20_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_20.text, sizeof s_f_text_sizes.size_20.text, text);
    if (!s_f_text_sizes.size_20.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_20_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_20.lv_obj, s_f_text_sizes.size_20.text);
}

void ui_f_text_sizes_size_22_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_22.text, sizeof s_f_text_sizes.size_22.text, text);
    if (!s_f_text_sizes.size_22.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_22_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_22.lv_obj, s_f_text_sizes.size_22.text);
}

void ui_f_text_sizes_size_24_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_24.text, sizeof s_f_text_sizes.size_24.text, text);
    if (!s_f_text_sizes.size_24.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_24_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_24.lv_obj, s_f_text_sizes.size_24.text);
}

void ui_f_text_sizes_size_26_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_26.text, sizeof s_f_text_sizes.size_26.text, text);
    if (!s_f_text_sizes.size_26.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_26_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_26.lv_obj, s_f_text_sizes.size_26.text);
}

void ui_f_text_sizes_size_28_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_28.text, sizeof s_f_text_sizes.size_28.text, text);
    if (!s_f_text_sizes.size_28.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_28_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_28.lv_obj, s_f_text_sizes.size_28.text);
}

void ui_f_text_sizes_size_30_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_30.text, sizeof s_f_text_sizes.size_30.text, text);
    if (!s_f_text_sizes.size_30.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_30_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_30.lv_obj, s_f_text_sizes.size_30.text);
}

void ui_f_text_sizes_size_32_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_32.text, sizeof s_f_text_sizes.size_32.text, text);
    if (!s_f_text_sizes.size_32.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_32_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_32.lv_obj, s_f_text_sizes.size_32.text);
}

void ui_f_text_sizes_size_34_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_34.text, sizeof s_f_text_sizes.size_34.text, text);
    if (!s_f_text_sizes.size_34.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_34_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_34.lv_obj, s_f_text_sizes.size_34.text);
}

void ui_f_text_sizes_size_36_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_36.text, sizeof s_f_text_sizes.size_36.text, text);
    if (!s_f_text_sizes.size_36.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_36_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_36.lv_obj, s_f_text_sizes.size_36.text);
}

void ui_f_text_sizes_size_38_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_38.text, sizeof s_f_text_sizes.size_38.text, text);
    if (!s_f_text_sizes.size_38.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_38_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_38.lv_obj, s_f_text_sizes.size_38.text);
}

void ui_f_text_sizes_size_40_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_40.text, sizeof s_f_text_sizes.size_40.text, text);
    if (!s_f_text_sizes.size_40.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_40_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_40.lv_obj, s_f_text_sizes.size_40.text);
}

void ui_f_text_sizes_size_42_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_42.text, sizeof s_f_text_sizes.size_42.text, text);
    if (!s_f_text_sizes.size_42.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_42_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_42.lv_obj, s_f_text_sizes.size_42.text);
}

void ui_f_text_sizes_size_44_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_44.text, sizeof s_f_text_sizes.size_44.text, text);
    if (!s_f_text_sizes.size_44.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_44_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_44.lv_obj, s_f_text_sizes.size_44.text);
}

void ui_f_text_sizes_size_46_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_46.text, sizeof s_f_text_sizes.size_46.text, text);
    if (!s_f_text_sizes.size_46.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_46_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_46.lv_obj, s_f_text_sizes.size_46.text);
}

void ui_f_text_sizes_size_48_set_text(const char *text)
{
    ui_copy_text(s_f_text_sizes.size_48.text, sizeof s_f_text_sizes.size_48.text, text);
    if (!s_f_text_sizes.size_48.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_size_48_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_f_text_sizes.size_48.lv_obj, s_f_text_sizes.size_48.text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_text_sizes_load(void)
{
    if (!s_f_text_sizes.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_load: screen not built");
        return;
    }
    lv_scr_load(s_f_text_sizes.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_text_sizes_init(void)
{
    if (s_f_text_sizes.lv_screen)
        return;
    s_f_text_sizes.lv_screen = lv_obj_create(NULL);

    /* size_8 (label) */
    s_f_text_sizes.size_8.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_8.lv_obj, 6, 4);
    lv_obj_set_width(s_f_text_sizes.size_8.lv_obj, 19);
    lv_label_set_long_mode(s_f_text_sizes.size_8.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_8.lv_obj, s_f_text_sizes.size_8.text);
    ui_apply_style(s_f_text_sizes.size_8.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_8.style);

    /* size_10 (label) */
    s_f_text_sizes.size_10.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_10.lv_obj, 6, 16);
    lv_obj_set_width(s_f_text_sizes.size_10.lv_obj, 27);
    lv_label_set_long_mode(s_f_text_sizes.size_10.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_10.lv_obj, s_f_text_sizes.size_10.text);
    ui_apply_style(s_f_text_sizes.size_10.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_10.style);

    /* size_12 (label) */
    s_f_text_sizes.size_12.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_12.lv_obj, 6, 30);
    lv_obj_set_width(s_f_text_sizes.size_12.lv_obj, 32);
    lv_label_set_long_mode(s_f_text_sizes.size_12.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_12.lv_obj, s_f_text_sizes.size_12.text);
    ui_apply_style(s_f_text_sizes.size_12.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_12.style);

    /* size_14 (label) */
    s_f_text_sizes.size_14.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_14.lv_obj, 6, 47);
    lv_obj_set_width(s_f_text_sizes.size_14.lv_obj, 38);
    lv_label_set_long_mode(s_f_text_sizes.size_14.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_14.lv_obj, s_f_text_sizes.size_14.text);
    ui_apply_style(s_f_text_sizes.size_14.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_14.style);

    /* size_16 (label) */
    s_f_text_sizes.size_16.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_16.lv_obj, 6, 66);
    lv_obj_set_width(s_f_text_sizes.size_16.lv_obj, 43);
    lv_label_set_long_mode(s_f_text_sizes.size_16.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_16.lv_obj, s_f_text_sizes.size_16.text);
    ui_apply_style(s_f_text_sizes.size_16.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_16.style);

    /* size_18 (label) */
    s_f_text_sizes.size_18.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_18.lv_obj, 6, 88);
    lv_obj_set_width(s_f_text_sizes.size_18.lv_obj, 48);
    lv_label_set_long_mode(s_f_text_sizes.size_18.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_18.lv_obj, s_f_text_sizes.size_18.text);
    ui_apply_style(s_f_text_sizes.size_18.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_18.style);

    /* size_20 (label) */
    s_f_text_sizes.size_20.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_20.lv_obj, 6, 112);
    lv_obj_set_width(s_f_text_sizes.size_20.lv_obj, 58);
    lv_label_set_long_mode(s_f_text_sizes.size_20.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_20.lv_obj, s_f_text_sizes.size_20.text);
    ui_apply_style(s_f_text_sizes.size_20.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_20.style);

    /* size_22 (label) */
    s_f_text_sizes.size_22.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_22.lv_obj, 6, 138);
    lv_obj_set_width(s_f_text_sizes.size_22.lv_obj, 62);
    lv_label_set_long_mode(s_f_text_sizes.size_22.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_22.lv_obj, s_f_text_sizes.size_22.text);
    ui_apply_style(s_f_text_sizes.size_22.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_22.style);

    /* size_24 (label) */
    s_f_text_sizes.size_24.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_24.lv_obj, 6, 167);
    lv_obj_set_width(s_f_text_sizes.size_24.lv_obj, 69);
    lv_label_set_long_mode(s_f_text_sizes.size_24.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_24.lv_obj, s_f_text_sizes.size_24.text);
    ui_apply_style(s_f_text_sizes.size_24.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_24.style);

    /* size_26 (label) */
    s_f_text_sizes.size_26.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_26.lv_obj, 6, 198);
    lv_obj_set_width(s_f_text_sizes.size_26.lv_obj, 74);
    lv_label_set_long_mode(s_f_text_sizes.size_26.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_26.lv_obj, s_f_text_sizes.size_26.text);
    ui_apply_style(s_f_text_sizes.size_26.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_26.style);

    /* size_28 (label) */
    s_f_text_sizes.size_28.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_28.lv_obj, 6, 232);
    lv_obj_set_width(s_f_text_sizes.size_28.lv_obj, 81);
    lv_label_set_long_mode(s_f_text_sizes.size_28.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_28.lv_obj, s_f_text_sizes.size_28.text);
    ui_apply_style(s_f_text_sizes.size_28.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_28.style);

    /* size_30 (label) */
    s_f_text_sizes.size_30.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_30.lv_obj, 6, 268);
    lv_obj_set_width(s_f_text_sizes.size_30.lv_obj, 87);
    lv_label_set_long_mode(s_f_text_sizes.size_30.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_30.lv_obj, s_f_text_sizes.size_30.text);
    ui_apply_style(s_f_text_sizes.size_30.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_30.style);

    /* size_32 (label) */
    s_f_text_sizes.size_32.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_32.lv_obj, 166, 4);
    lv_obj_set_width(s_f_text_sizes.size_32.lv_obj, 90);
    lv_label_set_long_mode(s_f_text_sizes.size_32.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_32.lv_obj, s_f_text_sizes.size_32.text);
    ui_apply_style(s_f_text_sizes.size_32.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_32.style);

    /* size_34 (label) */
    s_f_text_sizes.size_34.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_34.lv_obj, 166, 45);
    lv_obj_set_width(s_f_text_sizes.size_34.lv_obj, 99);
    lv_label_set_long_mode(s_f_text_sizes.size_34.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_34.lv_obj, s_f_text_sizes.size_34.text);
    ui_apply_style(s_f_text_sizes.size_34.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_34.style);

    /* size_36 (label) */
    s_f_text_sizes.size_36.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_36.lv_obj, 166, 88);
    lv_obj_set_width(s_f_text_sizes.size_36.lv_obj, 102);
    lv_label_set_long_mode(s_f_text_sizes.size_36.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_36.lv_obj, s_f_text_sizes.size_36.text);
    ui_apply_style(s_f_text_sizes.size_36.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_36.style);

    /* size_38 (label) */
    s_f_text_sizes.size_38.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_38.lv_obj, 166, 134);
    lv_obj_set_width(s_f_text_sizes.size_38.lv_obj, 109);
    lv_label_set_long_mode(s_f_text_sizes.size_38.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_38.lv_obj, s_f_text_sizes.size_38.text);
    ui_apply_style(s_f_text_sizes.size_38.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_38.style);

    /* size_40 (label) */
    s_f_text_sizes.size_40.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_40.lv_obj, 166, 182);
    lv_obj_set_width(s_f_text_sizes.size_40.lv_obj, 120);
    lv_label_set_long_mode(s_f_text_sizes.size_40.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_40.lv_obj, s_f_text_sizes.size_40.text);
    ui_apply_style(s_f_text_sizes.size_40.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_40.style);

    /* size_42 (label) */
    s_f_text_sizes.size_42.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_42.lv_obj, 166, 233);
    lv_obj_set_width(s_f_text_sizes.size_42.lv_obj, 122);
    lv_label_set_long_mode(s_f_text_sizes.size_42.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_42.lv_obj, s_f_text_sizes.size_42.text);
    ui_apply_style(s_f_text_sizes.size_42.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_42.style);

    /* size_44 (label) */
    s_f_text_sizes.size_44.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_44.lv_obj, 166, 286);
    lv_obj_set_width(s_f_text_sizes.size_44.lv_obj, 132);
    lv_label_set_long_mode(s_f_text_sizes.size_44.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_44.lv_obj, s_f_text_sizes.size_44.text);
    ui_apply_style(s_f_text_sizes.size_44.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_44.style);

    /* size_46 (label) */
    s_f_text_sizes.size_46.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_46.lv_obj, 166, 342);
    lv_obj_set_width(s_f_text_sizes.size_46.lv_obj, 135);
    lv_label_set_long_mode(s_f_text_sizes.size_46.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_46.lv_obj, s_f_text_sizes.size_46.text);
    ui_apply_style(s_f_text_sizes.size_46.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_46.style);

    /* size_48 (label) */
    s_f_text_sizes.size_48.lv_obj = lv_label_create(s_f_text_sizes.lv_screen);
    lv_obj_set_pos(s_f_text_sizes.size_48.lv_obj, 166, 400);
    lv_obj_set_width(s_f_text_sizes.size_48.lv_obj, 143);
    lv_label_set_long_mode(s_f_text_sizes.size_48.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_f_text_sizes.size_48.lv_obj, s_f_text_sizes.size_48.text);
    ui_apply_style(s_f_text_sizes.size_48.lv_obj, UI_CHILD_LABEL, &s_f_text_sizes.size_48.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_text_sizes_deinit(void)
{
    if (!s_f_text_sizes.lv_screen)
        return;
    if (lv_screen_active() == s_f_text_sizes.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_text_sizes_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_f_text_sizes.lv_screen);   /* frees every child, callback and animation */
    s_f_text_sizes.lv_screen = NULL;
    s_f_text_sizes.size_8.lv_obj = NULL;
    s_f_text_sizes.size_10.lv_obj = NULL;
    s_f_text_sizes.size_12.lv_obj = NULL;
    s_f_text_sizes.size_14.lv_obj = NULL;
    s_f_text_sizes.size_16.lv_obj = NULL;
    s_f_text_sizes.size_18.lv_obj = NULL;
    s_f_text_sizes.size_20.lv_obj = NULL;
    s_f_text_sizes.size_22.lv_obj = NULL;
    s_f_text_sizes.size_24.lv_obj = NULL;
    s_f_text_sizes.size_26.lv_obj = NULL;
    s_f_text_sizes.size_28.lv_obj = NULL;
    s_f_text_sizes.size_30.lv_obj = NULL;
    s_f_text_sizes.size_32.lv_obj = NULL;
    s_f_text_sizes.size_34.lv_obj = NULL;
    s_f_text_sizes.size_36.lv_obj = NULL;
    s_f_text_sizes.size_38.lv_obj = NULL;
    s_f_text_sizes.size_40.lv_obj = NULL;
    s_f_text_sizes.size_42.lv_obj = NULL;
    s_f_text_sizes.size_44.lv_obj = NULL;
    s_f_text_sizes.size_46.lv_obj = NULL;
    s_f_text_sizes.size_48.lv_obj = NULL;
}
