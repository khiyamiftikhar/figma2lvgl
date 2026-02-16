from core.config_name import runtime_folder_name

def generate_cmake():
    runtime_dir = runtime_folder_name()

    return f"""
idf_component_register(
    SRCS
        "generated/*.c"
        "{runtime_dir}/src/*.c"
    INCLUDE_DIRS
        "generated"
    PRIV_INCLUDE_DIRS
        "{runtime_dir}/include"
    PRIV_REQUIRES lvgl esp_lvgl_port esp_timer        
)
"""
