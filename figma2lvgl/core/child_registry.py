# child_registry.py

from figma2lvgl.core.generic_child import ChildSpec


CHILDREN = {
    "UI_CHILD_LABEL": ChildSpec(
        type_name="UI_CHILD_LABEL",
        #job_template="label_job",
        callback_template="",
        setter_template="label_setter",
        init_template="label_init",
        setter_args="const char *text",
    ),

    "UI_CHILD_IMAGE": ChildSpec(
    type_name="UI_CHILD_IMAGE",
    callback_template="",
    setter_template="image_setter",
    init_template="image_init",
    setter_args="void",
    requires_asset=True,   # NEW, main needs to know the names of image files and check if those named files exist 
    ),

    "UI_CHILD_BAR": ChildSpec(
        type_name="UI_CHILD_BAR",
        #job_template="bar_job",
        callback_template="bar_callback",
        setter_template="bar_setter",
        init_template="bar_init",
        setter_args="int value, uint32_t duration_ms"
    ),
}
