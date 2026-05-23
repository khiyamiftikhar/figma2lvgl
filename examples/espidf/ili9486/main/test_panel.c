
/**
 * color_intercept_test.c
 *
 * Drop this into your main/ folder and call color_intercept_test_run()
 * from app_main() AFTER ili9486_display_init() has returned.
 *
 * What it does:
 *   1. Computes every encoding variant of your test colour (RGB565, BGR565,
 *      byte-swapped versions) so you know what bytes to expect on the wire.
 *   2. Hooks into the LVGL flush path by replacing the display's flush
 *      callback with a thin wrapper that logs the first non-trivial flush.
 *   3. Creates a full-screen LVGL object painted with your test colour.
 *   4. Lets LVGL render, then prints a decode table so you can match what
 *      actually arrived against the expected values and immediately know
 *      which combination (swap_bytes on/off, BGR bit on/off) is correct.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "esp_lcd_panel_ops.h"
#include "test_panel.h"

/* ── bring in the panel handle from your init module ─────────────────────── */
extern esp_lcd_panel_handle_t ili9486_display_get_panel(void);

static const char *TAG = "COLOR_INTERCEPT";

/* ── test colour ─────────────────────────────────────────────────────────── */
#define TEST_HEX  0xFF0000   // pure RED → expect 0xF8 0x00 (or 0x00 0xF8 swapped)   /* change to any 24-bit RGB colour */

/* ── intercept state ─────────────────────────────────────────────────────── */
static lv_display_flush_cb_t  s_original_flush_cb = NULL;
static esp_lcd_panel_handle_t s_panel              = NULL;
static volatile bool          s_logged             = false;

/* ══════════════════════════════════════════════════════════════════════════
 * SECTION 1 — colour math helpers
 * ══════════════════════════════════════════════════════════════════════════ */

/** Pack a 24-bit RGB888 value into a 16-bit RGB565 word (big-endian natural). */
static uint16_t rgb888_to_rgb565(uint32_t hex)
{
    uint8_t r = (hex >> 16) & 0xFF;
    uint8_t g = (hex >>  8) & 0xFF;
    uint8_t b = (hex >>  0) & 0xFF;
    return (uint16_t)(((r & 0xF8u) << 8) | ((g & 0xFCu) << 3) | (b >> 3));
}

/** Pack the same value as BGR565 (blue in the top 5 bits). */
static uint16_t rgb888_to_bgr565(uint32_t hex)
{
    uint8_t r = (hex >> 16) & 0xFF;
    uint8_t g = (hex >>  8) & 0xFF;
    uint8_t b = (hex >>  0) & 0xFF;
    return (uint16_t)(((b & 0xF8u) << 8) | ((g & 0xFCu) << 3) | (r >> 3));
}

/** Byte-swap a 16-bit word. */
static inline uint16_t swap16(uint16_t v)
{
    return (uint16_t)((v >> 8) | (v << 8));
}

/**
 * Print the full decision table for a given 24-bit source colour.
 *
 * Columns
 *   Config            : what LVGL / driver settings produce this encoding
 *   uint16            : the 16-bit pixel word
 *   byte[0] byte[1]   : the two bytes on the SPI bus (index 0 = first sent)
 *   Decoded R G B     : what colour the ILI9486 will reconstruct (BGR chip)
 *
 * ILI9486 is a BGR panel: it interprets the top 5 bits of the received
 * 16-bit word as BLUE, the middle 6 as GREEN, the bottom 5 as RED.
 */
static void print_expected_table(uint32_t hex)
{
    uint8_t r8 = (hex >> 16) & 0xFF;
    uint8_t g8 = (hex >>  8) & 0xFF;
    uint8_t b8 = (hex >>  0) & 0xFF;

    uint16_t rgb  = rgb888_to_rgb565(hex);
    uint16_t bgr  = rgb888_to_bgr565(hex);
    uint16_t rgb_s = swap16(rgb);
    uint16_t bgr_s = swap16(bgr);

    /* Decode what the ILI9486 would actually display from each word.
     * ILI9486 maps: bits[15:11]=B, bits[10:5]=G, bits[4:0]=R              */
    #define ILI_B(w)  (((w) >> 11) & 0x1Fu)
    #define ILI_G(w)  (((w) >>  5) & 0x3Fu)
    #define ILI_R(w)  ( (w)        & 0x1Fu)

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔═══════════════════════════════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║  Source colour  #%06lX   R=%3d G=%3d B=%3d                   ║",
             (unsigned long)hex, r8, g8, b8);
    ESP_LOGI(TAG, "╠═══════════════════════════════════════════════════════════════════╣");
    ESP_LOGI(TAG, "║  Config                  │ uint16  │byte[0] byte[1]│Display B G R║");
    ESP_LOGI(TAG, "╠═══════════════════════════════════════════════════════════════════╣");

    /* row helper: config label, word, what ILI9486 sees */
    #define ROW(label, word) \
        ESP_LOGI(TAG, "║  %-24s│ 0x%04X  │  0x%02X   0x%02X  │ %2d  %2d  %2d ║", \
                 (label), (word), \
                 (word) & 0xFF, ((word) >> 8) & 0xFF, \
                 ILI_B(word), ILI_G(word), ILI_R(word))

    ROW("RGB565, swap=false",  rgb);
    ROW("RGB565, swap=true",   rgb_s);
    ROW("BGR565, swap=false",  bgr);
    ROW("BGR565, swap=true",   bgr_s);

    #undef ROW
    #undef ILI_B
    #undef ILI_G
    #undef ILI_R

    ESP_LOGI(TAG, "╠═══════════════════════════════════════════════════════════════════╣");
    ESP_LOGI(TAG, "║  GOAL: Display B G R should equal source B=%2d G=%2d R=%2d          ║",
             b8 >> 3, g8 >> 2, r8 >> 3);
    ESP_LOGI(TAG, "╚═══════════════════════════════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");
}

/* ══════════════════════════════════════════════════════════════════════════
 * SECTION 2 — flush callback intercept
 * ══════════════════════════════════════════════════════════════════════════ */

/**
 * Decode and log a 16-bit pixel word from the flush buffer.
 * Tries both LE and BE interpretations and both RGB / BGR mappings.
 */
static void log_pixel(const char *label, uint8_t b0, uint8_t b1)
{
    /* Reconstruct as LE (LVGL default) and BE */
    uint16_t le = (uint16_t)((b1 << 8) | b0);   /* b0=low byte */
    uint16_t be = (uint16_t)((b0 << 8) | b1);   /* b0=high byte */

    /* RGB interpretation: bits[15:11]=R  bits[10:5]=G  bits[4:0]=B */
    #define RGB_R(w) (((w)>>11)&0x1F)
    #define RGB_G(w) (((w)>> 5)&0x3F)
    #define RGB_B(w) ( (w)     &0x1F)
    /* BGR interpretation: bits[15:11]=B  bits[10:5]=G  bits[4:0]=R */
    #define BGR_B(w) (((w)>>11)&0x1F)
    #define BGR_G(w) (((w)>> 5)&0x3F)
    #define BGR_R(w) ( (w)     &0x1F)

    ESP_LOGI(TAG, "  [%s]  raw=0x%02X 0x%02X", label, b0, b1);
    ESP_LOGI(TAG, "         LE 0x%04X → RGB(%d,%d,%d)  BGR(%d,%d,%d)",
             le,
             RGB_R(le)<<3, RGB_G(le)<<2, RGB_B(le)<<3,
             BGR_B(le)<<3, BGR_G(le)<<2, BGR_R(le)<<3);
    ESP_LOGI(TAG, "         BE 0x%04X → RGB(%d,%d,%d)  BGR(%d,%d,%d)",
             be,
             RGB_R(be)<<3, RGB_G(be)<<2, RGB_B(be)<<3,
             BGR_B(be)<<3, BGR_G(be)<<2, BGR_R(be)<<3);

    #undef RGB_R
    #undef RGB_G
    #undef RGB_B
    #undef BGR_B
    #undef BGR_G
    #undef BGR_R
}

/**
 * Our replacement flush callback.
 * On the first call it logs pixel data, then forwards to the real panel and
 * calls lv_display_flush_ready().  Subsequent calls are transparent.
 */
static void intercept_flush_cb(lv_display_t *disp,
                                const lv_area_t *area,
                                uint8_t *px_map)
{
    if (!s_logged) {
        s_logged = true;

        int w = area->x2 - area->x1 + 1;
        int h = area->y2 - area->y1 + 1;

        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "┌─── FLUSH INTERCEPTED ─────────────────────────────┐");
        ESP_LOGI(TAG, "│  Area: (%d,%d)→(%d,%d)  size=%d×%d px",
                 area->x1, area->y1, area->x2, area->y2, w, h);
        ESP_LOGI(TAG, "│  Buffer pointer: %p", (void *)px_map);

        /* Sample pixel at (0,0) of the flush area */
        log_pixel("pixel[0,0]", px_map[0], px_map[1]);

        /* Sample centre pixel */
        if (w > 2 && h > 2) {
            int cx = w / 2, cy = h / 2;
            int idx = (cy * w + cx) * 2;
            log_pixel("pixel[cx,cy]", px_map[idx], px_map[idx + 1]);
        }

        /* Raw hex dump of first 16 bytes */
        ESP_LOGI(TAG, "│  First 16 bytes:");
        ESP_LOG_BUFFER_HEX_LEVEL(TAG, px_map,
                                 (w * h * 2 > 16) ? 16 : w * h * 2,
                                 ESP_LOG_INFO);
        ESP_LOGI(TAG, "└───────────────────────────────────────────────────┘");
        ESP_LOGI(TAG, "");
        ESP_LOGI(TAG, "Now compare the bytes above with the expected table:");
        ESP_LOGI(TAG, "  The row whose byte[0]/byte[1] match your raw bytes");
        ESP_LOGI(TAG, "  tells you exactly which config setting is active.");
    }

        // Temporarily force-swap the first pixel manually and log it
    uint8_t b0 = px_map[0], b1 = px_map[1];
    ESP_LOGI(TAG, "Pre-draw bytes: 0x%02X 0x%02X", b0, b1);

    // Manually swap and draw just pixel 0 to see if it changes colour on screen
    uint8_t swapped[2] = { b1, b0 };
    esp_lcd_panel_draw_bitmap(s_panel, 0, 0, 1, 1, swapped);
    vTaskDelay(pdMS_TO_TICKS(2000));  // look at top-left pixel colour

    /* Forward to the real panel */
    esp_lcd_panel_draw_bitmap(s_panel,
                              area->x1, area->y1,
                              area->x2 + 1, area->y2 + 1,
                              px_map);
    lv_display_flush_ready(disp);
}

/* ══════════════════════════════════════════════════════════════════════════
 * SECTION 3 — public entry point
 * ══════════════════════════════════════════════════════════════════════════ */

/**
 * color_intercept_test_run()
 *
 * Call once after ili9486_display_init() has completed and returned a valid
 * lv_display_t handle.
 *
 * @param disp   The lv_display_t* returned by ili9486_display_init().
 */
void color_intercept_test_run(lv_display_t *disp)
{
    ESP_LOGI(TAG, "=== Color intercept test starting ===");

    /* ── 1. Print expected table before anything is drawn ── */
    print_expected_table(TEST_HEX);

    /* ── 2. Grab the panel handle ── */
    s_panel = ili9486_display_get_panel();
    if (!s_panel) {
        ESP_LOGE(TAG, "Panel handle is NULL — init must complete first");
        return;
    }

    /* ── 3. Replace the LVGL flush callback with our intercept ── */
    /*
     * lvgl_port sets its own flush callback on the display.  We replace it
     * here so that the very next flush passes through our logger first.
     * We call esp_lcd_panel_draw_bitmap() directly (bypassing lvgl_port's
     * wrapper) and then lv_display_flush_ready() ourselves, so the flow is
     * identical to what lvgl_port does internally.
     */
    if (lvgl_port_lock(0)) {
        lv_display_set_flush_cb(disp, intercept_flush_cb);
        lvgl_port_unlock();
    }

    /* ── 4. Create a full-screen object with the test colour ── */
    if (lvgl_port_lock(0)) {
    lv_obj_t *scr = lv_display_get_screen_active(disp);

    // Paint the screen itself — no child object needed
    lv_obj_set_style_bg_color(scr, lv_color_hex(TEST_HEX), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);

    lv_color_t c = lv_color_hex(TEST_HEX);
    ESP_LOGI(TAG, "lv_color_hex(#%06lX) → 0x%04X",
             (unsigned long)TEST_HEX, lv_color_to_u32(c) & 0xFFFF);

    lv_refr_now(disp);
    lvgl_port_unlock();
}

    /* ── 5. Give LVGL time to flush, then print interpretation guide ── */
    vTaskDelay(pdMS_TO_TICKS(1500));

    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "=== HOW TO READ THE RESULTS ================================");
    ESP_LOGI(TAG, "Match the captured byte[0]/byte[1] against the expected table.");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "  Matching row       → Action");
    ESP_LOGI(TAG, "  ─────────────────────────────────────────────────────────");
    ESP_LOGI(TAG, "  RGB565 swap=false  → Set rgb_endian=LCD_RGB_ENDIAN_BGR in");
    ESP_LOGI(TAG, "                       panel_config  (BGR bit in MADCTL)");
    ESP_LOGI(TAG, "  RGB565 swap=true   → swap_bytes is active; also set BGR bit");
    ESP_LOGI(TAG, "  BGR565 swap=false  → Correct! No swap_bytes, BGR bit set.");
    ESP_LOGI(TAG, "  BGR565 swap=true   → Disable swap_bytes; BGR bit already set");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "  If none match exactly → byte order AND BGR are both wrong.");
    ESP_LOGI(TAG, "============================================================");
    ESP_LOGI(TAG, "=== Test complete ===");
}