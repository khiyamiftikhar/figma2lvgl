/**
 * la_color_test_v2.c  —  Logic Analyzer Diagnostic Tool v2
 * 
 * IMPROVEMENTS OVER V1:
 * 1. Uses a real GPIO trigger pin for LA sync.
 * 2. Sends ACTUAL NOP commands (0x00) as a sync marker.
 * 3. Removed the massive 320x480 black screen clear (was eating LA memory).
 * 4. Reduced block size to 4x4 pixels (just 32 bytes per color).
 * 5. Requires io_handle to send raw commands.
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "la_color.h" // Assuming this header exists for the declaration

static const char *TAG = "LA_TEST_V2";

/* ── CONFIGURATION ─────────────────────────────────────────────────────── */
// SET THIS TO AN UNUSED GPIO PIN ON YOUR BOARD!
// Connect your Logic Analyzer trigger probe to this pin.
#define LA_TRIGGER_PIN     4  

/* ── Test Geometry ─────────────────────────────────────────────────────── */
#define BLK_W   4
#define BLK_H   4
#define BLK_PIX (BLK_W * BLK_H)  // 16 pixels = 32 bytes. Perfect for LA.

/* ── RGB565 Big-Endian Pixel Values ────────────────────────────────────── */
#define PIX_RED   0xF800u
#define PIX_GREEN 0x07E0u
#define PIX_BLUE  0x001Fu
#define PIX_WHITE 0xFFFFu
#define PIX_BLACK 0x0000u

static uint16_t s_pix_buf[BLK_PIX];

static void fill_buf(uint16_t pixel_be)
{
    for (int i = 0; i < BLK_PIX; i++) {
        s_pix_buf[i] = pixel_be;
    }
}

static void trigger_high(void) {
    gpio_set_level(LA_TRIGGER_PIN, 1);
}

static void trigger_low(void) {
    gpio_set_level(LA_TRIGGER_PIN, 0);
}

/* ── Public Entry Point ────────────────────────────────────────────────── */
// NOTE: Signature changed to include io_handle!
void la_color_test_run(esp_lcd_panel_handle_t panel, esp_lcd_panel_io_handle_t io)
{
    ESP_LOGI(TAG, "=================================================");
    ESP_LOGI(TAG, "  LA TEST V2 STARTING");
    ESP_LOGI(TAG, "  Trigger Pin: GPIO %d", LA_TRIGGER_PIN);
    ESP_LOGI(TAG, "=================================================");

    /* 1. Setup Trigger GPIO */
    gpio_config_t trig_conf = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = 1ULL << LA_TRIGGER_PIN,
    };
    gpio_config(&trig_conf);
    trigger_low();

    /* Small delay to ensure LA is ready and watching */
    vTaskDelay(pdMS_TO_TICKS(1000));
    ESP_LOGI(TAG, "Arm your Logic Analyzer NOW! (Rising edge trigger on GPIO %d)", LA_TRIGGER_PIN);
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* ── FIRE TRIGGER & SEND REAL SYNC MARKER ──────────────────────── */
    trigger_high(); // <--- LA CAPTURE STARTS HERE

    // Send 3 ACTUAL NOP commands (DC=LOW, Data=0x00)
    esp_lcd_panel_io_tx_param(io, 0x00, NULL, 0);
    esp_lcd_panel_io_tx_param(io, 0x00, NULL, 0);
    esp_lcd_panel_io_tx_param(io, 0x00, NULL, 0);

    ESP_LOGI(TAG, "Trigger fired! Sending colors...");

    /* ── Color Blocks ──────────────────────────────────────────────── */
    // We don't clear the screen. We don't care about screen garbage, 
    // we only care about what the LA sees on MOSI.

    // RED Block
    fill_buf(PIX_RED);
    esp_lcd_panel_draw_bitmap(panel, 0, 0, BLK_W, BLK_H, s_pix_buf);
    vTaskDelay(pdMS_TO_TICKS(100));

    // GREEN Block
    fill_buf(PIX_GREEN);
    esp_lcd_panel_draw_bitmap(panel, 10, 0, 10 + BLK_W, BLK_H, s_pix_buf);
    vTaskDelay(pdMS_TO_TICKS(100));

    // BLUE Block
    fill_buf(PIX_BLUE);
    esp_lcd_panel_draw_bitmap(panel, 20, 0, 20 + BLK_W, BLK_H, s_pix_buf);
    vTaskDelay(pdMS_TO_TICKS(100));

    // WHITE Block
    fill_buf(PIX_WHITE);
    esp_lcd_panel_draw_bitmap(panel, 30, 0, 30 + BLK_W, BLK_H, s_pix_buf);
    vTaskDelay(pdMS_TO_TICKS(100));

    // BLACK Block
    fill_buf(PIX_BLACK);
    esp_lcd_panel_draw_bitmap(panel, 40, 0, 40 + BLK_W, BLK_H, s_pix_buf);

    trigger_low(); // <--- LA CAN STOP CAPTURING HERE

    ESP_LOGI(TAG, "=================================================");
    ESP_LOGI(TAG, "  CAPTURE COMPLETE.");
    ESP_LOGI(TAG, "  Look for 3x NOP (0x00) right after trigger.");
    ESP_LOGI(TAG, "  Then find RAMWR (0x2C). Next 32 bytes are RED.");
    ESP_LOGI(TAG, "  Expected RED: F8 00 F8 00 (or 00 F8 00 F8)");
    ESP_LOGI(TAG, "=================================================");
}