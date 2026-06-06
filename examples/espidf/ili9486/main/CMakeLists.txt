set(UI_COMPONENT_DIR "../../../../ui_src")

idf_component_register(
    SRCS main.c touch.c ili9486.c
     	 "${UI_COMPONENT_DIR}/src/ui_home.c"
		 "${UI_COMPONENT_DIR}/priv_src/icon_wifi.c"
         "${UI_COMPONENT_DIR}/priv_src/wifi_off.c"
         "${UI_COMPONENT_DIR}/priv_src/ui_style.c"


    INCLUDE_DIRS "."
                 "${UI_COMPONENT_DIR}/include"

    PRIV_INCLUDE_DIRS
                 "${UI_COMPONENT_DIR}/priv_include"

    PRIV_REQUIRES esp-lcd-ili9486  esp_lcd_touch_xpt2046
                 
)

