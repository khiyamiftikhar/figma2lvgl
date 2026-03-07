set(UI_COMPONENT_DIR "../../../../ui_component")

idf_component_register(
    SRCS main.c 
     	 "${UI_COMPONENT_DIR}/src_generated/ui_home.c"
		 "${UI_COMPONENT_DIR}/assets/src_generated/icon_wifi.c"


    INCLUDE_DIRS "."
                 "${UI_COMPONENT_DIR}/src_generated"

    PRIV_INCLUDE_DIRS
                 "${UI_COMPONENT_DIR}/runtime"
                 "${UI_COMPONENT_DIR}/assets/src_generated"
)

