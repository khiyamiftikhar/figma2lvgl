
idf_component_register(
    SRCS
        "generated/*.c"
        "runtime_v1_0_0/src/*.c"
    INCLUDE_DIRS
        "generated"
    PRIV_INCLUDE_DIRS
        "runtime_v1_0_0/include"
    PRIV_REQUIRES lvgl esp_lvgl_port esp_timer        
)
