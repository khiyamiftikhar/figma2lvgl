
#ifndef UI_TESTSCREEN_H
#define UI_TESTSCREEN_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
// ------------------------------
// API
// ------------------------------
void ui_testscreen_init(void);
void ui_testscreen_load(void);
void ui_testscreen_set_greeting(const char *text);
void ui_testscreen_set_battery_bar(int value, uint32_t duration_ms);
#ifdef __cplusplus
}
#endif
#endif
