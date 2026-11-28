#include "ui_f_images.h"
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
        const lv_image_dsc_t *src;
    } icon_star;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } icon_star_2;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        const lv_image_dsc_t *src;
    } icon_heart;
    struct {
        lv_obj_t   *lv_obj;
        ui_style_t  style;
        struct {
            lv_obj_t   *lv_obj;
            ui_style_t  style;
            const lv_image_dsc_t *src;
        } icon_heart;
    } panel_icons;
} s_f_images = {
    .icon_star = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .icon_star_2 = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .icon_heart = {
        .style = { .box = {0}, .text = {0}, .effects = {0} },
    },
    .panel_icons = {
        .style = {
            .box = { .has_bg = true, .bg = 0xF5F5F5 }
        },
        .icon_heart = {
            .style = { .box = {0}, .text = {0}, .effects = {0} },
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
void ui_f_images_icon_star_display(void)
{
    s_f_images.icon_star.src = &icon_star;
    if (!s_f_images.icon_star.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_images_icon_star_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_f_images.icon_star.lv_obj, s_f_images.icon_star.src);
}

void ui_f_images_icon_star_2_display(void)
{
    s_f_images.icon_star_2.src = &icon_star;
    if (!s_f_images.icon_star_2.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_images_icon_star_2_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_f_images.icon_star_2.lv_obj, s_f_images.icon_star_2.src);
}

void ui_f_images_icon_heart_display(void)
{
    s_f_images.icon_heart.src = &icon_heart;
    if (!s_f_images.icon_heart.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_images_icon_heart_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_f_images.icon_heart.lv_obj, s_f_images.icon_heart.src);
}

void ui_f_images_panel_icons_icon_heart_display(void)
{
    s_f_images.panel_icons.icon_heart.src = &icon_heart;
    if (!s_f_images.panel_icons.icon_heart.lv_obj) {
        UI_LIFECYCLE_WARN("ui_f_images_panel_icons_icon_heart_display: screen not built; value stored");
        return;
    }
    lv_image_set_src(s_f_images.panel_icons.icon_heart.lv_obj, s_f_images.panel_icons.icon_heart.src);
}

// ------------------------------
// SCREEN LOAD
// ------------------------------
void ui_f_images_load(void)
{
    if (!s_f_images.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_images_load: screen not built");
        return;
    }
    lv_scr_load(s_f_images.lv_screen);
}

// ------------------------------
// SCREEN INIT (builds from the struct's values; no-op if built)
// ------------------------------
void ui_f_images_init(void)
{
    if (s_f_images.lv_screen)
        return;
    s_f_images.lv_screen = lv_obj_create(NULL);

    /* icon_star (image) */
    s_f_images.icon_star.lv_obj = lv_image_create(s_f_images.lv_screen);
    lv_obj_set_pos(s_f_images.icon_star.lv_obj, 20, 20);
    lv_obj_set_size(s_f_images.icon_star.lv_obj, 24, 24);
    if (s_f_images.icon_star.src)
        lv_image_set_src(s_f_images.icon_star.lv_obj, s_f_images.icon_star.src);
    ui_apply_style(s_f_images.icon_star.lv_obj, UI_CHILD_IMAGE, &s_f_images.icon_star.style);

    /* icon_star_2 (image) */
    s_f_images.icon_star_2.lv_obj = lv_image_create(s_f_images.lv_screen);
    lv_obj_set_pos(s_f_images.icon_star_2.lv_obj, 60, 20);
    lv_obj_set_size(s_f_images.icon_star_2.lv_obj, 24, 24);
    if (s_f_images.icon_star_2.src)
        lv_image_set_src(s_f_images.icon_star_2.lv_obj, s_f_images.icon_star_2.src);
    ui_apply_style(s_f_images.icon_star_2.lv_obj, UI_CHILD_IMAGE, &s_f_images.icon_star_2.style);

    /* icon_heart (image) */
    s_f_images.icon_heart.lv_obj = lv_image_create(s_f_images.lv_screen);
    lv_obj_set_pos(s_f_images.icon_heart.lv_obj, 100, 20);
    lv_obj_set_size(s_f_images.icon_heart.lv_obj, 24, 24);
    if (s_f_images.icon_heart.src)
        lv_image_set_src(s_f_images.icon_heart.lv_obj, s_f_images.icon_heart.src);
    ui_apply_style(s_f_images.icon_heart.lv_obj, UI_CHILD_IMAGE, &s_f_images.icon_heart.style);

    /* panel_icons (panel) */
    s_f_images.panel_icons.lv_obj = lv_obj_create(s_f_images.lv_screen);
    lv_obj_remove_style_all(s_f_images.panel_icons.lv_obj);
    lv_obj_set_pos(s_f_images.panel_icons.lv_obj, 20, 80);
    lv_obj_set_size(s_f_images.panel_icons.lv_obj, 120, 50);
    lv_obj_clear_flag(s_f_images.panel_icons.lv_obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_scrollbar_mode(s_f_images.panel_icons.lv_obj, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(s_f_images.panel_icons.lv_obj, LV_OPA_COVER, LV_PART_MAIN);
    ui_apply_style(s_f_images.panel_icons.lv_obj, UI_CHILD_PANEL, &s_f_images.panel_icons.style);

    /* icon_heart (image) */
    s_f_images.panel_icons.icon_heart.lv_obj = lv_image_create(s_f_images.panel_icons.lv_obj);
    lv_obj_set_pos(s_f_images.panel_icons.icon_heart.lv_obj, 10, 13);
    lv_obj_set_size(s_f_images.panel_icons.icon_heart.lv_obj, 24, 24);
    if (s_f_images.panel_icons.icon_heart.src)
        lv_image_set_src(s_f_images.panel_icons.icon_heart.lv_obj, s_f_images.panel_icons.icon_heart.src);
    ui_apply_style(s_f_images.panel_icons.icon_heart.lv_obj, UI_CHILD_IMAGE, &s_f_images.panel_icons.icon_heart.style);
}

// ------------------------------
// SCREEN DEINIT (refused for the active screen; values kept)
// ------------------------------
void ui_f_images_deinit(void)
{
    if (!s_f_images.lv_screen)
        return;
    if (lv_screen_active() == s_f_images.lv_screen) {
        UI_LIFECYCLE_WARN("ui_f_images_deinit: refused, it is the active screen; load another first");
        return;
    }
    lv_obj_delete(s_f_images.lv_screen);   /* frees every child, callback and animation */
    s_f_images.lv_screen = NULL;
    s_f_images.icon_star.lv_obj = NULL;
    s_f_images.icon_star_2.lv_obj = NULL;
    s_f_images.icon_heart.lv_obj = NULL;
    s_f_images.panel_icons.lv_obj = NULL;
    s_f_images.panel_icons.icon_heart.lv_obj = NULL;
}
