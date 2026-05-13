# child_registry.py

from figma2lvgl.core.generic_child import ChildSpec
from figma2lvgl.core.widget_type import WidgetType


CHILDREN = {
    WidgetType.LABEL: ChildSpec(
        type_name             = WidgetType.LABEL,
        callback_template     = "",
        setter_template       = "label_setter",
        init_template         = "label_init",
        setter_args           = "const char *text",
        setter_name_pattern   = "ui_{screen}_set_{child_id}",
        callback_name_pattern = "",
    ),

    WidgetType.IMAGE: ChildSpec(
        type_name             = WidgetType.IMAGE,
        callback_template     = "",
        setter_template       = "image_setter",
        init_template         = "image_init",
        setter_args           = "void",
        requires_asset        = True,
        setter_name_pattern   = "ui_{screen}_display_{child_id}",
        callback_name_pattern = "",
    ),

    WidgetType.BAR: ChildSpec(
        type_name             = WidgetType.BAR,
        callback_template     = "bar_callback",
        setter_template       = "bar_setter",
        init_template         = "bar_init",
        setter_args           = "int value, uint32_t duration_ms",
        setter_name_pattern   = "ui_{screen}_set_{child_id}",
        callback_name_pattern = "ui_{screen}_bar_job_cb",
    ),
}

