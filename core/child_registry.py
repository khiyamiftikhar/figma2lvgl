# child_registry.py

from core.generic_child import ChildSpec


CHILDREN = {
    "UI_CHILD_LABEL": ChildSpec(
        type_name="UI_CHILD_LABEL",
        job_template="label_job",
        callback_template="label_callback",
        setter_template="label_setter",
        init_template="label_init",
        setter_args="const char *text",
    ),

    "UI_CHILD_ICON": ChildSpec(
        type_name="UI_CHILD_ICON",
        job_template="icon_job",
        callback_template="icon_callback",
        setter_template="icon_setter",
        init_template="icon_init",
        setter_args="uint8_t state",
    ),

    "UI_CHILD_BAR": ChildSpec(
        type_name="UI_CHILD_BAR",
        job_template="bar_job",
        callback_template="bar_callback",
        setter_template="bar_setter",
        init_template="bar_init",
        setter_args="int value",
    ),
}
