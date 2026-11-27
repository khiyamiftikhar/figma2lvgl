#include "ui_rules.h"
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
        char        text[UI_MAX_STRING_LENGTH];
    } caption;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_ok;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_next;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btnback;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_press;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_save;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_vol;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } volume_slider;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } brightness_slider;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_rev;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_flat;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } list_items;
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
    } batterybar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } icon_wifi;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } wifi_off;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_slot;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } title;
    } header;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_inner;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } toolbar_bg;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } btn_slider;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const char *label_text;
    } button_icon;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } bar_volume_slider;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } volume_slider_bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } list_icon;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } grid_bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } bar_icon;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } icon_bar;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_btn;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        int32_t value;
        int32_t min;
        int32_t max;
    } slider_panel;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
    } panel_2_3;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } time_click;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_shown;
    } box_with_hidden_child;
} s_rules = {
    .caption = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 12 }
        },
        .text = "Rules",
    },
    .btn_ok = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Ok",
    },
    .btn_next = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Btn Next",
    },
    .btnback = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Btnback",
    },
    .btn_press = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Press",
    },
    .btn_save = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Save Lp",
    },
    .slider_vol = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .volume_slider = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .brightness_slider = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 255,
    },
    .slider_rev = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 100,
        .max   = 0,
    },
    .slider_flat = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 5,
        .max   = 5,
    },
    .list_items = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .battery_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x4CAF50 }
        },
        .value = 0,
        .min   = -10,
        .max   = 90,
    },
    .batterybar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x4CAF50 }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .icon_wifi = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .wifi_off = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .panel_slot = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .header = {
        .style = {
            .box = { .has_bg = true, .bg = 0x3F51B5 }
        },
        .title = {
            .style = {
                .text = { .has_color = true, .color = 0xFFFFFF, .has_size = true, .size = 12 }
            },
            .text = "Head",
        },
    },
    .btn_inner = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Inner",
    },
    .toolbar_bg = {
        .style = {
            .box = { .has_bg = true, .bg = 0x607D8B }
        },
    },
    .btn_slider = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Slider",
    },
    .button_icon = {
        .style = {
            .box = { .has_bg = true, .bg = 0x2196F3 }
        },
        .label_text = "Icon",
    },
    .bar_volume_slider = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .volume_slider_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x4CAF50 }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .list_icon = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .grid_bar = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .bar_icon = {
        .style = {
            .box = { .has_bg = true, .bg = 0x4CAF50 }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .icon_bar = {
        .style = {
            .box = { .has_bg = true, .bg = 0x4CAF50 }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .panel_btn = {
        .style = {
            .box = { .has_bg = true, .bg = 0xCCCCCC }
        },
    },
    .slider_panel = {
        .style = {
            .box = { .has_bg = true, .bg = 0x9E9E9E }
        },
        .value = 0,
        .min   = 0,
        .max   = 100,
    },
    .panel_2_3 = {
        .style = {
            .box = { .has_bg = true, .bg = 0xCCCCCC }
        },
    },
    .time_click = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 12 }
        },
        .text = "12:00",
    },
    .box_with_hidden_child = {
        .style = {
            .box = { .has_bg = true, .bg = 0xEEEEEE }
        },
        .btn_shown = {
            .style = {
                .box = { .has_bg = true, .bg = 0x2196F3 }
            },
            .label_text = "Shown",
        },
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
void ui_rules_caption_set_text(const char *text)
{
    if (s_rules.caption.lv_obj)
        lv_label_set_text(s_rules.caption.lv_obj, text);
}

void ui_rules_btn_ok_set_label(const char *text)
{
    if (!s_rules.btn_ok.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.btn_ok.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_rules_btn_next_set_label(const char *text)
{
    if (!s_rules.btn_next.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.btn_next.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_rules_btnback_set_label(const char *text)
{
    if (!s_rules.btnback.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.btnback.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_rules_btn_press_set_label(const char *text)
{
    if (!s_rules.btn_press.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.btn_press.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_rules_btn_save_set_label(const char *text)
{
    if (!s_rules.btn_save.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.btn_save.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_rules_slider_vol_set_value(int32_t value)
{
    if (s_rules.slider_vol.lv_obj)
        lv_slider_set_value(s_rules.slider_vol.lv_obj, value, LV_ANIM_OFF);
}

void ui_rules_volume_slider_set_value(int32_t value)
{
    if (s_rules.volume_slider.lv_obj)
        lv_slider_set_value(s_rules.volume_slider.lv_obj, value, LV_ANIM_OFF);
}

void ui_rules_brightness_slider_set_value(int32_t value)
{
    if (s_rules.brightness_slider.lv_obj)
        lv_slider_set_value(s_rules.brightness_slider.lv_obj, value, LV_ANIM_OFF);
}

void ui_rules_slider_rev_set_value(int32_t value)
{
    if (s_rules.slider_rev.lv_obj)
        lv_slider_set_value(s_rules.slider_rev.lv_obj, value, LV_ANIM_OFF);
}

void ui_rules_slider_flat_set_value(int32_t value)
{
    if (s_rules.slider_flat.lv_obj)
        lv_slider_set_value(s_rules.slider_flat.lv_obj, value, LV_ANIM_OFF);
}

lv_obj_t *ui_rules_get_list_items(void)
{
    return s_rules.list_items.lv_obj;
}

void ui_rules_battery_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_rules.battery_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_rules.battery_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_rules.battery_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_rules.battery_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_rules_batterybar_set_value(int value, uint32_t duration_ms)
{
    if (!s_rules.batterybar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_rules.batterybar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_rules.batterybar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_rules.batterybar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_rules_icon_wifi_display(void)
{
    if (!s_rules.icon_wifi.lv_obj) return;
    s_rules.icon_wifi.src = &icon_wifi;
    lv_image_set_src(s_rules.icon_wifi.lv_obj, s_rules.icon_wifi.src);
}

void ui_rules_wifi_off_display(void)
{
    if (!s_rules.wifi_off.lv_obj) return;
    s_rules.wifi_off.src = &wifi_off;
    lv_image_set_src(s_rules.wifi_off.lv_obj, s_rules.wifi_off.src);
}

void ui_rules_btn_inner_set_label(const char *text)
{
    if (!s_rules.btn_inner.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.btn_inner.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_rules_btn_slider_set_label(const char *text)
{
    if (!s_rules.btn_slider.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.btn_slider.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_rules_button_icon_set_label(const char *text)
{
    if (!s_rules.button_icon.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.button_icon.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_rules_bar_volume_slider_set_value(int32_t value)
{
    if (s_rules.bar_volume_slider.lv_obj)
        lv_slider_set_value(s_rules.bar_volume_slider.lv_obj, value, LV_ANIM_OFF);
}

void ui_rules_volume_slider_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_rules.volume_slider_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_rules.volume_slider_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_rules.volume_slider_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_rules.volume_slider_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

lv_obj_t *ui_rules_get_list_icon(void)
{
    return s_rules.list_icon.lv_obj;
}

lv_obj_t *ui_rules_get_grid_bar(void)
{
    return s_rules.grid_bar.lv_obj;
}

void ui_rules_bar_icon_set_value(int value, uint32_t duration_ms)
{
    if (!s_rules.bar_icon.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_rules.bar_icon.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_rules.bar_icon.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_rules.bar_icon.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_rules_icon_bar_set_value(int value, uint32_t duration_ms)
{
    if (!s_rules.icon_bar.lv_obj) return;
    if (duration_ms == 0) {
        lv_bar_set_value(s_rules.icon_bar.lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, s_rules.icon_bar.lv_obj);
    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);
    lv_anim_set_values(&anim, lv_bar_get_value(s_rules.icon_bar.lv_obj), value);
    lv_anim_set_time(&anim, duration_ms);
    lv_anim_start(&anim);
}

void ui_rules_slider_panel_set_value(int32_t value)
{
    if (s_rules.slider_panel.lv_obj)
        lv_slider_set_value(s_rules.slider_panel.lv_obj, value, LV_ANIM_OFF);
}

void ui_rules_time_click_set_text(const char *text)
{
    if (s_rules.time_click.lv_obj)
        lv_label_set_text(s_rules.time_click.lv_obj, text);
}

void ui_rules_header_title_set_text(const char *text)
{
    if (s_rules.header.title.lv_obj)
        lv_label_set_text(s_rules.header.title.lv_obj, text);
}

void ui_rules_box_with_hidden_child_btn_shown_set_label(const char *text)
{
    if (!s_rules.box_with_hidden_child.btn_shown.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_rules.box_with_hidden_child.btn_shown.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_rules_load(void)
{
    lv_scr_load(s_rules.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_rules_init(void)
{
    s_rules.lv_screen = lv_obj_create(NULL);

    /* caption (label) */
    s_rules.caption.lv_obj = lv_label_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.caption.lv_obj, 4, 4);
    lv_obj_set_width(s_rules.caption.lv_obj, 80);
    lv_label_set_long_mode(s_rules.caption.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_rules.caption.lv_obj, s_rules.caption.text);
    ui_apply_style(s_rules.caption.lv_obj, UI_CHILD_LABEL, &s_rules.caption.style);

    /* btn_ok (button) */
    s_rules.btn_ok.lv_obj = lv_button_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.btn_ok.lv_obj, 4, 24);
    lv_obj_set_size(s_rules.btn_ok.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.btn_ok.lv_obj);
        lv_label_set_text(lbl, s_rules.btn_ok.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.btn_ok.style);
    }
    lv_obj_add_event_cb(s_rules.btn_ok.lv_obj, ui_rules_on_btn_ok_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_rules.btn_ok.lv_obj, UI_CHILD_BUTTON, &s_rules.btn_ok.style);

    /* btn_next (button) */
    s_rules.btn_next.lv_obj = lv_button_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.btn_next.lv_obj, 48, 24);
    lv_obj_set_size(s_rules.btn_next.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.btn_next.lv_obj);
        lv_label_set_text(lbl, s_rules.btn_next.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.btn_next.style);
    }
    lv_obj_add_event_cb(s_rules.btn_next.lv_obj, ui_rules_on_btn_next_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_rules.btn_next.lv_obj, UI_CHILD_BUTTON, &s_rules.btn_next.style);

    /* btnback (button) */
    s_rules.btnback.lv_obj = lv_button_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.btnback.lv_obj, 92, 24);
    lv_obj_set_size(s_rules.btnback.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.btnback.lv_obj);
        lv_label_set_text(lbl, s_rules.btnback.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.btnback.style);
    }
    lv_obj_add_event_cb(s_rules.btnback.lv_obj, ui_rules_on_btnback_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_rules.btnback.lv_obj, UI_CHILD_BUTTON, &s_rules.btnback.style);

    /* btn_press (button) */
    s_rules.btn_press.lv_obj = lv_button_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.btn_press.lv_obj, 136, 24);
    lv_obj_set_size(s_rules.btn_press.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.btn_press.lv_obj);
        lv_label_set_text(lbl, s_rules.btn_press.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.btn_press.style);
    }
    lv_obj_add_event_cb(s_rules.btn_press.lv_obj, ui_rules_on_btn_press_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_rules.btn_press.lv_obj, UI_CHILD_BUTTON, &s_rules.btn_press.style);

    /* btn_save (button) */
    s_rules.btn_save.lv_obj = lv_button_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.btn_save.lv_obj, 180, 24);
    lv_obj_set_size(s_rules.btn_save.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.btn_save.lv_obj);
        lv_label_set_text(lbl, s_rules.btn_save.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.btn_save.style);
    }
    lv_obj_add_event_cb(s_rules.btn_save.lv_obj, ui_rules_on_btn_save_clicked,
                        LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(s_rules.btn_save.lv_obj, ui_rules_on_btn_save_long_pressed,
                        LV_EVENT_LONG_PRESSED, NULL);
    ui_apply_style(s_rules.btn_save.lv_obj, UI_CHILD_BUTTON, &s_rules.btn_save.style);

    /* slider_vol (slider) */
    s_rules.slider_vol.lv_obj = lv_slider_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.slider_vol.lv_obj, 4, 48);
    lv_obj_set_size(s_rules.slider_vol.lv_obj, 100, 10);
    lv_slider_set_range(s_rules.slider_vol.lv_obj, s_rules.slider_vol.min, s_rules.slider_vol.max);
    lv_slider_set_value(s_rules.slider_vol.lv_obj, s_rules.slider_vol.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_rules.slider_vol.lv_obj, ui_rules_on_slider_vol,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_rules.slider_vol.lv_obj, UI_CHILD_SLIDER, &s_rules.slider_vol.style);

    /* volume_slider (slider) */
    s_rules.volume_slider.lv_obj = lv_slider_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.volume_slider.lv_obj, 108, 48);
    lv_obj_set_size(s_rules.volume_slider.lv_obj, 100, 10);
    lv_slider_set_range(s_rules.volume_slider.lv_obj, s_rules.volume_slider.min, s_rules.volume_slider.max);
    lv_slider_set_value(s_rules.volume_slider.lv_obj, s_rules.volume_slider.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_rules.volume_slider.lv_obj, ui_rules_on_volume_slider,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_rules.volume_slider.lv_obj, UI_CHILD_SLIDER, &s_rules.volume_slider.style);

    /* brightness_slider (slider) */
    s_rules.brightness_slider.lv_obj = lv_slider_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.brightness_slider.lv_obj, 4, 62);
    lv_obj_set_size(s_rules.brightness_slider.lv_obj, 100, 10);
    lv_slider_set_range(s_rules.brightness_slider.lv_obj, s_rules.brightness_slider.min, s_rules.brightness_slider.max);
    lv_slider_set_value(s_rules.brightness_slider.lv_obj, s_rules.brightness_slider.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_rules.brightness_slider.lv_obj, ui_rules_on_brightness_slider,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_rules.brightness_slider.lv_obj, UI_CHILD_SLIDER, &s_rules.brightness_slider.style);

    /* slider_rev (slider) */
    s_rules.slider_rev.lv_obj = lv_slider_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.slider_rev.lv_obj, 108, 62);
    lv_obj_set_size(s_rules.slider_rev.lv_obj, 100, 10);
    lv_slider_set_range(s_rules.slider_rev.lv_obj, s_rules.slider_rev.min, s_rules.slider_rev.max);
    lv_slider_set_value(s_rules.slider_rev.lv_obj, s_rules.slider_rev.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_rules.slider_rev.lv_obj, ui_rules_on_slider_rev,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_rules.slider_rev.lv_obj, UI_CHILD_SLIDER, &s_rules.slider_rev.style);

    /* slider_flat (slider) */
    s_rules.slider_flat.lv_obj = lv_slider_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.slider_flat.lv_obj, 212, 62);
    lv_obj_set_size(s_rules.slider_flat.lv_obj, 100, 10);
    lv_slider_set_range(s_rules.slider_flat.lv_obj, s_rules.slider_flat.min, s_rules.slider_flat.max);
    lv_slider_set_value(s_rules.slider_flat.lv_obj, s_rules.slider_flat.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_rules.slider_flat.lv_obj, ui_rules_on_slider_flat,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_rules.slider_flat.lv_obj, UI_CHILD_SLIDER, &s_rules.slider_flat.style);

    /* list_items (dynamic container — firmware fills at runtime) */
    s_rules.list_items.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.list_items.lv_obj, 4, 76);
    lv_obj_set_size(s_rules.list_items.lv_obj, 100, 40);
    lv_obj_set_scrollbar_mode(s_rules.list_items.lv_obj, LV_SCROLLBAR_MODE_AUTO);
    ui_apply_style(s_rules.list_items.lv_obj, UI_CHILD_PANEL, &s_rules.list_items.style);

    /* battery_bar (bar) */
    s_rules.battery_bar.lv_obj = lv_bar_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.battery_bar.lv_obj, 108, 76);
    lv_obj_set_size(s_rules.battery_bar.lv_obj, 100, 10);
    lv_bar_set_range(s_rules.battery_bar.lv_obj, s_rules.battery_bar.min, s_rules.battery_bar.max);
    lv_bar_set_value(s_rules.battery_bar.lv_obj, s_rules.battery_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_rules.battery_bar.lv_obj, UI_CHILD_BAR, &s_rules.battery_bar.style);

    /* batterybar (bar) */
    s_rules.batterybar.lv_obj = lv_bar_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.batterybar.lv_obj, 108, 90);
    lv_obj_set_size(s_rules.batterybar.lv_obj, 100, 10);
    lv_bar_set_range(s_rules.batterybar.lv_obj, s_rules.batterybar.min, s_rules.batterybar.max);
    lv_bar_set_value(s_rules.batterybar.lv_obj, s_rules.batterybar.value, LV_ANIM_OFF);
    ui_apply_style(s_rules.batterybar.lv_obj, UI_CHILD_BAR, &s_rules.batterybar.style);

    /* icon_wifi (image) */
    s_rules.icon_wifi.lv_obj = lv_image_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.icon_wifi.lv_obj, 212, 76);
    lv_obj_set_size(s_rules.icon_wifi.lv_obj, 48, 48);
    if (s_rules.icon_wifi.src)
        lv_image_set_src(s_rules.icon_wifi.lv_obj, s_rules.icon_wifi.src);
    ui_apply_style(s_rules.icon_wifi.lv_obj, UI_CHILD_IMAGE, &s_rules.icon_wifi.style);

    /* wifi_off (image) */
    s_rules.wifi_off.lv_obj = lv_image_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.wifi_off.lv_obj, 20, 120);
    lv_obj_set_size(s_rules.wifi_off.lv_obj, 48, 48);
    if (s_rules.wifi_off.src)
        lv_image_set_src(s_rules.wifi_off.lv_obj, s_rules.wifi_off.src);
    ui_apply_style(s_rules.wifi_off.lv_obj, UI_CHILD_IMAGE, &s_rules.wifi_off.style);

    /* panel_slot (panel) */
    s_rules.panel_slot.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_remove_style_all(s_rules.panel_slot.lv_obj);
    lv_obj_set_pos(s_rules.panel_slot.lv_obj, 136, 120);
    lv_obj_set_size(s_rules.panel_slot.lv_obj, 60, 30);
    lv_obj_clear_flag(s_rules.panel_slot.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_rules.panel_slot.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_rules.panel_slot.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    ui_apply_style(s_rules.panel_slot.lv_obj, UI_CHILD_PANEL, &s_rules.panel_slot.style);

    /* header (panel) */
    s_rules.header.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_remove_style_all(s_rules.header.lv_obj);
    lv_obj_set_pos(s_rules.header.lv_obj, 200, 120);
    lv_obj_set_size(s_rules.header.lv_obj, 100, 30);
    lv_obj_clear_flag(s_rules.header.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_rules.header.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_rules.header.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_rules.header.lv_obj, UI_CHILD_PANEL, &s_rules.header.style);

    /* btn_inner (button) */
    s_rules.btn_inner.lv_obj = lv_button_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.btn_inner.lv_obj, 14, 182);
    lv_obj_set_size(s_rules.btn_inner.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.btn_inner.lv_obj);
        lv_label_set_text(lbl, s_rules.btn_inner.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.btn_inner.style);
    }
    lv_obj_add_event_cb(s_rules.btn_inner.lv_obj, ui_rules_on_btn_inner_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_rules.btn_inner.lv_obj, UI_CHILD_BUTTON, &s_rules.btn_inner.style);

    /* toolbar_bg (panel) */
    s_rules.toolbar_bg.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_remove_style_all(s_rules.toolbar_bg.lv_obj);
    lv_obj_set_pos(s_rules.toolbar_bg.lv_obj, 128, 172);
    lv_obj_set_size(s_rules.toolbar_bg.lv_obj, 120, 20);
    lv_obj_clear_flag(s_rules.toolbar_bg.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_rules.toolbar_bg.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_rules.toolbar_bg.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_rules.toolbar_bg.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_rules.toolbar_bg.lv_obj, UI_CHILD_PANEL, &s_rules.toolbar_bg.style);

    /* btn_slider (button) */
    s_rules.btn_slider.lv_obj = lv_button_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.btn_slider.lv_obj, 4, 216);
    lv_obj_set_size(s_rules.btn_slider.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.btn_slider.lv_obj);
        lv_label_set_text(lbl, s_rules.btn_slider.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.btn_slider.style);
    }
    lv_obj_add_event_cb(s_rules.btn_slider.lv_obj, ui_rules_on_btn_slider_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_rules.btn_slider.lv_obj, UI_CHILD_BUTTON, &s_rules.btn_slider.style);

    /* button_icon (button) */
    s_rules.button_icon.lv_obj = lv_button_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.button_icon.lv_obj, 48, 216);
    lv_obj_set_size(s_rules.button_icon.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.button_icon.lv_obj);
        lv_label_set_text(lbl, s_rules.button_icon.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.button_icon.style);
    }
    lv_obj_add_event_cb(s_rules.button_icon.lv_obj, ui_rules_on_button_icon_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_rules.button_icon.lv_obj, UI_CHILD_BUTTON, &s_rules.button_icon.style);

    /* bar_volume_slider (slider) */
    s_rules.bar_volume_slider.lv_obj = lv_slider_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.bar_volume_slider.lv_obj, 92, 216);
    lv_obj_set_size(s_rules.bar_volume_slider.lv_obj, 100, 10);
    lv_slider_set_range(s_rules.bar_volume_slider.lv_obj, s_rules.bar_volume_slider.min, s_rules.bar_volume_slider.max);
    lv_slider_set_value(s_rules.bar_volume_slider.lv_obj, s_rules.bar_volume_slider.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_rules.bar_volume_slider.lv_obj, ui_rules_on_bar_volume_slider,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_rules.bar_volume_slider.lv_obj, UI_CHILD_SLIDER, &s_rules.bar_volume_slider.style);

    /* volume_slider_bar (bar) */
    s_rules.volume_slider_bar.lv_obj = lv_bar_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.volume_slider_bar.lv_obj, 196, 216);
    lv_obj_set_size(s_rules.volume_slider_bar.lv_obj, 100, 10);
    lv_bar_set_range(s_rules.volume_slider_bar.lv_obj, s_rules.volume_slider_bar.min, s_rules.volume_slider_bar.max);
    lv_bar_set_value(s_rules.volume_slider_bar.lv_obj, s_rules.volume_slider_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_rules.volume_slider_bar.lv_obj, UI_CHILD_BAR, &s_rules.volume_slider_bar.style);

    /* list_icon (dynamic container — firmware fills at runtime) */
    s_rules.list_icon.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.list_icon.lv_obj, 4, 240);
    lv_obj_set_size(s_rules.list_icon.lv_obj, 60, 30);
    lv_obj_set_scrollbar_mode(s_rules.list_icon.lv_obj, LV_SCROLLBAR_MODE_AUTO);
    ui_apply_style(s_rules.list_icon.lv_obj, UI_CHILD_PANEL, &s_rules.list_icon.style);

    /* grid_bar (dynamic container — firmware fills at runtime) */
    s_rules.grid_bar.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.grid_bar.lv_obj, 68, 240);
    lv_obj_set_size(s_rules.grid_bar.lv_obj, 60, 30);
    lv_obj_set_scrollbar_mode(s_rules.grid_bar.lv_obj, LV_SCROLLBAR_MODE_AUTO);
    ui_apply_style(s_rules.grid_bar.lv_obj, UI_CHILD_PANEL, &s_rules.grid_bar.style);

    /* bar_icon (bar) */
    s_rules.bar_icon.lv_obj = lv_bar_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.bar_icon.lv_obj, 132, 240);
    lv_obj_set_size(s_rules.bar_icon.lv_obj, 60, 10);
    lv_bar_set_range(s_rules.bar_icon.lv_obj, s_rules.bar_icon.min, s_rules.bar_icon.max);
    lv_bar_set_value(s_rules.bar_icon.lv_obj, s_rules.bar_icon.value, LV_ANIM_OFF);
    ui_apply_style(s_rules.bar_icon.lv_obj, UI_CHILD_BAR, &s_rules.bar_icon.style);

    /* icon_bar (bar) */
    s_rules.icon_bar.lv_obj = lv_bar_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.icon_bar.lv_obj, 196, 240);
    lv_obj_set_size(s_rules.icon_bar.lv_obj, 60, 10);
    lv_bar_set_range(s_rules.icon_bar.lv_obj, s_rules.icon_bar.min, s_rules.icon_bar.max);
    lv_bar_set_value(s_rules.icon_bar.lv_obj, s_rules.icon_bar.value, LV_ANIM_OFF);
    ui_apply_style(s_rules.icon_bar.lv_obj, UI_CHILD_BAR, &s_rules.icon_bar.style);

    /* panel_btn (panel) */
    s_rules.panel_btn.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_remove_style_all(s_rules.panel_btn.lv_obj);
    lv_obj_set_pos(s_rules.panel_btn.lv_obj, 4, 274);
    lv_obj_set_size(s_rules.panel_btn.lv_obj, 60, 30);
    lv_obj_clear_flag(s_rules.panel_btn.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_rules.panel_btn.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_rules.panel_btn.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_rules.panel_btn.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_rules.panel_btn.lv_obj, UI_CHILD_PANEL, &s_rules.panel_btn.style);

    /* slider_panel (slider) */
    s_rules.slider_panel.lv_obj = lv_slider_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.slider_panel.lv_obj, 68, 274);
    lv_obj_set_size(s_rules.slider_panel.lv_obj, 100, 10);
    lv_slider_set_range(s_rules.slider_panel.lv_obj, s_rules.slider_panel.min, s_rules.slider_panel.max);
    lv_slider_set_value(s_rules.slider_panel.lv_obj, s_rules.slider_panel.value, LV_ANIM_OFF);
    lv_obj_add_event_cb(s_rules.slider_panel.lv_obj, ui_rules_on_slider_panel,
                        LV_EVENT_VALUE_CHANGED, NULL);
    ui_apply_style(s_rules.slider_panel.lv_obj, UI_CHILD_SLIDER, &s_rules.slider_panel.style);

    /* panel_2_3 (panel) */
    s_rules.panel_2_3.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_remove_style_all(s_rules.panel_2_3.lv_obj);
    lv_obj_set_pos(s_rules.panel_2_3.lv_obj, 172, 274);
    lv_obj_set_size(s_rules.panel_2_3.lv_obj, 60, 30);
    lv_obj_clear_flag(s_rules.panel_2_3.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_rules.panel_2_3.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_add_flag(s_rules.panel_2_3.lv_obj, LV_OBJ_FLAG_OVERFLOW_VISIBLE);
    lv_obj_set_style_bg_opa(s_rules.panel_2_3.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_rules.panel_2_3.lv_obj, UI_CHILD_PANEL, &s_rules.panel_2_3.style);

    /* time_click (label) */
    s_rules.time_click.lv_obj = lv_label_create(s_rules.lv_screen);
    lv_obj_set_pos(s_rules.time_click.lv_obj, 4, 308);
    lv_obj_set_width(s_rules.time_click.lv_obj, 80);
    lv_label_set_long_mode(s_rules.time_click.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_rules.time_click.lv_obj, s_rules.time_click.text);
    ui_apply_style(s_rules.time_click.lv_obj, UI_CHILD_LABEL, &s_rules.time_click.style);

    /* box_with_hidden_child (panel) */
    s_rules.box_with_hidden_child.lv_obj = lv_obj_create(s_rules.lv_screen);
    lv_obj_remove_style_all(s_rules.box_with_hidden_child.lv_obj);
    lv_obj_set_pos(s_rules.box_with_hidden_child.lv_obj, 132, 308);
    lv_obj_set_size(s_rules.box_with_hidden_child.lv_obj, 100, 40);
    lv_obj_clear_flag(s_rules.box_with_hidden_child.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_rules.box_with_hidden_child.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_rules.box_with_hidden_child.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_rules.box_with_hidden_child.lv_obj, UI_CHILD_PANEL, &s_rules.box_with_hidden_child.style);

    /* title (label) */
    s_rules.header.title.lv_obj = lv_label_create(s_rules.header.lv_obj);
    lv_obj_set_pos(s_rules.header.title.lv_obj, 4, 4);
    lv_obj_set_width(s_rules.header.title.lv_obj, 80);
    lv_label_set_long_mode(s_rules.header.title.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_rules.header.title.lv_obj, s_rules.header.title.text);
    ui_apply_style(s_rules.header.title.lv_obj, UI_CHILD_LABEL, &s_rules.header.title.style);

    /* btn_shown (button) */
    s_rules.box_with_hidden_child.btn_shown.lv_obj = lv_button_create(s_rules.box_with_hidden_child.lv_obj);
    lv_obj_set_pos(s_rules.box_with_hidden_child.btn_shown.lv_obj, 4, 4);
    lv_obj_set_size(s_rules.box_with_hidden_child.btn_shown.lv_obj, 40, 20);
    {
        lv_obj_t *lbl = lv_label_create(s_rules.box_with_hidden_child.btn_shown.lv_obj);
        lv_label_set_text(lbl, s_rules.box_with_hidden_child.btn_shown.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_rules.box_with_hidden_child.btn_shown.style);
    }
    lv_obj_add_event_cb(s_rules.box_with_hidden_child.btn_shown.lv_obj, ui_rules_on_btn_shown_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_rules.box_with_hidden_child.btn_shown.lv_obj, UI_CHILD_BUTTON, &s_rules.box_with_hidden_child.btn_shown.style);
}
