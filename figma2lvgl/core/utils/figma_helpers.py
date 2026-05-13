

from figma2lvgl.core.widget_type import WidgetType


def map_tag_to_child_type(node):
    """
    Map a Figma XML node to a WidgetType enum value.
    Returns None for unrecognized nodes — caller emits the warning with
    frame context and skips the node.
    """
    name = node.attrib.get("name", "").lower()

    if node.tag == "Text":
        return WidgetType.LABEL

    if "bar" in name:
        return WidgetType.BAR

    if "icon" in name or "image" in name:
        return WidgetType.IMAGE

    # No match — return None so parse_screen() can warn with full context
    # instead of silently producing an empty label.
    return None
