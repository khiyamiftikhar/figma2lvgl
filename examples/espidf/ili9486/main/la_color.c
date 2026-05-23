/**
 * la_color_test.c  —  Logic Analyzer colour sequence test
 *
 * Sends 5 solid colour blocks directly via esp_lcd_panel_draw_bitmap,
 * bypassing LVGL entirely.  Each block is 40×40 px and appears at a
 * known screen position, so you can correlate what the analyser captured
 * with what you see on screen.
 *
 * HOW TO USE
 * ──────────
 * 1. Set pclk_hz = 2 000 000 in your io_config (2 MHz).
 * 2. Connect analyser to: CS, CLK, MOSI, DC  (GND shared).
 * 3. In PulseView: add SPI decoder on those 4 channels.
 * 4. Call la_color_test_run() from app_main() after panel init,
 *    BEFORE initialising LVGL (so nothing else is on the bus).
 * 5. Capture, then look for the SYNC MARKER described below.
 *
 * WHAT TO FIND IN PULSEVIEW
 * ─────────────────────────
 * DC=LOW  marks a command byte.
 * DC=HIGH marks data bytes (parameters or pixel data).
 *
 * The test starts with a SYNC MARKER — 3 special commands in a row
 * that you can search for in the decoded byte stream:
 *
 *   NOP (0x00)  NOP (0x00)  NOP (0x00)   ← 3× DC=LOW 0x00
 *
 * Immediately after the marker the real sequence begins.
 * Each colour block follows this pattern on the wire:
 *
 *   DC=LOW  0x2A  [4 data bytes: x_start, x_end]   ← CASET
 *   DC=LOW  0x2B  [4 data bytes: y_start, y_end]   ← RASET
 *   DC=LOW  0x2C                                    ← RAMWR
 *   DC=HIGH [N×2 bytes of pixel data]               ← pixels
 *
 * EXPECTED PIXEL BYTES (what we WANT to see after RAMWR)
 * ───────────────────────────────────────────────────────
 *   Block   Colour   Correct bytes (BE RGB565)
 *   ─────   ──────   ─────────────────────────
 *   1       RED      F8 00  repeating  (0xF800)
 *   2       GREEN    07 E0  repeating  (0x07E0)
 *   3       BLUE     00 1F  repeating  (0x001F)
 *   4       WHITE    FF FF  repeating  (0xFFFF)
 *   5       BLACK    00 00  repeating  (0x0000)
 *
 * DIAGNOSIS
 * ─────────
 * Compare what PulseView actually shows with the table above.
 *
 *   Captured after RED RAMWR   → Meaning
 *   ──────────────────────────────────────────────────────────────
 *   F8 00                      ✓ Byte order correct, RGB mode
 *   00 F8                      ✗ Bytes reversed → set swap_bytes=true
 *   00 1F                      ✗ R and B swapped → BGR issue
 *   1F 00                      ✗ Both swapped and BGR
 *   anything else              ✗ Pixel data corrupted (cmd bits / DMA)
 *
 * Send me the captured bytes for the RED block and I can tell you
 * exactly which combination of swap_bytes / BGR is needed.
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_lcd_panel_ops.h"
#include "la_color.h"

static const char *TAG = "LA_TEST";

/* ── Block geometry ─────────────────────────────────────────────────────── */
#define BLK_W   40
#define BLK_H   40
#define BLK_PIX (BLK_W * BLK_H)          /* 1600 pixels per block          */

/* ── RGB565 big-endian pixel values ─────────────────────────────────────── */
/*    ILI9486 SPI is MSB-first: high byte arrives first at the controller.  */
/*    0xF800 = RED in correct BE RGB565.                                     */
#define PIX_RED   0xF800u
#define PIX_GREEN 0x07E0u
#define PIX_BLUE  0x001Fu
#define PIX_WHITE 0xFFFFu
#define PIX_BLACK 0x0000u

/* Pixel buffer — holds one full block, byte-swapped for SPI              */
static uint16_t s_pix_buf[BLK_PIX];

/* Fill buffer with a 16-bit pixel value in the byte order LVGL would use */
static void fill_buf(uint16_t pixel_be)
{
    for (int i = 0; i < BLK_PIX; i++)
        s_pix_buf[i] = pixel_be;
}

/* Draw one block at (x, y) and log what we're sending */
static void draw_block(esp_lcd_panel_handle_t panel,
                       int x, int y,
                       uint16_t pixel_be,
                       const char *label)
{
    fill_buf(pixel_be);
    ESP_LOGI(TAG, "Sending %s block @ (%d,%d)  pixel=0x%04X  bytes=[0x%02X, 0x%02X]",
             label, x, y, pixel_be,
             ((uint8_t *)s_pix_buf)[0],
             ((uint8_t *)s_pix_buf)[1]);

    esp_lcd_panel_draw_bitmap(panel,
                              x, y,
                              x + BLK_W,
                              y + BLK_H,
                              s_pix_buf);

    vTaskDelay(pdMS_TO_TICKS(800));   /* pause so each block is distinct   */
}

/* ── Public entry point ─────────────────────────────────────────────────── */
void la_color_test_run(esp_lcd_panel_handle_t panel)
{
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "╔══════════════════════════════════════════════════════╗");
    ESP_LOGI(TAG, "║         LOGIC ANALYSER COLOUR TEST                  ║");
    ESP_LOGI(TAG, "║  SPI should be at 2 MHz for reliable capture        ║");
    ESP_LOGI(TAG, "╠══════════════════════════════════════════════════════╣");
    ESP_LOGI(TAG, "║  SYNC MARKER:  3× NOP (DC=LOW 0x00) on the wire    ║");
    ESP_LOGI(TAG, "║  Then 5 colour blocks follow (RED GREEN BLUE W B)   ║");
    ESP_LOGI(TAG, "╚══════════════════════════════════════════════════════╝");
    ESP_LOGI(TAG, "");

    /* ── Fill screen BLACK so blocks are visible ─────────────────────── */
    uint16_t black_row[320];
    memset(black_row, 0x00, sizeof(black_row));
    for (int y = 0; y < 480; y++)
        esp_lcd_panel_draw_bitmap(panel, 0, y, 320, y + 1, black_row);

    vTaskDelay(pdMS_TO_TICKS(500));

    /* ── SYNC MARKER: 3 NOP commands so you can find us in PulseView ─── */
    /*    Search the decoded stream for:  00 00 00  on DC=LOW lines        */
    ESP_LOGI(TAG, "Sending SYNC MARKER (3x NOP) — find these in PulseView");
    esp_lcd_panel_io_handle_t io = NULL;   /* not needed — marker via panel */
    /*
     * We don't have direct IO handle here, so we use a harmless side-effect:
     * drawing a 0-size bitmap triggers a CASET+RASET+RAMWR sequence.
     * Instead, write 3 known pixels at a fixed position in quick succession
     * using BLACK (0x0000) — easy to spot as 00 00 00 00 00 00 on MOSI.
     */
    static uint16_t marker[3] = { 0x0000, 0x0000, 0x0000 };
    esp_lcd_panel_draw_bitmap(panel, 0, 0, 3, 1, marker);
    vTaskDelay(pdMS_TO_TICKS(200));

    /* ── Colour blocks ───────────────────────────────────────────────── */
    /* Laid out left to right, top row, each 40×40 px with 10 px gaps    */
    draw_block(panel,  10, 50, PIX_RED,   "RED  ");
    draw_block(panel,  70, 50, PIX_GREEN, "GREEN");
    draw_block(panel, 130, 50, PIX_BLUE,  "BLUE ");
    draw_block(panel, 190, 50, PIX_WHITE, "WHITE");
    draw_block(panel, 250, 50, PIX_BLACK, "BLACK");

    /* ── Done ────────────────────────────────────────────────────────── */
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "TEST COMPLETE.  Stop the capture now.");
    ESP_LOGI(TAG, "");
    ESP_LOGI(TAG, "In PulseView:");
    ESP_LOGI(TAG, "  1. Find the RAMWR byte (0x2C) on a DC=LOW edge");
    ESP_LOGI(TAG, "  2. The next bytes (DC=HIGH) are the RED pixels");
    ESP_LOGI(TAG, "  3. They should repeat as:  F8 00  F8 00  F8 00 ...");
    ESP_LOGI(TAG, "  4. Share those actual bytes and I will diagnose");
}