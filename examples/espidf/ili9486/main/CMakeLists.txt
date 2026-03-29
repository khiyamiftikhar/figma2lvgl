set(UI_COMPONENT_DIR "../../../../ui_src")

idf_component_register(
    SRCS main.c 
     	 "${UI_COMPONENT_DIR}/src/ui_ili9486_home.c"
		 "${UI_COMPONENT_DIR}/priv_src/icon_wifi.c"
         "${UI_COMPONENT_DIR}/priv_src/ui_style.c"


    INCLUDE_DIRS "."
                 "${UI_COMPONENT_DIR}/include"

    PRIV_INCLUDE_DIRS
                 "${UI_COMPONENT_DIR}/priv_include"
                 
)

