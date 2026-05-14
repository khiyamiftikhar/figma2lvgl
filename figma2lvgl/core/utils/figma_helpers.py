
import re
from figma2lvgl.core.widget_type import WidgetType


def _is_auto_named(name: str) -> bool:
    """
    Returns True for Figma auto-generated names like "Frame 12", "Group 3".
    These indicate structural/layout-only frames with no semantic meaning.
    """
    return bool(re.match(
        r'^(Frame|Group|Rectangle|Ellipse|Vector|Polygon|Star|Line)\s+\d+$',
        name, re.IGNORECASE
    ))


def _has_fill(node) -> bool:
    fills = node.find("fills")
    if fills is None:
        return False
    for fill in fills.findall("fill"):
        if fill.attrib.get("visible", "true").lower() != "false":
            return True
    return False


def _has_border(node) -> bool:
    strokes = node.find("strokes")
    if strokes is None:
        return False
    return len(list(strokes)) > 0


def _has_radius(node) -> bool:
    return node.attrib.get("cornerRadius") is not None


def detect_widget_type(node) -> WidgetType | None:
    """
    Determine the WidgetType for a Figma XML node.

    Detection priority (in order):
      1. Text node tag → LABEL
      2. Name starts with btn_/button_ → BUTTON
      3. Name starts with slider_ or ends with _slider → SLIDER
      4. Name starts with list_/grid_ → DYNAMIC (stop recursion)
      5. Name contains bar → BAR
      6. Name contains icon/image → IMAGE
      7. Has children + visual properties or meaningful name → PANEL
      8. Has children + no visual + auto-named → STRUCTURAL (drop+promote)
      9. No match → None (caller warns and skips)
    """
    tag  = node.tag
    name = node.attrib.get("name", "")
    name_lower = name.lower()

    # 1. Figma Text nodes are always labels
    if tag == "Text":
        return WidgetType.LABEL

    # 2. Interactive: button
    if name_lower.startswith(("btn_", "button_")):
        return WidgetType.BUTTON

    # 3. Interactive: slider
    if name_lower.startswith("slider_") or name_lower.endswith("_slider"):
        return WidgetType.SLIDER

    # 4. Dynamic container (list/grid — stop recursion)
    if name_lower.startswith(("list_", "grid_")):
        return WidgetType.DYNAMIC

    # 5. Passive: bar
    if "bar" in name_lower:
        return WidgetType.BAR

    # 6. Passive: image/icon
    if "icon" in name_lower or "image" in name_lower:
        return WidgetType.IMAGE

    # 7 & 8. Frame/container analysis
    children_el = node.find("children")
    has_children = children_el is not None and len(list(children_el)) > 0

    if has_children:
        has_visual     = _has_fill(node) or _has_border(node) or _has_radius(node)
        has_meaningful = not _is_auto_named(name)
        if has_visual or has_meaningful:
            return WidgetType.PANEL       # visible container — recurse
        else:
            return WidgetType.STRUCTURAL  # invisible grouping — drop+promote

    # 9. Leaf with no match
    return None
