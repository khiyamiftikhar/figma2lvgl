# core/widget_type.py
#
# Widget type enum replacing bare string identifiers like "UI_CHILD_LABEL".
# Using an enum means a typo (e.g. WidgetType.LABLE) raises AttributeError
# at import time rather than returning silent None from CHILDREN.get().

from enum import Enum


class WidgetType(Enum):
    LABEL = "UI_CHILD_LABEL"
    IMAGE = "UI_CHILD_IMAGE"
    BAR   = "UI_CHILD_BAR"

    def c_enum_name(self) -> str:
        """Return the C enum value string for use in generated code."""
        return self.value
