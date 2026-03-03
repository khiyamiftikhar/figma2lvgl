#set(DISPLAY_SRCS 
#	 priv_src/ili9486_display.c)

#if(CONFIG_FEATURE_GUI)
 #   list(APPEND GUI_SRCS 



idf_component_register(SRCS "display.c"  "priv_src/ssd1306_display.c" "priv_src/ili9486_display.c"
				INCLUDE_DIRS "." "priv_src"
				REQUIRES lvgl esp_lvgl_port esp_timer driver esp-lcd-ili9486)
			#REQUIRES esp_lcd)