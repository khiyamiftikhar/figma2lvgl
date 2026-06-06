# core/widget_type.py
#
# Widget type enum replacing bare string identifiers like "UI_CHILD_LABEL".
# Using an enum means a typo (e.g. WidgetType.LABLE) raises AttributeError
# at import time rather than returning silent None from CHILDREN.get().

from enum import Enum


class WidgetType(Enum):
    LABEL      = "UI_CHILD_LABEL"
    IMAGE      = "UI_CHILD_IMAGE"
    BAR        = "UI_CHILD_BAR"
    BUTTON     = "UI_CHILD_BUTTON"    # interactive — lv_button_create
    SLIDER     = "UI_CHILD_SLIDER"    # interactive — lv_slider_create
    PANEL      = "UI_CHILD_PANEL"     # container   — lv_obj_create
    DYNAMIC    = "UI_CHILD_DYNAMIC"   # runtime-filled list/grid — no children parsed
    STRUCTURAL = "_STRUCTURAL"         # invisible grouping — dropped, children promoted

    def c_enum_name(self) -> str:
        """C enum value string for use in generated code."""
        return self.value

    @property
    def is_interactive(self) -> bool:
        return self in (WidgetType.BUTTON, WidgetType.SLIDER)

    @property
    def is_container(self) -> bool:
        return self in (WidgetType.PANEL, WidgetType.DYNAMIC)
