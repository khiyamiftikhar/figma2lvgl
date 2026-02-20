#set(DISPLAY_SRCS 
#	 priv_src/ili9486_display.c)

#if(CONFIG_FEATURE_GUI)
 #   list(APPEND GUI_SRCS 



idf_component_register(SRCS "display.c"  "priv_src/ssd1306_display.c" "priv_src/ili9486_display.c" "priv_src/driver/esp_ili9486_panel.c"
			INCLUDE_DIRS "."
			PRIV_INCLUDE_DIRS "." "priv_src/driver"
			PRIV_REQUIRES lvgl esp_lvgl_port esp_timer driver)
			#REQUIRES esp_lcd)