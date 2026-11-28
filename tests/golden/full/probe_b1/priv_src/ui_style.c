#include "ui_style.h"
#include <string.h>

// ── Font mapping ──────────────────────────────────────────────────────────────

// Every Montserrat size LVGL ships (8 to 48, even sizes). A size that isn't
// enabled in lv_conf.h, or that LVGL doesn't have, uses LV_FONT_DEFAULT.
const lv_font_t *ui_get_font(uint16_t size)
{
    switch (size)
    {
#if LV_FONT_MONTSERRAT_8
        case 8: return &lv_font_montserrat_8;
#endif
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
#if LV_FONT_MONTSERRAT_26
        case 26: return &lv_font_montserrat_26;
#endif
#if LV_FONT_MONTSERRAT_28
        case 28: return &lv_font_montserrat_28;
#endif
#if LV_FONT_MONTSERRAT_30
        case 30: return &lv_font_montserrat_30;
#endif
#if LV_FONT_MONTSERRAT_32
        case 32: return &lv_font_montserrat_32;
#endif
#if LV_FONT_MONTSERRAT_34
        case 34: return &lv_font_montserrat_34;
#endif
#if LV_FONT_MONTSERRAT_36
        case 36: return &lv_font_montserrat_36;
#endif
#if LV_FONT_MONTSERRAT_38
        case 38: return &lv_font_montserrat_38;
#endif
#if LV_FONT_MONTSERRAT_40
        case 40: return &lv_font_montserrat_40;
#endif
#if LV_FONT_MONTSERRAT_42
        case 42: return &lv_font_montserrat_42;
#endif
#if LV_FONT_MONTSERRAT_44
        case 44: return &lv_font_montserrat_44;
#endif
#if LV_FONT_MONTSERRAT_46
        case 46: return &lv_font_montserrat_46;
#endif
#if LV_FONT_MONTSERRAT_48
        case 48: return &lv_font_montserrat_48;
#endif
        default: return LV_FONT_DEFAULT;
    }
}

// ── Style application ─────────────────────────────────────────────────────────

void ui_apply_text_style(lv_obj_t *obj, const ui_style_t *s)
{
    if (!obj || !s)
        return;

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

    // ── Text styles: labels only. A button's text gets its own Figma Text
    // layer's style through ui_apply_text_style(), never the button's box.
    if (type == UI_CHILD_LABEL)
        ui_apply_text_style(obj, s);

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


// ── Text buffers (design doc 11, rule 3) ─────────────────────────────────────

void ui_copy_text(char *dst, size_t size, const char *text)
{
    if (!dst || size == 0)
        return;
    size_t n = 0;
    if (text)
    {
        while (text[n] && n < size - 1)
            n++;
        if (text[n])
        {
            // Cut: step back over continuation bytes to a character start,
            // then drop that character if it doesn't fit whole.
            size_t start = n;
            while (start > 0 && ((unsigned char)text[start] & 0xC0) == 0x80)
                start--;
            n = start;
        }
        memcpy(dst, text, n);
    }
    dst[n] = 0;
}
