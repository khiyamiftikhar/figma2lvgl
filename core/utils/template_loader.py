# core/utils/template_loader.py

from core.templates import (
    label_templates,
    image_templates,
    bar_templates,
)

TEMPLATE_MAP = {
    # LABEL
    #"label_callback": label_templates.LABEL_JOB_CALLBACK,
    "label_setter": label_templates.LABEL_SETTER,
    "label_init": label_templates.LABEL_INIT,

    # IMAGE
    #"image_callback": image_templates.IMAGE_JOB_CALLBACK,
    "image_setter": image_templates.IMAGE_SETTER,
    "image_init": image_templates.IMAGE_INIT,

    # BAR
    #"bar_callback": bar_templates.BAR_JOB_CALLBACK,
    "bar_setter": bar_templates.BAR_SETTER,
    "bar_init": bar_templates.BAR_INIT,
}


def load_template(name):
    return TEMPLATE_MAP[name]