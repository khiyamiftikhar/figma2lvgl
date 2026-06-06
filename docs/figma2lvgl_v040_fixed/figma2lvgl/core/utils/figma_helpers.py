
import re
from figma2lvgl.core.widget_type import WidgetType


# ── Behavioral modifier parsing ───────────────────────────────────────────────
#
# Figma widget names encode two things that must be separated:
#   1. Identity   — becomes the C struct field name and API prefix
#   2. Modifiers  — consumed by the generator to produce code variations
#
# Rule: strip modifiers BEFORE forming the struct ID.
# Modifiers drive lv_obj_add_event_cb calls and range values, never names.
#
# Examples:
#   "btn_ok_lp"              → base "btn_ok",          mods ["lp"]
#   "btn_ok"                 → base "btn_ok",          mods []   (default: click)
#   "brightness_slider_0_255"→ base "brightness_slider", range (0,255)

EVENT_SUFFIX_MAP = {
    # key (suffix without leading _) → LVGL event constant
    "lpr":     "LV_EVENT_LONG_PRESSED_REPEAT",
    "lp":      "LV_EVENT_LONG_PRESSED",
    "release": "LV_EVENT_RELEASED",
    "press":   "LV_EVENT_PRESSED",
    "click":   "LV_EVENT_CLICKED",   # explicit; also the default
}

# Default event for buttons (always registered even with no suffix)
BUTTON_DEFAULT_EVENT = "LV_EVENT_CLICKED"


def parse_widget_name(raw_name: str) -> tuple[str, list[str]]:
    """
    Split a Figma widget name into (base_name, event_modifier_keys).

    base_name  — used for normalize_id() → struct field, API function names
    modifiers  — list of keys from EVENT_SUFFIX_MAP → drive lv_obj_add_event_cb

    Only one event modifier suffix is expected per widget.
    Suffixes are evaluated longest-first to avoid partial matches (_lp vs _lpr).

    Does NOT strip slider range numbers (_0_255) — those are stripped separately
    in _parse_children() for WidgetType.SLIDER nodes.
    """
    base = raw_name.lower()
    modifiers = []

    for key in sorted(EVENT_SUFFIX_MAP.keys(), key=len, reverse=True):
        suffix = f"_{key}"
        if base.endswith(suffix):
            modifiers.append(key)
            base = base[:-len(suffix)]
            break   # one event modifier per widget

    return base, modifiers


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


def _instance_has_vector_children(node) -> bool:
    """
    Returns True if a Figma component INSTANCE contains at least one Vector
    child. Figma icon components are always a Frame/Instance wrapping one or
    more Vector paths. This heuristic identifies them without requiring a
    specific naming convention.
    """
    children_el = node.find("children")
    if children_el is None:
        return False
    for child in children_el:
        if child.tag == "Vector":
            return True
        # Also check one level deeper (some icon components wrap in a Group)
        sub = child.find("children")
        if sub is not None:
            for grandchild in sub:
                if grandchild.tag == "Vector":
                    return True
    return False


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

    # 7. Vector nodes are Figma SVG path data — never a widget themselves
    if tag == "Vector":
        return None

    # 8. Component instances:
    #    - If the instance contains Vector children it IS an icon component
    #      (Figma icon components are always a Frame/Instance wrapping Vector paths).
    #      Treat the whole instance as IMAGE → expects <normalized_name>.png
    #    - Otherwise unknown component structure → skip with warning.
    if node.attrib.get("type", "") == "INSTANCE":
        if _instance_has_vector_children(node):
            return WidgetType.IMAGE
        return None

    # 9 & 10. Frame/container analysis
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
