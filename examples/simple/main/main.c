
#include "lcd_device.h"
#include "ui_home.h"
#include "ui_boot.h"




void app_main(void)
{
    lcd_init();
    ui_boot_init();
    ui_home_init();
}