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
        char        text[UI_TEXT_SIZE(2)];
    } label_2nd_row;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } label_default;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } label_class;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } label_1;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } label_2;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } label_style;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } label_lv_obj;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } title;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } title_3;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } title_2;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        char        text[UI_TEXT_SIZE(2)];
    } title_4;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        label_text[UI_TEXT_SIZE(3)];
        } btn_ok;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_TEXT_SIZE(2)];
        } title;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_TEXT_SIZE(2)];
        } label_text;
    } panel_a;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        label_text[UI_TEXT_SIZE(3)];
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
                char        label_text[UI_TEXT_SIZE(3)];
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
                char        label_text[UI_TEXT_SIZE(3)];
            } btn_go;
        } panel_c;
    } y;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_TEXT_SIZE(2)];
        } e_label;
    } panel_d;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            char        text[UI_TEXT_SIZE(2)];
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
                    char        text[UI_TEXT_SIZE(2)];
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
        .src = &image_ui_style,
    },
    .icon_wifi = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
        .src = &icon_wifi,
    },
    .icon_wifi_2 = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
        .src = &icon_wifi,
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
    ui_copy_text(s_hostile.label_2nd_row.text, sizeof s_hostile.label_2nd_row.text, text);
    if (!s_hostile.label_2nd_row.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_label_2nd_row_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.label_2nd_row.lv_obj, s_hostile.label_2nd_row.text);
}

void ui_hostile_label_default_set_text(const char *text)
{
    ui_copy_text(s_hostile.label_default.text, sizeof s_hostile.label_default.text, text);
    if (!s_hostile.label_default.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_label_default_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.label_default.lv_obj, s_hostile.label_default.text);
}

void ui_hostile_label_class_set_text(const char *text)
{
    ui_copy_text(s_hostile.label_class.text, sizeof s_hostile.label_class.text, text);
    if (!s_hostile.label_class.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_label_class_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.label_class.lv_obj, s_hostile.label_class.text);
}

void ui_hostile_label_1_set_text(const char *text)
{
    ui_copy_text(s_hostile.label_1.text, sizeof s_hostile.label_1.text, text);
    if (!s_hostile.label_1.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_label_1_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.label_1.lv_obj, s_hostile.label_1.text);
}

void ui_hostile_label_2_set_text(const char *text)
{
    ui_copy_text(s_hostile.label_2.text, sizeof s_hostile.label_2.text, text);
    if (!s_hostile.label_2.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_label_2_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.label_2.lv_obj, s_hostile.label_2.text);
}

void ui_hostile_label_style_set_text(const char *text)
{
    ui_copy_text(s_hostile.label_style.text, sizeof s_hostile.label_style.text, text);
    if (!s_hostile.label_style.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_label_style_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.label_style.lv_obj, s_hostile.label_style.text);
}

void ui_hostile_label_lv_obj_set_text(const char *text)
{
    ui_copy_text(s_hostile.label_lv_obj.text, sizeof s_hostile.label_lv_obj.text, text);
    if (!s_hostile.label_lv_obj.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_label_lv_obj_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.label_lv_obj.lv_obj, s_hostile.label_lv_obj.text);
}

void ui_hostile_title_set_text(const char *text)
{
    ui_copy_text(s_hostile.title.text, sizeof s_hostile.title.text, text);
    if (!s_hostile.title.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_title_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.title.lv_obj, s_hostile.title.text);
}

void ui_hostile_title_3_set_text(const char *text)
{
    ui_copy_text(s_hostile.title_3.text, sizeof s_hostile.title_3.text, text);
    if (!s_hostile.title_3.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_title_3_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.title_3.lv_obj, s_hostile.title_3.text);
}

void ui_hostile_title_2_set_text(const char *text)
{
    ui_copy_text(s_hostile.title_2.text, sizeof s_hostile.title_2.text, text);
    if (!s_hostile.title_2.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_title_2_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.title_2.lv_obj, s_hostile.title_2.text);
}

void ui_hostile_title_4_set_text(const char *text)
{
    ui_copy_text(s_hostile.title_4.text, sizeof s_hostile.title_4.text, text);
    if (!s_hostile.title_4.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_title_4_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.title_4.lv_obj, s_hostile.title_4.text);
}

void ui_hostile_ui_style_display(void)
{
    s_hostile.ui_style.src = &image_ui_style;
    if (!s_hostile.ui_style.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_ui_style_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_hostile.ui_style.lv_obj, s_hostile.ui_style.src);
}

void ui_hostile_ui_style_hide(void)
{
    s_hostile.ui_style.src = NULL;   /* hidden, also after deinit and init */
    if (!s_hostile.ui_style.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_ui_style_hide: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_hostile.ui_style.lv_obj, NULL);
}

void ui_hostile_icon_wifi_display(void)
{
    s_hostile.icon_wifi.src = &icon_wifi;
    if (!s_hostile.icon_wifi.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_icon_wifi_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_hostile.icon_wifi.lv_obj, s_hostile.icon_wifi.src);
}

void ui_hostile_icon_wifi_hide(void)
{
    s_hostile.icon_wifi.src = NULL;   /* hidden, also after deinit and init */
    if (!s_hostile.icon_wifi.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_icon_wifi_hide: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_hostile.icon_wifi.lv_obj, NULL);
}

void ui_hostile_icon_wifi_2_display(void)
{
    s_hostile.icon_wifi_2.src = &icon_wifi;
    if (!s_hostile.icon_wifi_2.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_icon_wifi_2_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_hostile.icon_wifi_2.lv_obj, s_hostile.icon_wifi_2.src);
}

void ui_hostile_icon_wifi_2_hide(void)
{
    s_hostile.icon_wifi_2.src = NULL;   /* hidden, also after deinit and init */
    if (!s_hostile.icon_wifi_2.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_icon_wifi_2_hide: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_hostile.icon_wifi_2.lv_obj, NULL);
}

void ui_hostile_panel_a_btn_ok_set_label(const char *text)
{
    ui_copy_text(s_hostile.panel_a.btn_ok.label_text, sizeof s_hostile.panel_a.btn_ok.label_text, text);
    if (!s_hostile.panel_a.btn_ok.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_panel_a_btn_ok_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_hostile.panel_a.btn_ok.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_hostile.panel_a.btn_ok.label_text);
}

void ui_hostile_panel_a_title_set_text(const char *text)
{
    ui_copy_text(s_hostile.panel_a.title.text, sizeof s_hostile.panel_a.title.text, text);
    if (!s_hostile.panel_a.title.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_panel_a_title_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.panel_a.title.lv_obj, s_hostile.panel_a.title.text);
}

void ui_hostile_panel_a_label_text_set_text(const char *text)
{
    ui_copy_text(s_hostile.panel_a.label_text.text, sizeof s_hostile.panel_a.label_text.text, text);
    if (!s_hostile.panel_a.label_text.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_panel_a_label_text_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.panel_a.label_text.lv_obj, s_hostile.panel_a.label_text.text);
}

void ui_hostile_panel_b_btn_ok_set_label(const char *text)
{
    ui_copy_text(s_hostile.panel_b.btn_ok.label_text, sizeof s_hostile.panel_b.btn_ok.label_text, text);
    if (!s_hostile.panel_b.btn_ok.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_panel_b_btn_ok_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_hostile.panel_b.btn_ok.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_hostile.panel_b.btn_ok.label_text);
}

void ui_hostile_panel_d_e_label_set_text(const char *text)
{
    ui_copy_text(s_hostile.panel_d.e_label.text, sizeof s_hostile.panel_d.e_label.text, text);
    if (!s_hostile.panel_d.e_label.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_panel_d_e_label_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.panel_d.e_label.lv_obj, s_hostile.panel_d.e_label.text);
}

void ui_hostile_panel_d_e_label_set_text_2(const char *text)
{
    ui_copy_text(s_hostile.panel_d_e.label.text, sizeof s_hostile.panel_d_e.label.text, text);
    if (!s_hostile.panel_d_e.label.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_panel_d_e_label_set_text_2: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.panel_d_e.label.lv_obj, s_hostile.panel_d_e.label.text);
}

void ui_hostile_x_panel_c_btn_go_set_label(const char *text)
{
    ui_copy_text(s_hostile.x.panel_c.btn_go.label_text, sizeof s_hostile.x.panel_c.btn_go.label_text, text);
    if (!s_hostile.x.panel_c.btn_go.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_x_panel_c_btn_go_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_hostile.x.panel_c.btn_go.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_hostile.x.panel_c.btn_go.label_text);
}

void ui_hostile_y_panel_c_btn_go_set_label(const char *text)
{
    ui_copy_text(s_hostile.y.panel_c.btn_go.label_text, sizeof s_hostile.y.panel_c.btn_go.label_text, text);
    if (!s_hostile.y.panel_c.btn_go.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_y_panel_c_btn_go_set_label: screen not built; value stored");
        return;
    }
    lv_obj_t *lbl = lv_obj_get_child(s_hostile.y.panel_c.btn_go.lv_obj, 0);
    if (lbl) lv_label_set_text(lbl, s_hostile.y.panel_c.btn_go.label_text);
}

void ui_hostile_a_b_c_deep_set_text(const char *text)
{
    ui_copy_text(s_hostile.a.b.c.deep.text, sizeof s_hostile.a.b.c.deep.text, text);
    if (!s_hostile.a.b.c.deep.lv_obj) {
        UI_LIFECYCLE_WARN("ui_hostile_a_b_c_deep_set_text: screen not built; value stored");
        return;
    }
    lv_label_set_text(s_hostile.a.b.c.deep.lv_obj, s_hostile.a.b.c.deep.text);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_hostile_load(void)
{
    if (!s_hostile.lv_screen) {
        UI_LIFECYCLE_WARN("ui_hostile_load: screen not built");
        return;
    }
    lv_scr_load(s_hostile.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_hostile_init(void)
{
    if (s_hostile.lv_screen)
        return;
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

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_hostile_deinit(void)
{
    if (!s_hostile.lv_screen)
        return;
    if (lv_screen_active() == s_hostile.lv_screen) {
        UI_LIFECYCLE_WARN("ui_hostile_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_hostile.lv_screen);   /* frees every child, callback and animation */
    s_hostile.lv_screen = NULL;
    s_hostile.label_2nd_row.lv_obj = NULL;
    s_hostile.label_default.lv_obj = NULL;
    s_hostile.label_class.lv_obj = NULL;
    s_hostile.label_1.lv_obj = NULL;
    s_hostile.label_2.lv_obj = NULL;
    s_hostile.label_style.lv_obj = NULL;
    s_hostile.label_lv_obj.lv_obj = NULL;
    s_hostile.title.lv_obj = NULL;
    s_hostile.title_3.lv_obj = NULL;
    s_hostile.title_2.lv_obj = NULL;
    s_hostile.title_4.lv_obj = NULL;
    s_hostile.panel_a.lv_obj = NULL;
    s_hostile.panel_b.lv_obj = NULL;
    s_hostile.x.lv_obj = NULL;
    s_hostile.y.lv_obj = NULL;
    s_hostile.panel_d.lv_obj = NULL;
    s_hostile.panel_d_e.lv_obj = NULL;
    s_hostile.a.lv_obj = NULL;
    s_hostile.ui_style.lv_obj = NULL;
    s_hostile.icon_wifi.lv_obj = NULL;
    s_hostile.icon_wifi_2.lv_obj = NULL;
    s_hostile.panel_a.btn_ok.lv_obj = NULL;
    s_hostile.panel_a.title.lv_obj = NULL;
    s_hostile.panel_a.label_text.lv_obj = NULL;
    s_hostile.panel_b.btn_ok.lv_obj = NULL;
    s_hostile.x.panel_c.lv_obj = NULL;
    s_hostile.y.panel_c.lv_obj = NULL;
    s_hostile.panel_d.e_label.lv_obj = NULL;
    s_hostile.panel_d_e.label.lv_obj = NULL;
    s_hostile.a.b.lv_obj = NULL;
    s_hostile.x.panel_c.btn_go.lv_obj = NULL;
    s_hostile.y.panel_c.btn_go.lv_obj = NULL;
    s_hostile.a.b.c.lv_obj = NULL;
    s_hostile.a.b.c.deep.lv_obj = NULL;
}
