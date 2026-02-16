# utils/template_loader.py

from templates import (
    label_templates,
    icon_templates,
    bar_templates,
)

# -------------------------------------------------
# Template registry
# -------------------------------------------------

TEMPLATE_MAP = {
    # LABEL
    "label_job": label_templates.LABEL_JOB_STRUCT,
    "label_callback": label_templates.LABEL_JOB_CALLBACK,
    "label_setter": label_templates.LABEL_SETTER,
    "label_init": label_templates.LABEL_INIT,

    # ICON
    "icon_job": icon_templates.ICON_JOB_STRUCT,
    "icon_callback": icon_templates.ICON_JOB_CALLBACK,
    "icon_setter": icon_templates.ICON_SETTER,
    "icon_init": icon_templates.ICON_INIT,

    # BAR
    "bar_job": bar_templates.BAR_JOB_STRUCT,
    "bar_callback": bar_templates.BAR_JOB_CALLBACK,
    "bar_setter": bar_templates.BAR_SETTER,
    "bar_init": bar_templates.BAR_INIT,
}


def load_template(name: str) -> str:
    """
    Return template string by symbolic name.
    """
    try:
        return TEMPLATE_MAP[name]
    except KeyError:
        raise KeyError(f"Template '{name}' not registered")
