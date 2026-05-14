#include "ui_style.h"

// ── Font mapping ──────────────────────────────────────────────────────────────

const lv_font_t *ui_get_font(uint16_t size)
{
    switch (size)
    {
#if LV_FONT_MONTSERRAT_10
        case 10: return &lv_font_montserrat_10;
#endif
#if LV_FONT_MONTSERRAT_12
        case 12: return &lv_font_montserrat_12;
#endif
#if LV_FONT_MONTSERRAT_14
        case 14: return &lv_font_montserrat_14;
#endif
#if LV_FONT_MONTSERRAT_16
        case 16: return &lv_font_montserrat_16;
#endif
#if LV_FONT_MONTSERRAT_18
        case 18: return &lv_font_montserrat_18;
#endif
#if LV_FONT_MONTSERRAT_20
        case 20: return &lv_font_montserrat_20;
#endif
#if LV_FONT_MONTSERRAT_22
        case 22: return &lv_font_montserrat_22;
#endif
#if LV_FONT_MONTSERRAT_24
        case 24: return &lv_font_montserrat_24;
#endif
        default: return LV_FONT_DEFAULT;
    }
}

// ── Style application ─────────────────────────────────────────────────────────

void ui_apply_style(lv_obj_t *obj, ui_child_type_t type, const ui_style_t *s)
{
    if (!obj || !s)
        return;

    // ── Box styles (all widget types) ─────────────────────────────────────
    if (s->box.has_bg)
        lv_obj_set_style_bg_color(obj, lv_color_hex(s->box.bg), LV_PART_MAIN);

    if (s->box.has_bg_opa)
        lv_obj_set_style_bg_opa(obj, s->box.bg_opa, LV_PART_MAIN);

    if (s->box.has_border_color)
        lv_obj_set_style_border_color(obj, lv_color_hex(s->box.border_color), LV_PART_MAIN);

    if (s->box.has_border_width)
        lv_obj_set_style_border_width(obj, s->box.border_width, LV_PART_MAIN);

    if (s->box.has_radius)
        lv_obj_set_style_radius(obj, s->box.radius, LV_PART_MAIN);

    // ── Text styles ───────────────────────────────────────────────────────
    if (type == UI_CHILD_LABEL || type == UI_CHILD_BUTTON)
    {
        if (s->text.has_color)
            lv_obj_set_style_text_color(obj, lv_color_hex(s->text.color), LV_PART_MAIN);

        if (s->text.has_size)
        {
            const lv_font_t *font = ui_get_font(s->text.size);
            if (font)
                lv_obj_set_style_text_font(obj, font, LV_PART_MAIN);
        }

        if (s->text.has_align)
            lv_obj_set_style_text_align(obj, s->text.align, LV_PART_MAIN);
    }

    // ── Bar indicator: fill color → animated portion ──────────────────────
    if (type == UI_CHILD_BAR && s->box.has_bg)
        lv_obj_set_style_bg_color(obj, lv_color_hex(s->box.bg), LV_PART_INDICATOR);

    // ── Slider indicator: same pattern as bar ─────────────────────────────
    if (type == UI_CHILD_SLIDER && s->box.has_bg)
        lv_obj_set_style_bg_color(obj, lv_color_hex(s->box.bg), LV_PART_INDICATOR);

    // ── Global opacity ────────────────────────────────────────────────────
    if (s->effects.has_opacity)
        lv_obj_set_style_opa(obj, s->effects.opacity, LV_PART_MAIN);
}
