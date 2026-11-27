#include "ui_hostile.h"
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
    } label_2nd_row;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_default;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_class;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_1;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_2;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_style;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } label_lv_obj;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } title;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } title_3;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } title_2;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_MAX_STRING_LENGTH];
    } title_4;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_ok;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } title;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } label_text;
    } panel_a;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const char *label_text;
        } btn_ok;
    } panel_b;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            struct {
                lv_obj_t   *lv_obj;
                ui_style_t  style;
                const char *label_text;
            } btn_go;
        } panel_c;
    } x;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            struct {
                lv_obj_t   *lv_obj;
                ui_style_t  style;
                const char *label_text;
            } btn_go;
        } panel_c;
    } y;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } e_label;
    } panel_d;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_MAX_STRING_LENGTH];
        } label;
    } panel_d_e;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            struct {
                lv_obj_t   *lv_obj;
                ui_style_t  style;
                struct {
                    lv_obj_t   *lv_obj;
                    ui_style_t  style;
                    char        text[UI_MAX_STRING_LENGTH];
                } deep;
            } c;
        } b;
    } a;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } ui_style;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } icon_wifi;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } icon_wifi_2;
} s_hostile = {
    .label_2nd_row = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .label_default = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .label_class = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .label_1 = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .label_2 = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .label_style = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .label_lv_obj = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .title = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .title_3 = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .title_2 = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .title_4 = {
        .style = {
            .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
        },
        .text = "t",
    },
    .panel_a = {
        .style = {
            .box = { .has_bg = true, .bg = 0xDDDDDD }
        },
        .btn_ok = {
            .style = {
                .box = { .has_bg = true, .bg = 0x2196F3 }
            },
            .label_text = "Ok",
        },
        .title = {
            .style = {
                .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
            },
            .text = "t",
        },
        .label_text = {
            .style = {
                .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
            },
            .text = "t",
        },
    },
    .panel_b = {
        .style = {
            .box = { .has_bg = true, .bg = 0xDDDDDD }
        },
        .btn_ok = {
            .style = {
                .box = { .has_bg = true, .bg = 0x2196F3 }
            },
            .label_text = "Ok",
        },
    },
    .x = {
        .style = {
            .box = { .has_bg = true, .bg = 0xDDDDDD }
        },
        .panel_c = {
            .style = {
                .box = { .has_bg = true, .bg = 0xDDDDDD }
            },
            .btn_go = {
                .style = {
                    .box = { .has_bg = true, .bg = 0x2196F3 }
                },
                .label_text = "Go",
            },
        },
    },
    .y = {
        .style = {
            .box = { .has_bg = true, .bg = 0xDDDDDD }
        },
        .panel_c = {
            .style = {
                .box = { .has_bg = true, .bg = 0xDDDDDD }
            },
            .btn_go = {
                .style = {
                    .box = { .has_bg = true, .bg = 0x2196F3 }
                },
                .label_text = "Go",
            },
        },
    },
    .panel_d = {
        .style = {
            .box = { .has_bg = true, .bg = 0xDDDDDD }
        },
        .e_label = {
            .style = {
                .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
            },
            .text = "t",
        },
    },
    .panel_d_e = {
        .style = {
            .box = { .has_bg = true, .bg = 0xDDDDDD }
        },
        .label = {
            .style = {
                .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
            },
            .text = "t",
        },
    },
    .a = {
        .style = {
            .box = { .has_bg = true, .bg = 0xDDDDDD }
        },
        .b = {
            .style = {
                .box = { .has_bg = true, .bg = 0xDDDDDD }
            },
            .c = {
                .style = {
                    .box = { .has_bg = true, .bg = 0xDDDDDD }
                },
                .deep = {
                    .style = {
                        .text = { .has_color = true, .color = 0x000000, .has_size = true, .size = 10 }
                    },
                    .text = "t",
                },
            },
        },
    },
    .ui_style = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .icon_wifi = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .icon_wifi_2 = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
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
void ui_hostile_label_2nd_row_set_text(const char *text)
{
    if (s_hostile.label_2nd_row.lv_obj)
        lv_label_set_text(s_hostile.label_2nd_row.lv_obj, text);
}

void ui_hostile_label_default_set_text(const char *text)
{
    if (s_hostile.label_default.lv_obj)
        lv_label_set_text(s_hostile.label_default.lv_obj, text);
}

void ui_hostile_label_class_set_text(const char *text)
{
    if (s_hostile.label_class.lv_obj)
        lv_label_set_text(s_hostile.label_class.lv_obj, text);
}

void ui_hostile_label_1_set_text(const char *text)
{
    if (s_hostile.label_1.lv_obj)
        lv_label_set_text(s_hostile.label_1.lv_obj, text);
}

void ui_hostile_label_2_set_text(const char *text)
{
    if (s_hostile.label_2.lv_obj)
        lv_label_set_text(s_hostile.label_2.lv_obj, text);
}

void ui_hostile_label_style_set_text(const char *text)
{
    if (s_hostile.label_style.lv_obj)
        lv_label_set_text(s_hostile.label_style.lv_obj, text);
}

void ui_hostile_label_lv_obj_set_text(const char *text)
{
    if (s_hostile.label_lv_obj.lv_obj)
        lv_label_set_text(s_hostile.label_lv_obj.lv_obj, text);
}

void ui_hostile_title_set_text(const char *text)
{
    if (s_hostile.title.lv_obj)
        lv_label_set_text(s_hostile.title.lv_obj, text);
}

void ui_hostile_title_3_set_text(const char *text)
{
    if (s_hostile.title_3.lv_obj)
        lv_label_set_text(s_hostile.title_3.lv_obj, text);
}

void ui_hostile_title_2_set_text(const char *text)
{
    if (s_hostile.title_2.lv_obj)
        lv_label_set_text(s_hostile.title_2.lv_obj, text);
}

void ui_hostile_title_4_set_text(const char *text)
{
    if (s_hostile.title_4.lv_obj)
        lv_label_set_text(s_hostile.title_4.lv_obj, text);
}

void ui_hostile_ui_style_display(void)
{
    if (!s_hostile.ui_style.lv_obj) return;
    s_hostile.ui_style.src = &image_ui_style;
    lv_image_set_src(s_hostile.ui_style.lv_obj, s_hostile.ui_style.src);
}

void ui_hostile_icon_wifi_display(void)
{
    if (!s_hostile.icon_wifi.lv_obj) return;
    s_hostile.icon_wifi.src = &icon_wifi;
    lv_image_set_src(s_hostile.icon_wifi.lv_obj, s_hostile.icon_wifi.src);
}

void ui_hostile_icon_wifi_2_display(void)
{
    if (!s_hostile.icon_wifi_2.lv_obj) return;
    s_hostile.icon_wifi_2.src = &icon_wifi;
    lv_image_set_src(s_hostile.icon_wifi_2.lv_obj, s_hostile.icon_wifi_2.src);
}

void ui_hostile_panel_a_btn_ok_set_label(const char *text)
{
    if (!s_hostile.panel_a.btn_ok.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_hostile.panel_a.btn_ok.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_hostile_panel_a_title_set_text(const char *text)
{
    if (s_hostile.panel_a.title.lv_obj)
        lv_label_set_text(s_hostile.panel_a.title.lv_obj, text);
}

void ui_hostile_panel_a_label_text_set_text(const char *text)
{
    if (s_hostile.panel_a.label_text.lv_obj)
        lv_label_set_text(s_hostile.panel_a.label_text.lv_obj, text);
}

void ui_hostile_panel_b_btn_ok_set_label(const char *text)
{
    if (!s_hostile.panel_b.btn_ok.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_hostile.panel_b.btn_ok.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_hostile_panel_d_e_label_set_text(const char *text)
{
    if (s_hostile.panel_d.e_label.lv_obj)
        lv_label_set_text(s_hostile.panel_d.e_label.lv_obj, text);
}

void ui_hostile_panel_d_e_label_set_text_2(const char *text)
{
    if (s_hostile.panel_d_e.label.lv_obj)
        lv_label_set_text(s_hostile.panel_d_e.label.lv_obj, text);
}

void ui_hostile_x_panel_c_btn_go_set_label(const char *text)
{
    if (!s_hostile.x.panel_c.btn_go.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_hostile.x.panel_c.btn_go.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_hostile_y_panel_c_btn_go_set_label(const char *text)
{
    if (!s_hostile.y.panel_c.btn_go.lv_obj) return;
    lv_obj_t *lbl = lv_obj_get_child(s_hostile.y.panel_c.btn_go.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, text);
}

void ui_hostile_a_b_c_deep_set_text(const char *text)
{
    if (s_hostile.a.b.c.deep.lv_obj)
        lv_label_set_text(s_hostile.a.b.c.deep.lv_obj, text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_hostile_load(void)
{
    lv_scr_load(s_hostile.lv_screen);
}

// ------------------------------
// SCREEN INIT
// ------------------------------
void ui_hostile_init(void)
{
    s_hostile.lv_screen = lv_obj_create(NULL);

    /* label_2nd_row (label) */
    s_hostile.label_2nd_row.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.label_2nd_row.lv_obj, 4, 2);
    lv_obj_set_width(s_hostile.label_2nd_row.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.label_2nd_row.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.label_2nd_row.lv_obj, s_hostile.label_2nd_row.text);
    ui_apply_style(s_hostile.label_2nd_row.lv_obj, UI_CHILD_LABEL, &s_hostile.label_2nd_row.style);

    /* label_default (label) */
    s_hostile.label_default.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.label_default.lv_obj, 4, 18);
    lv_obj_set_width(s_hostile.label_default.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.label_default.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.label_default.lv_obj, s_hostile.label_default.text);
    ui_apply_style(s_hostile.label_default.lv_obj, UI_CHILD_LABEL, &s_hostile.label_default.style);

    /* label_class (label) */
    s_hostile.label_class.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.label_class.lv_obj, 4, 34);
    lv_obj_set_width(s_hostile.label_class.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.label_class.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.label_class.lv_obj, s_hostile.label_class.text);
    ui_apply_style(s_hostile.label_class.lv_obj, UI_CHILD_LABEL, &s_hostile.label_class.style);

    /* label_1 (label) */
    s_hostile.label_1.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.label_1.lv_obj, 4, 50);
    lv_obj_set_width(s_hostile.label_1.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.label_1.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.label_1.lv_obj, s_hostile.label_1.text);
    ui_apply_style(s_hostile.label_1.lv_obj, UI_CHILD_LABEL, &s_hostile.label_1.style);

    /* label_2 (label) */
    s_hostile.label_2.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.label_2.lv_obj, 4, 66);
    lv_obj_set_width(s_hostile.label_2.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.label_2.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.label_2.lv_obj, s_hostile.label_2.text);
    ui_apply_style(s_hostile.label_2.lv_obj, UI_CHILD_LABEL, &s_hostile.label_2.style);

    /* label_style (label) */
    s_hostile.label_style.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.label_style.lv_obj, 4, 82);
    lv_obj_set_width(s_hostile.label_style.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.label_style.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.label_style.lv_obj, s_hostile.label_style.text);
    ui_apply_style(s_hostile.label_style.lv_obj, UI_CHILD_LABEL, &s_hostile.label_style.style);

    /* label_lv_obj (label) */
    s_hostile.label_lv_obj.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.label_lv_obj.lv_obj, 4, 98);
    lv_obj_set_width(s_hostile.label_lv_obj.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.label_lv_obj.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.label_lv_obj.lv_obj, s_hostile.label_lv_obj.text);
    ui_apply_style(s_hostile.label_lv_obj.lv_obj, UI_CHILD_LABEL, &s_hostile.label_lv_obj.style);

    /* title (label) */
    s_hostile.title.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.title.lv_obj, 4, 114);
    lv_obj_set_width(s_hostile.title.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.title.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.title.lv_obj, s_hostile.title.text);
    ui_apply_style(s_hostile.title.lv_obj, UI_CHILD_LABEL, &s_hostile.title.style);

    /* title_3 (label) */
    s_hostile.title_3.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.title_3.lv_obj, 4, 130);
    lv_obj_set_width(s_hostile.title_3.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.title_3.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.title_3.lv_obj, s_hostile.title_3.text);
    ui_apply_style(s_hostile.title_3.lv_obj, UI_CHILD_LABEL, &s_hostile.title_3.style);

    /* title_2 (label) */
    s_hostile.title_2.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.title_2.lv_obj, 4, 146);
    lv_obj_set_width(s_hostile.title_2.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.title_2.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.title_2.lv_obj, s_hostile.title_2.text);
    ui_apply_style(s_hostile.title_2.lv_obj, UI_CHILD_LABEL, &s_hostile.title_2.style);

    /* title_4 (label) */
    s_hostile.title_4.lv_obj = lv_label_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.title_4.lv_obj, 4, 162);
    lv_obj_set_width(s_hostile.title_4.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.title_4.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.title_4.lv_obj, s_hostile.title_4.text);
    ui_apply_style(s_hostile.title_4.lv_obj, UI_CHILD_LABEL, &s_hostile.title_4.style);

    /* panel_a (panel) */
    s_hostile.panel_a.lv_obj = lv_obj_create(s_hostile.lv_screen);
    lv_obj_remove_style_all(s_hostile.panel_a.lv_obj);
    lv_obj_set_pos(s_hostile.panel_a.lv_obj, 100, 2);
    lv_obj_set_size(s_hostile.panel_a.lv_obj, 100, 60);
    lv_obj_clear_flag(s_hostile.panel_a.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.panel_a.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.panel_a.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.panel_a.lv_obj, UI_CHILD_PANEL, &s_hostile.panel_a.style);

    /* panel_b (panel) */
    s_hostile.panel_b.lv_obj = lv_obj_create(s_hostile.lv_screen);
    lv_obj_remove_style_all(s_hostile.panel_b.lv_obj);
    lv_obj_set_pos(s_hostile.panel_b.lv_obj, 210, 2);
    lv_obj_set_size(s_hostile.panel_b.lv_obj, 100, 60);
    lv_obj_clear_flag(s_hostile.panel_b.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.panel_b.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.panel_b.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.panel_b.lv_obj, UI_CHILD_PANEL, &s_hostile.panel_b.style);

    /* x (panel) */
    s_hostile.x.lv_obj = lv_obj_create(s_hostile.lv_screen);
    lv_obj_remove_style_all(s_hostile.x.lv_obj);
    lv_obj_set_pos(s_hostile.x.lv_obj, 100, 70);
    lv_obj_set_size(s_hostile.x.lv_obj, 100, 40);
    lv_obj_clear_flag(s_hostile.x.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.x.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.x.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.x.lv_obj, UI_CHILD_PANEL, &s_hostile.x.style);

    /* y (panel) */
    s_hostile.y.lv_obj = lv_obj_create(s_hostile.lv_screen);
    lv_obj_remove_style_all(s_hostile.y.lv_obj);
    lv_obj_set_pos(s_hostile.y.lv_obj, 210, 70);
    lv_obj_set_size(s_hostile.y.lv_obj, 100, 40);
    lv_obj_clear_flag(s_hostile.y.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.y.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.y.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.y.lv_obj, UI_CHILD_PANEL, &s_hostile.y.style);

    /* panel_d (panel) */
    s_hostile.panel_d.lv_obj = lv_obj_create(s_hostile.lv_screen);
    lv_obj_remove_style_all(s_hostile.panel_d.lv_obj);
    lv_obj_set_pos(s_hostile.panel_d.lv_obj, 100, 120);
    lv_obj_set_size(s_hostile.panel_d.lv_obj, 100, 30);
    lv_obj_clear_flag(s_hostile.panel_d.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.panel_d.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.panel_d.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.panel_d.lv_obj, UI_CHILD_PANEL, &s_hostile.panel_d.style);

    /* panel_d_e (panel) */
    s_hostile.panel_d_e.lv_obj = lv_obj_create(s_hostile.lv_screen);
    lv_obj_remove_style_all(s_hostile.panel_d_e.lv_obj);
    lv_obj_set_pos(s_hostile.panel_d_e.lv_obj, 210, 120);
    lv_obj_set_size(s_hostile.panel_d_e.lv_obj, 100, 30);
    lv_obj_clear_flag(s_hostile.panel_d_e.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.panel_d_e.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.panel_d_e.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.panel_d_e.lv_obj, UI_CHILD_PANEL, &s_hostile.panel_d_e.style);

    /* a (panel) */
    s_hostile.a.lv_obj = lv_obj_create(s_hostile.lv_screen);
    lv_obj_remove_style_all(s_hostile.a.lv_obj);
    lv_obj_set_pos(s_hostile.a.lv_obj, 100, 160);
    lv_obj_set_size(s_hostile.a.lv_obj, 200, 80);
    lv_obj_clear_flag(s_hostile.a.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.a.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.a.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.a.lv_obj, UI_CHILD_PANEL, &s_hostile.a.style);

    /* ui_style (image) */
    s_hostile.ui_style.lv_obj = lv_image_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.ui_style.lv_obj, 4, 250);
    lv_obj_set_size(s_hostile.ui_style.lv_obj, 48, 48);
    if (s_hostile.ui_style.src)
        lv_image_set_src(s_hostile.ui_style.lv_obj, s_hostile.ui_style.src);
    ui_apply_style(s_hostile.ui_style.lv_obj, UI_CHILD_IMAGE, &s_hostile.ui_style.style);

    /* icon_wifi (image) */
    s_hostile.icon_wifi.lv_obj = lv_image_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.icon_wifi.lv_obj, 60, 250);
    lv_obj_set_size(s_hostile.icon_wifi.lv_obj, 48, 48);
    if (s_hostile.icon_wifi.src)
        lv_image_set_src(s_hostile.icon_wifi.lv_obj, s_hostile.icon_wifi.src);
    ui_apply_style(s_hostile.icon_wifi.lv_obj, UI_CHILD_IMAGE, &s_hostile.icon_wifi.style);

    /* icon_wifi_2 (image) */
    s_hostile.icon_wifi_2.lv_obj = lv_image_create(s_hostile.lv_screen);
    lv_obj_set_pos(s_hostile.icon_wifi_2.lv_obj, 116, 250);
    lv_obj_set_size(s_hostile.icon_wifi_2.lv_obj, 48, 48);
    if (s_hostile.icon_wifi_2.src)
        lv_image_set_src(s_hostile.icon_wifi_2.lv_obj, s_hostile.icon_wifi_2.src);
    ui_apply_style(s_hostile.icon_wifi_2.lv_obj, UI_CHILD_IMAGE, &s_hostile.icon_wifi_2.style);

    /* btn_ok (button) */
    s_hostile.panel_a.btn_ok.lv_obj = lv_button_create(s_hostile.panel_a.lv_obj);
    lv_obj_set_pos(s_hostile.panel_a.btn_ok.lv_obj, 2, 2);
    lv_obj_set_size(s_hostile.panel_a.btn_ok.lv_obj, 40, 18);
    {
        lv_obj_t *lbl = lv_label_create(s_hostile.panel_a.btn_ok.lv_obj);
        lv_label_set_text(lbl, s_hostile.panel_a.btn_ok.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_hostile.panel_a.btn_ok.style);
    }
    lv_obj_add_event_cb(s_hostile.panel_a.btn_ok.lv_obj, ui_hostile_on_panel_a_btn_ok_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_hostile.panel_a.btn_ok.lv_obj, UI_CHILD_BUTTON, &s_hostile.panel_a.btn_ok.style);

    /* title (label) */
    s_hostile.panel_a.title.lv_obj = lv_label_create(s_hostile.panel_a.lv_obj);
    lv_obj_set_pos(s_hostile.panel_a.title.lv_obj, 4, 24);
    lv_obj_set_width(s_hostile.panel_a.title.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.panel_a.title.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.panel_a.title.lv_obj, s_hostile.panel_a.title.text);
    ui_apply_style(s_hostile.panel_a.title.lv_obj, UI_CHILD_LABEL, &s_hostile.panel_a.title.style);

    /* label_text (label) */
    s_hostile.panel_a.label_text.lv_obj = lv_label_create(s_hostile.panel_a.lv_obj);
    lv_obj_set_pos(s_hostile.panel_a.label_text.lv_obj, 4, 40);
    lv_obj_set_width(s_hostile.panel_a.label_text.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.panel_a.label_text.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.panel_a.label_text.lv_obj, s_hostile.panel_a.label_text.text);
    ui_apply_style(s_hostile.panel_a.label_text.lv_obj, UI_CHILD_LABEL, &s_hostile.panel_a.label_text.style);

    /* btn_ok (button) */
    s_hostile.panel_b.btn_ok.lv_obj = lv_button_create(s_hostile.panel_b.lv_obj);
    lv_obj_set_pos(s_hostile.panel_b.btn_ok.lv_obj, 2, 2);
    lv_obj_set_size(s_hostile.panel_b.btn_ok.lv_obj, 40, 18);
    {
        lv_obj_t *lbl = lv_label_create(s_hostile.panel_b.btn_ok.lv_obj);
        lv_label_set_text(lbl, s_hostile.panel_b.btn_ok.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_hostile.panel_b.btn_ok.style);
    }
    lv_obj_add_event_cb(s_hostile.panel_b.btn_ok.lv_obj, ui_hostile_on_panel_b_btn_ok_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_hostile.panel_b.btn_ok.lv_obj, UI_CHILD_BUTTON, &s_hostile.panel_b.btn_ok.style);

    /* panel_c (panel) */
    s_hostile.x.panel_c.lv_obj = lv_obj_create(s_hostile.x.lv_obj);
    lv_obj_remove_style_all(s_hostile.x.panel_c.lv_obj);
    lv_obj_set_pos(s_hostile.x.panel_c.lv_obj, 2, 2);
    lv_obj_set_size(s_hostile.x.panel_c.lv_obj, 90, 30);
    lv_obj_clear_flag(s_hostile.x.panel_c.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.x.panel_c.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.x.panel_c.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.x.panel_c.lv_obj, UI_CHILD_PANEL, &s_hostile.x.panel_c.style);

    /* panel_c (panel) */
    s_hostile.y.panel_c.lv_obj = lv_obj_create(s_hostile.y.lv_obj);
    lv_obj_remove_style_all(s_hostile.y.panel_c.lv_obj);
    lv_obj_set_pos(s_hostile.y.panel_c.lv_obj, 2, 2);
    lv_obj_set_size(s_hostile.y.panel_c.lv_obj, 90, 30);
    lv_obj_clear_flag(s_hostile.y.panel_c.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.y.panel_c.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.y.panel_c.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.y.panel_c.lv_obj, UI_CHILD_PANEL, &s_hostile.y.panel_c.style);

    /* e_label (label) */
    s_hostile.panel_d.e_label.lv_obj = lv_label_create(s_hostile.panel_d.lv_obj);
    lv_obj_set_pos(s_hostile.panel_d.e_label.lv_obj, 4, 2);
    lv_obj_set_width(s_hostile.panel_d.e_label.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.panel_d.e_label.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.panel_d.e_label.lv_obj, s_hostile.panel_d.e_label.text);
    ui_apply_style(s_hostile.panel_d.e_label.lv_obj, UI_CHILD_LABEL, &s_hostile.panel_d.e_label.style);

    /* label (label) */
    s_hostile.panel_d_e.label.lv_obj = lv_label_create(s_hostile.panel_d_e.lv_obj);
    lv_obj_set_pos(s_hostile.panel_d_e.label.lv_obj, 4, 2);
    lv_obj_set_width(s_hostile.panel_d_e.label.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.panel_d_e.label.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.panel_d_e.label.lv_obj, s_hostile.panel_d_e.label.text);
    ui_apply_style(s_hostile.panel_d_e.label.lv_obj, UI_CHILD_LABEL, &s_hostile.panel_d_e.label.style);

    /* b (panel) */
    s_hostile.a.b.lv_obj = lv_obj_create(s_hostile.a.lv_obj);
    lv_obj_remove_style_all(s_hostile.a.b.lv_obj);
    lv_obj_set_pos(s_hostile.a.b.lv_obj, 2, 2);
    lv_obj_set_size(s_hostile.a.b.lv_obj, 190, 70);
    lv_obj_clear_flag(s_hostile.a.b.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.a.b.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.a.b.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.a.b.lv_obj, UI_CHILD_PANEL, &s_hostile.a.b.style);

    /* btn_go (button) */
    s_hostile.x.panel_c.btn_go.lv_obj = lv_button_create(s_hostile.x.panel_c.lv_obj);
    lv_obj_set_pos(s_hostile.x.panel_c.btn_go.lv_obj, 2, 2);
    lv_obj_set_size(s_hostile.x.panel_c.btn_go.lv_obj, 40, 18);
    {
        lv_obj_t *lbl = lv_label_create(s_hostile.x.panel_c.btn_go.lv_obj);
        lv_label_set_text(lbl, s_hostile.x.panel_c.btn_go.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_hostile.x.panel_c.btn_go.style);
    }
    lv_obj_add_event_cb(s_hostile.x.panel_c.btn_go.lv_obj, ui_hostile_on_x_panel_c_btn_go_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_hostile.x.panel_c.btn_go.lv_obj, UI_CHILD_BUTTON, &s_hostile.x.panel_c.btn_go.style);

    /* btn_go (button) */
    s_hostile.y.panel_c.btn_go.lv_obj = lv_button_create(s_hostile.y.panel_c.lv_obj);
    lv_obj_set_pos(s_hostile.y.panel_c.btn_go.lv_obj, 2, 2);
    lv_obj_set_size(s_hostile.y.panel_c.btn_go.lv_obj, 40, 18);
    {
        lv_obj_t *lbl = lv_label_create(s_hostile.y.panel_c.btn_go.lv_obj);
        lv_label_set_text(lbl, s_hostile.y.panel_c.btn_go.label_text);
        lv_obj_center(lbl);
        /* the Figma Text layer's colour/font/alignment; not the button's box */
        ui_apply_text_style(lbl, &s_hostile.y.panel_c.btn_go.style);
    }
    lv_obj_add_event_cb(s_hostile.y.panel_c.btn_go.lv_obj, ui_hostile_on_y_panel_c_btn_go_clicked,
                        LV_EVENT_CLICKED, NULL);
    ui_apply_style(s_hostile.y.panel_c.btn_go.lv_obj, UI_CHILD_BUTTON, &s_hostile.y.panel_c.btn_go.style);

    /* c (panel) */
    s_hostile.a.b.c.lv_obj = lv_obj_create(s_hostile.a.b.lv_obj);
    lv_obj_remove_style_all(s_hostile.a.b.c.lv_obj);
    lv_obj_set_pos(s_hostile.a.b.c.lv_obj, 2, 2);
    lv_obj_set_size(s_hostile.a.b.c.lv_obj, 180, 60);
    lv_obj_clear_flag(s_hostile.a.b.c.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_hostile.a.b.c.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_hostile.a.b.c.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_hostile.a.b.c.lv_obj, UI_CHILD_PANEL, &s_hostile.a.b.c.style);

    /* deep (label) */
    s_hostile.a.b.c.deep.lv_obj = lv_label_create(s_hostile.a.b.c.lv_obj);
    lv_obj_set_pos(s_hostile.a.b.c.deep.lv_obj, 4, 2);
    lv_obj_set_width(s_hostile.a.b.c.deep.lv_obj, 60);
    lv_label_set_long_mode(s_hostile.a.b.c.deep.lv_obj, LV_LABEL_LONG_CLIP);
    lv_label_set_text(s_hostile.a.b.c.deep.lv_obj, s_hostile.a.b.c.deep.text);
    ui_apply_style(s_hostile.a.b.c.deep.lv_obj, UI_CHILD_LABEL, &s_hostile.a.b.c.deep.style);
}
