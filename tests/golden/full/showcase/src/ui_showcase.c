#include "ui_showcase.h"
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
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } title;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const lv_image_dsc_t *src;
        } icon_star;
    } panel_top;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_ok;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_cancel;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } battery_bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_volume;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_level;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } card_title;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } card_body;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const lv_image_dsc_t *src;
        } icon_heart;
    } panel_card;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } list_devices;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_dim;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_gradient;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_shadow;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_fill_a;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_fill_b;
    } panel_row;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } custom_font;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_icon;
} s_showcase = {
    .panel_top = {
        .style = {
            .box = { .has_bg = true, .bg = 0x263238 }
        },
        .title = {
            .style = {
                .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 16 }
            },
            .text = "Showcase",
        },
        .icon_star = {
            .style = { .box = {0}, .text = {0}, .effects = {0} },
        },
    },
    .btn_ok = {
        .style = {
            .box = { .has_bg = true, .bg = 0x43A047, .has_radius = true, .radius = 8 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "OK",
    },
    .btn_cancel = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFFFFFF, .has_border_color = true, .border_color = 0xE53935, .has_border_width = true, .border_width = 2, .has_radius = true, .radius = 8 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Cancel",
    },
    .battery_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x4CAF50 }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .slider_volume = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .slider_level = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .panel_card = {
        .style = {
            .box = { .has_bg = true, .bg = 0xECEFF1, .has_radius = true, .radius = 8 }
        },
        .card_title = {
            .style = {
                .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 18 }
            },
            .text = "Card",
        },
        .card_body = {
            .style = {
                .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 12 }
            },
            .text = "Two lines of\nbody text",
        },
        .icon_heart = {
            .style = { .box = {0}, .text = {0}, .effects = {0} },
        },
    },
    .list_devices = {
        .style = {
            .box = { .has_bg = true, .bg = 0xE0E0E0 }
        },
    },
    .panel_dim = {
        .style = {
            .box = { .has_bg = true, .bg = 0x3F51B5 },
            .effects = { .has_opacity = true, .opacity = 128 }
        },
    },
    .panel_gradient = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .btn_shadow = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFFFFFF, .has_radius = true, .radius = 8 },
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 14 }
        },
        .label_text = "Shadow",
    },
    .panel_row = {
        .style = {
            .box = { .has_bg = true, .bg = 0xFFF8E1 }
        },
        .btn_fill_a = {
            .style = {
                .box = { .has_bg = true, .bg = 0xFFB300 },
                .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
            },
            .label_text = "A",
        },
        .btn_fill_b = {
            .style = {
                .box = { .has_bg = true, .bg = 0xFFB300 },
                .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
            },
            .label_text = "B",
        },
    },
    .custom_font = {
        .style = {
            .text = { .has_color = true, .color = 0x212121, .has_size = true, .size = 18 }
        },
        .text = "Inter font",
    },
    .btn_icon = {
        .style = {
            .box = { .has_bg = true, .bg = 0x1E88E5, .has_radius = true, .radius = 6 },
            .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 14 }
        },
        .label_text = "Favourite",
    },
};

// ------------------------------
// BAR ANIMATION HELPER
// ------------------------------
static void bar_anim_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value((lv_obj_t *)obj, v, LV_ANIM_OFF);
}

// ------------------------------
// EVENT CALLBACKS
// Implement these in your application .c file.
// Linker will report undefined reference if not implemented.
// ------------------------------




// ------------------------------
// SETTERS
// ------------------------------
void ui_showcase_btn_ok_set_label(const char *text)
{
    if (!s_showcase.btn_ok.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_showcase.btn_ok.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_showcase_btn_cancel_set_label(const char *text)
{
    if (!s_showcase.btn_cancel.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_showcase.btn_cancel.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_showcase_battery_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_showcase.battery_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_showcase.battery_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_showcase.battery_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_showcase.battery_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_showcase_slider_volume_set_value(int32_t value)
{
    if (s_showcase.slider_volume.lv_obj)
        lv_slider_set_value(s_showcase.slider_volume.lv_obj, value, LV_ANIM_OFF);
}

void ui_showcase_slider_level_set_value(int32_t value)
{
    if (s_showcase.slider_level.lv_obj)
        lv_slider_set_value(s_showcase.slider_level.lv_obj, value, LV_ANIM_OFF);
}

lv_obj_t *ui_showcase_get_list_devices(void)
{
    return s_showcase.list_devices.lv_obj;
}

void ui_showcase_btn_shadow_set_label(const char *text)
{
    if (!s_showcase.btn_shadow.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_showcase.btn_shadow.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_showcase_custom_font_set_text(const char *text)
{
    if (s_showcase.custom_font.lv_obj)
        lv_label_set_text(s_showcase.custom_font.lv_obj, text);
}

void ui_showcase_btn_icon_set_label(const char *text)
{
    if (!s_showcase.btn_icon.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_showcase.btn_icon.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_showcase_panel_top_title_set_text(const char *text)
{
    if (s_showcase.panel_top.title.lv_obj)
        lv_label_set_text(s_showcase.panel_top.title.lv_obj, text);
}

void ui_showcase_panel_top_icon_star_display(void)
{
    if (!s_showcase.panel_top.icon_star.lv_obj) return;
    s_showcase.panel_top.icon_star.src = &icon_star;
    lv_image_set_src(s_showcase.panel_top.icon_star.lv_obj, s_showcase.panel_top.icon_star.src);
}

void ui_showcase_panel_card_card_title_set_text(const char *text)
{
    if (s_showcase.panel_card.card_title.lv_obj)
        lv_label_set_text(s_showcase.panel_card.card_title.lv_obj, text);
}

void ui_showcase_panel_card_card_body_set_text(const char *text)
{
    if (s_showcase.panel_card.card_body.lv_obj)
        lv_label_set_text(s_showcase.panel_card.card_body.lv_obj, text);
}

void ui_showcase_panel_card_icon_heart_display(void)
{
    if (!s_showcase.panel_card.icon_heart.lv_obj) return;
    s_showcase.panel_card.icon_heart.src = &icon_heart;
    lv_image_set_src(s_showcase.panel_card.icon_heart.lv_obj, s_showcase.panel_card.icon_heart.src);
}

void ui_showcase_panel_row_btn_fill_a_set_label(const char *text)
{
    if (!s_showcase.panel_row.btn_fill_a.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_showcase.panel_row.btn_fill_a.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_showcase_panel_row_btn_fill_b_set_label(const char *text)
{
    if (!s_showcase.panel_row.btn_fill_b.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_showcase.panel_row.btn_fill_b.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_showcase_load(void)
{
    lv_scr_load(s_showcase.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_showcase_init(void)
{
    s_showcase.lv_screen = lv_obj_create(NULL);

    /* panel_top (panel) */
    s_showcase.panel_top.lv_obj = lv_obj_create(s_showcase.lv_screen);
    lv_obj_remove_style_all(s_showcase.panel_top.lv_obj);
    lv_obj_set_pos(s_showcase.panel_top.lv_obj, 0, 0);
    lv_obj_set_size(s_showcase.panel_top.lv_obj, 480, 40);
    lv_obj_clear_flag(s_showcase.panel_top.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_showcase.panel_top.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_showcase.panel_top.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_showcase.panel_top.lv_obj, UI_CHILD_PANEL, &s_showcase.panel_top.style);

    /* btn_ok (button) */
    s_showcase.btn_ok.lv_obj = lv_button_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.btn_ok.lv_obj, 12, 52);
    lv_obj_set_size(s_showcase.btn_ok.lv_obj, 100, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_showcase.btn_ok.lv_obj);
        lv_label_set_text(lbl, s_showcase.btn_ok.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_showcase.btn_ok.style);
    }
    lv_obj_add_event_cb(s_showcase.btn_ok.lv_obj, ui_showcase_on_btn_ok_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_showcase.btn_ok.lv_obj, UI_CHILD_BUTTON, &s_showcase.btn_ok.style);

    /* btn_cancel (button) */
    s_showcase.btn_cancel.lv_obj = lv_button_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.btn_cancel.lv_obj, 122, 52);
    lv_obj_set_size(s_showcase.btn_cancel.lv_obj, 100, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_showcase.btn_cancel.lv_obj);
        lv_label_set_text(lbl, s_showcase.btn_cancel.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_showcase.btn_cancel.style);
    }
    lv_obj_add_event_cb(s_showcase.btn_cancel.lv_obj, ui_showcase_on_btn_cancel_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_showcase.btn_cancel.lv_obj, ui_showcase_on_btn_cancel_long_pressed,
                        LV_EVENT_LONG_PRESSED, NULL);
    ui_apply_style(s_showcase.btn_cancel.lv_obj, UI_CHILD_BUTTON, &s_showcase.btn_cancel.style);

    /* battery_bar (bar) */
    s_showcase.battery_bar.lv_obj = lv_bar_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.battery_bar.lv_obj, 12, 104);
    lv_obj_set_size(s_showcase.battery_bar.lv_obj, 210, 16);
    lv_bar_set_range(s_showcase.battery_bar.lv_obj, s_showcase.battery_bar.min, s_showcase.battery_bar.max);
    lv_bar_set_value(s_showcase.battery_bar.lv_obj, s_showcase.battery_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_showcase.battery_bar.lv_obj, UI_CHILD_BAR, &s_showcase.battery_bar.style);

    /* slider_volume (slider) */
    s_showcase.slider_volume.lv_obj = lv_slider_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.slider_volume.lv_obj, 12, 132);
    lv_obj_set_size(s_showcase.slider_volume.lv_obj, 210, 16);
    lv_slider_set_range(s_showcase.slider_volume.lv_obj, s_showcase.slider_volume.min, s_showcase.slider_volume.max);
    lv_slider_set_value(s_showcase.slider_volume.lv_obj, s_showcase.slider_volume.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_showcase.slider_volume.lv_obj, ui_showcase_on_slider_volume,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_showcase.slider_volume.lv_obj, UI_CHILD_SLIDER, &s_showcase.slider_volume.style);

    /* slider_level (slider) */
    s_showcase.slider_level.lv_obj = lv_slider_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.slider_level.lv_obj, 236, 52);
    lv_obj_set_size(s_showcase.slider_level.lv_obj, 16, 120);
    lv_slider_set_range(s_showcase.slider_level.lv_obj, s_showcase.slider_level.min, s_showcase.slider_level.max);
    lv_slider_set_value(s_showcase.slider_level.lv_obj, s_showcase.slider_level.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_showcase.slider_level.lv_obj, ui_showcase_on_slider_level,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_showcase.slider_level.lv_obj, UI_CHILD_SLIDER, &s_showcase.slider_level.style);

    /* panel_card (panel) */
    s_showcase.panel_card.lv_obj = lv_obj_create(s_showcase.lv_screen);
    lv_obj_remove_style_all(s_showcase.panel_card.lv_obj);
    lv_obj_set_pos(s_showcase.panel_card.lv_obj, 12, 160);
    lv_obj_set_size(s_showcase.panel_card.lv_obj, 210, 100);
    lv_obj_clear_flag(s_showcase.panel_card.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_showcase.panel_card.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_showcase.panel_card.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_showcase.panel_card.lv_obj, UI_CHILD_PANEL, &s_showcase.panel_card.style);

    /* list_devices (dynamic container — firmware fills at runtime) */
    s_showcase.list_devices.lv_obj = lv_obj_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.list_devices.lv_obj, 12, 268);
    lv_obj_set_size(s_showcase.list_devices.lv_obj, 210, 44);
    lv_obj_set_scrollbar_mode(s_showcase.list_devices.lv_obj, LV_SCROLLBAR_MODE_AUTO);
    ui_apply_style(s_showcase.list_devices.lv_obj, UI_CHILD_PANEL, &s_showcase.list_devices.style);

    /* panel_dim (panel) */
    s_showcase.panel_dim.lv_obj = lv_obj_create(s_showcase.lv_screen);
    lv_obj_remove_style_all(s_showcase.panel_dim.lv_obj);
    lv_obj_set_pos(s_showcase.panel_dim.lv_obj, 236, 180);
    lv_obj_set_size(s_showcase.panel_dim.lv_obj, 40, 40);
    lv_obj_clear_flag(s_showcase.panel_dim.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_showcase.panel_dim.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_showcase.panel_dim.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_showcase.panel_dim.lv_obj, UI_CHILD_PANEL, &s_showcase.panel_dim.style);

    /* panel_gradient (panel) */
    s_showcase.panel_gradient.lv_obj = lv_obj_create(s_showcase.lv_screen);
    lv_obj_remove_style_all(s_showcase.panel_gradient.lv_obj);
    lv_obj_set_pos(s_showcase.panel_gradient.lv_obj, 270, 52);
    lv_obj_set_size(s_showcase.panel_gradient.lv_obj, 200, 40);
    lv_obj_clear_flag(s_showcase.panel_gradient.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_showcase.panel_gradient.lv_obj, LV_SCROLLBAR_MODE_OFF);
    ui_apply_style(s_showcase.panel_gradient.lv_obj, UI_CHILD_PANEL, &s_showcase.panel_gradient.style);

    /* btn_shadow (button) */
    s_showcase.btn_shadow.lv_obj = lv_button_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.btn_shadow.lv_obj, 270, 100);
    lv_obj_set_size(s_showcase.btn_shadow.lv_obj, 200, 40);
    {
        lv_obj_t *lbl = lv_label_create(s_showcase.btn_shadow.lv_obj);
        lv_label_set_text(lbl, s_showcase.btn_shadow.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_showcase.btn_shadow.style);
    }
    lv_obj_add_event_cb(s_showcase.btn_shadow.lv_obj, ui_showcase_on_btn_shadow_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_showcase.btn_shadow.lv_obj, UI_CHILD_BUTTON, &s_showcase.btn_shadow.style);

    /* panel_row (panel) */
    s_showcase.panel_row.lv_obj = lv_obj_create(s_showcase.lv_screen);
    lv_obj_remove_style_all(s_showcase.panel_row.lv_obj);
    lv_obj_set_pos(s_showcase.panel_row.lv_obj, 270, 150);
    lv_obj_set_size(s_showcase.panel_row.lv_obj, 200, 44);
    lv_obj_clear_flag(s_showcase.panel_row.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_showcase.panel_row.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_showcase.panel_row.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_showcase.panel_row.lv_obj, UI_CHILD_PANEL, &s_showcase.panel_row.style);

    /* custom_font (label) */
    s_showcase.custom_font.lv_obj = lv_label_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.custom_font.lv_obj, 270, 204);
    lv_obj_set_width(s_showcase.custom_font.lv_obj, 78);
    lv_label_set_long_mode(s_showcase.custom_font.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_showcase.custom_font.lv_obj, s_showcase.custom_font.text);
    ui_apply_style(s_showcase.custom_font.lv_obj, UI_CHILD_LABEL, &s_showcase.custom_font.style);

    /* btn_icon (button) */
    s_showcase.btn_icon.lv_obj = lv_button_create(s_showcase.lv_screen);
    lv_obj_set_pos(s_showcase.btn_icon.lv_obj, 330, 236);
    lv_obj_set_size(s_showcase.btn_icon.lv_obj, 140, 36);
    {
        lv_obj_t *lbl = lv_label_create(s_showcase.btn_icon.lv_obj);
        lv_label_set_text(lbl, s_showcase.btn_icon.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_showcase.btn_icon.style);
    }
    lv_obj_add_event_cb(s_showcase.btn_icon.lv_obj, ui_showcase_on_btn_icon_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_showcase.btn_icon.lv_obj, UI_CHILD_BUTTON, &s_showcase.btn_icon.style);

    /* title (label) */
    s_showcase.panel_top.title.lv_obj = lv_label_create(s_showcase.panel_top.lv_obj);
    lv_obj_set_pos(s_showcase.panel_top.title.lv_obj, 12, 10);
    lv_obj_set_width(s_showcase.panel_top.title.lv_obj, 81);
    lv_label_set_long_mode(s_showcase.panel_top.title.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_showcase.panel_top.title.lv_obj, s_showcase.panel_top.title.text);
    ui_apply_style(s_showcase.panel_top.title.lv_obj, UI_CHILD_LABEL, &s_showcase.panel_top.title.style);

    /* icon_star (image) */
    s_showcase.panel_top.icon_star.lv_obj = lv_image_create(s_showcase.panel_top.lv_obj);
    lv_obj_set_pos(s_showcase.panel_top.icon_star.lv_obj, 444, 8);
    lv_obj_set_size(s_showcase.panel_top.icon_star.lv_obj, 24, 24);
    if (s_showcase.panel_top.icon_star.src)
        lv_image_set_src(s_showcase.panel_top.icon_star.lv_obj, s_showcase.panel_top.icon_star.src);
    ui_apply_style(s_showcase.panel_top.icon_star.lv_obj, UI_CHILD_IMAGE, &s_showcase.panel_top.icon_star.style);

    /* card_title (label) */
    s_showcase.panel_card.card_title.lv_obj = lv_label_create(s_showcase.panel_card.lv_obj);
    lv_obj_set_pos(s_showcase.panel_card.card_title.lv_obj, 10, 8);
    lv_obj_set_width(s_showcase.panel_card.card_title.lv_obj, 43);
    lv_label_set_long_mode(s_showcase.panel_card.card_title.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_showcase.panel_card.card_title.lv_obj, s_showcase.panel_card.card_title.text);
    ui_apply_style(s_showcase.panel_card.card_title.lv_obj, UI_CHILD_LABEL, &s_showcase.panel_card.card_title.style);

    /* card_body (label) */
    s_showcase.panel_card.card_body.lv_obj = lv_label_create(s_showcase.panel_card.lv_obj);
    lv_obj_set_pos(s_showcase.panel_card.card_body.lv_obj, 10, 36);
    lv_obj_set_width(s_showcase.panel_card.card_body.lv_obj, 70);
    lv_label_set_long_mode(s_showcase.panel_card.card_body.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_showcase.panel_card.card_body.lv_obj, s_showcase.panel_card.card_body.text);
    ui_apply_style(s_showcase.panel_card.card_body.lv_obj, UI_CHILD_LABEL, &s_showcase.panel_card.card_body.style);

    /* icon_heart (image) */
    s_showcase.panel_card.icon_heart.lv_obj = lv_image_create(s_showcase.panel_card.lv_obj);
    lv_obj_set_pos(s_showcase.panel_card.icon_heart.lv_obj, 176, 8);
    lv_obj_set_size(s_showcase.panel_card.icon_heart.lv_obj, 24, 24);
    if (s_showcase.panel_card.icon_heart.src)
        lv_image_set_src(s_showcase.panel_card.icon_heart.lv_obj, s_showcase.panel_card.icon_heart.src);
    ui_apply_style(s_showcase.panel_card.icon_heart.lv_obj, UI_CHILD_IMAGE, &s_showcase.panel_card.icon_heart.style);

    /* btn_fill_a (button) */
    s_showcase.panel_row.btn_fill_a.lv_obj = lv_button_create(s_showcase.panel_row.lv_obj);
    lv_obj_set_pos(s_showcase.panel_row.btn_fill_a.lv_obj, 6, 6);
    lv_obj_set_size(s_showcase.panel_row.btn_fill_a.lv_obj, 91, 32);
    {
        lv_obj_t *lbl = lv_label_create(s_showcase.panel_row.btn_fill_a.lv_obj);
        lv_label_set_text(lbl, s_showcase.panel_row.btn_fill_a.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_showcase.panel_row.btn_fill_a.style);
    }
    lv_obj_add_event_cb(s_showcase.panel_row.btn_fill_a.lv_obj, ui_showcase_on_btn_fill_a_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_showcase.panel_row.btn_fill_a.lv_obj, UI_CHILD_BUTTON, &s_showcase.panel_row.btn_fill_a.style);

    /* btn_fill_b (button) */
    s_showcase.panel_row.btn_fill_b.lv_obj = lv_button_create(s_showcase.panel_row.lv_obj);
    lv_obj_set_pos(s_showcase.panel_row.btn_fill_b.lv_obj, 103, 6);
    lv_obj_set_size(s_showcase.panel_row.btn_fill_b.lv_obj, 91, 32);
    {
        lv_obj_t *lbl = lv_label_create(s_showcase.panel_row.btn_fill_b.lv_obj);
        lv_label_set_text(lbl, s_showcase.panel_row.btn_fill_b.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_showcase.panel_row.btn_fill_b.style);
    }
    lv_obj_add_event_cb(s_showcase.panel_row.btn_fill_b.lv_obj, ui_showcase_on_btn_fill_b_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_showcase.panel_row.btn_fill_b.lv_obj, UI_CHILD_BUTTON, &s_showcase.panel_row.btn_fill_b.style);
}
