from figma2lvgl.core.config_name import runtime_folder_name

def generate_cmake():
    runtime_dir = runtime_folder_name()

    return f"""
file(GLOB GENERATED_SOURCES
    "${{CMAKE_CURRENT_LIST_DIR}}/src_generated/*.c"
)

file(GLOB RUNTIME_SOURCES
    "${{CMAKE_CURRENT_LIST_DIR}}/runtime/*.c"
)

file(GLOB GENERATED_ASSET_SOURCES
    "${{CMAKE_CURRENT_LIST_DIR}}/assets/src_generated/*.c"
)



idf_component_register(
    SRCS
        ${{GENERATED_SOURCES}}
        ${{RUNTIME_SOURCES}}
        ${{GENERATED_ASSET_SOURCES}}
    INCLUDE_DIRS
        "src_generated"
    PRIV_INCLUDE_DIRS
        "runtime"
        "assets/src_generated"
    PRIV_REQUIRES
        lvgl
        esp_lvgl_port
)

"""
