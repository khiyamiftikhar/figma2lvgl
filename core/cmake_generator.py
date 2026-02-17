from core.config_name import runtime_folder_name

def generate_cmake():
    runtime_dir = runtime_folder_name()

    return f"""
file(GLOB GENERATED_SOURCES
    "${{CMAKE_CURRENT_LIST_DIR}}/generated/*.c"
)

file(GLOB RUNTIME_SOURCES
    "${{CMAKE_CURRENT_LIST_DIR}}/runtime_v1_0_0/src/*.c"
)

idf_component_register(
    SRCS
        ${{GENERATED_SOURCES}}
        ${{RUNTIME_SOURCES}}
    INCLUDE_DIRS
        "generated"
    PRIV_INCLUDE_DIRS
        "runtime_v1_0_0/include"
    PRIV_REQUIRES
        lvgl
        esp_lvgl_port
)

"""
