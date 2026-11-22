# Generated UI from figma2lvgl. Default: ../ui_src (created by running
# figma2lvgl with -d pointing at this example, see README.md).
# Override with the FIGMA2LVGL_UI_SRC environment variable (used by CI).
if(DEFINED ENV{FIGMA2LVGL_UI_SRC})
    set(UI_SRC_DIR "$ENV{FIGMA2LVGL_UI_SRC}")
else()
    set(UI_SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/../ui_src")
endif()

# Every screen and image file figma2lvgl produced, so regenerated output
# builds without editing this list.
file(GLOB UI_SRCS
    "${UI_SRC_DIR}/src/*.c"
    "${UI_SRC_DIR}/priv_src/*.c")

if(NOT UI_SRCS)
    message(FATAL_ERROR
        "No generated UI found in ${UI_SRC_DIR}. "
        "Run figma2lvgl first (see examples/espidf/ili9486/README.md).")
endif()

idf_component_register(
    SRCS main.c touch.c ili9486.c ${UI_SRCS}
    INCLUDE_DIRS "." "${UI_SRC_DIR}/include"
    PRIV_INCLUDE_DIRS "${UI_SRC_DIR}/priv_include"
    PRIV_REQUIRES esp-lcd-ili9486 esp_lcd_touch_xpt2046
)
