# core/figma_parser.py  (v0.4.0)
#
# Replaces flat ParsedChild list with a ParsedNode tree.
# The parser now recurses into Figma's child hierarchy, correctly
# representing containers, buttons, sliders, and dynamic lists.

import re
import logging

from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.utils.utils import (
    normalize_id, to_snake_case, sanitize_c_string,
    UI_MAX_STRING_LENGTH, int_attr,
)
from figma2lvgl.core.utils.figma_helpers import (
    detect_widget_type, parse_widget_name,
)

logger = logging.getLogger(__name__)

MAX_DEPTH      = 5   # warn above this
MAX_DEPTH_HARD = 7   # skip subtree above this


# ── Color helpers ─────────────────────────────────────────────────────────────

def _hex_to_int(hex_color: str):
    if not hex_color:
        return None
    try:
        return int(hex_color.strip().lstrip("#"), 16)
    except ValueError:
        return None


def _first_visible_fill(node):
    fills = node.find("fills")
    if fills is None:
        return None
    for fill in fills.findall("fill"):
        if fill.attrib.get("visible", "true").lower() != "false":
            return fill
    return None


def _first_stroke(node):
    strokes = node.find("strokes")
    if strokes is None:
        return None
    return strokes.find("stroke")


# ── Style data classes ────────────────────────────────────────────────────────

class ParsedStyleBox:
    def __init__(self):
        self.bg_color     = None
        self.bg_opa       = None
        self.border_color = None
        self.border_width = None
        self.radius       = None


class ParsedStyleText:
    def __init__(self):
        self.color = None
        self.size  = None
        self.align = None


class ParsedStyleEffects:
    def __init__(self):
        self.opacity = None


class ParsedStyle:
    def __init__(self):
        self.box     = ParsedStyleBox()
        self.text    = ParsedStyleText()
        self.effects = ParsedStyleEffects()

    def is_empty(self):
        return all([
            self.box.bg_color     is None,
            self.box.bg_opa       is None,
            self.box.border_color is None,
            self.box.border_width is None,
            self.box.radius       is None,
            self.text.color       is None,
            self.text.size        is None,
            self.text.align       is None,
            self.effects.opacity  is None,
        ])


# ── Style extraction ──────────────────────────────────────────────────────────

def parse_style(node, widget_type: WidgetType) -> ParsedStyle:
    style     = ParsedStyle()
    node_type = node.attrib.get("type", "")
    is_text   = (node_type == "TEXT") or (widget_type == WidgetType.LABEL)

    fill = _first_visible_fill(node)
    if fill is not None:
        color_int = _hex_to_int(fill.attrib.get("color"))
        if color_int is not None:
            if is_text:
                style.text.color = color_int
            else:
                style.box.bg_color = color_int
        fill_opa = fill.attrib.get("opacity")
        if fill_opa is not None and not is_text:
            try:
                style.box.bg_opa = round(float(fill_opa) * 255)
            except ValueError:
                pass

    stroke = _first_stroke(node)
    if stroke is not None:
        color_int = _hex_to_int(stroke.attrib.get("color"))
        if color_int is not None:
            style.box.border_color = color_int

    stroke_weight = node.attrib.get("strokeWeight")
    if stroke_weight is not None:
        try:
            style.box.border_width = round(float(stroke_weight))
        except ValueError:
            pass
    if style.box.border_color is not None and style.box.border_width is None:
        style.box.border_width = 1

    radius = node.attrib.get("cornerRadius")
    if radius is not None:
        try:
            style.box.radius = round(float(radius))
        except ValueError:
            pass

    if is_text:
        font_size = node.attrib.get("fontSize")
        if font_size is not None:
            try:
                style.text.size = round(float(font_size))
            except ValueError:
                pass
        # FigML does not export textAlignHorizontal — LVGL default (left) applies.

    opacity = node.attrib.get("opacity")
    if opacity is not None:
        try:
            style.effects.opacity = round(float(opacity) * 255)
        except ValueError:
            pass

    return style


# ── ParsedNode ────────────────────────────────────────────────────────────────

class ParsedNode:
    """
    One UI element in the Figma hierarchy. Forms a tree — ParsedNode.children
    contains nested ParsedNode objects for PANEL containers.
    """
    def __init__(
        self,
        widget_type:    WidgetType,
        id:             str,
        raw_name:       str,
        x: int, y: int, w: int, h: int,
        style:          ParsedStyle,
        text_content:   str  = "",
        slider_min:     int  = 0,
        slider_max:     int  = 100,
        depth:          int  = 0,
        children:       list = None,
        event_modifiers: list = None,   # e.g. ["lp"] → long press registered
    ):
        self.widget_type     = widget_type
        self.id              = id
        self.raw_name        = raw_name
        self.x = x; self.y = y; self.w = w; self.h = h
        self.style           = style
        self.text_content    = text_content
        self.slider_min      = slider_min
        self.slider_max      = slider_max
        self.depth           = depth
        self.children        = children if children is not None else []
        self.event_modifiers = event_modifiers if event_modifiers is not None else []

    @property
    def is_dynamic_text(self) -> bool:
        """True → char[] in RAM + setter. False → const char * in Flash."""
        return self.widget_type not in (WidgetType.BUTTON,)


# ── Slider range helper ───────────────────────────────────────────────────────

def _parse_slider_range(name: str):
    m = re.search(r'_(n?\d+)_(n?\d+)$', name.lower())
    if m:
        def val(s): return -int(s[1:]) if s.startswith('n') else int(s)
        return val(m.group(1)), val(m.group(2))
    return 0, 100


# ── Button label helper ───────────────────────────────────────────────────────

def _find_button_label(button_xml) -> str:
    """Look one level inside button's children for a Text node's text."""
    children_el = button_xml.find("children")
    if children_el is not None:
        for child in children_el:
            if child.tag == "Text":
                raw = child.attrib.get("characters", "")
                if raw:
                    return sanitize_c_string(raw, UI_MAX_STRING_LENGTH)
    # Fall back: derive from node name
    name = button_xml.attrib.get("name", "btn").lower()
    for prefix in ("btn_", "button_"):
        if name.startswith(prefix):
            name = name[len(prefix):]
            break
    return name.replace("_", " ").title()


# ── Recursive children parser ─────────────────────────────────────────────────

def _parse_children(parent_xml, screen_name: str, depth: int, seen_ids: set):
    children_el = parent_xml.find("children")
    if children_el is None:
        return []

    result = []
    for child_xml in children_el:
        wtype = detect_widget_type(child_xml)
        name  = child_xml.attrib.get("name", "")

        # Structural frames: invisible grouping — drop and promote children
        if wtype == WidgetType.STRUCTURAL:
            result.extend(_parse_children(child_xml, screen_name, depth, seen_ids))
            continue

        # Unrecognized nodes: skip with warning
        if wtype is None:
            logger.warning(
                "In screen '%s': skipping node '%s' (tag='%s'). "
                "Rename to include a type keyword "
                "(btn_*, slider_*, list_*, bar, icon, image) "
                "or give it a fill/border to make it a panel.",
                screen_name, name, child_xml.tag,
            )
            continue

        # Hard depth limit
        if depth > MAX_DEPTH_HARD:
            logger.error(
                "Screen '%s': node '%s' exceeds max depth %d — subtree skipped.",
                screen_name, name, MAX_DEPTH_HARD,
            )
            continue

        if depth > MAX_DEPTH:
            logger.warning(
                "[UI GEN WARN] Screen '%s' depth %d at '%s'. "
                "Consider flattening (recommended max: %d).",
                screen_name, depth, name, MAX_DEPTH,
            )

        # Normalize ID — strip behavioral modifiers BEFORE forming the struct field name.
        # Modifiers (event suffixes, range numbers) are consumed by the generator
        # and must never leak into C struct names or API function names.
        raw_name_for_id = name or f"node_{len(result)}"
        base_name, event_mods = parse_widget_name(raw_name_for_id)

        # Slider range numbers: also strip from base name for a clean ID.
        # "_0_255" is configuration data, not identity.
        if wtype == WidgetType.SLIDER:
            import re as _re
            base_name = _re.sub(r'_(n?\d+)_(n?\d+)$', '', base_name)

        node_id = normalize_id(base_name)
        if node_id in seen_ids:
            node_id = f"{node_id}_{depth}"
            logger.warning("Duplicate id — renamed to '%s'", node_id)
        seen_ids.add(node_id)

        x = int_attr(child_xml, "x")
        y = int_attr(child_xml, "y")
        w = int_attr(child_xml, "width")
        h = int_attr(child_xml, "height")

        style        = parse_style(child_xml, wtype)
        text_content = ""
        slider_min, slider_max = 0, 100
        children = []

        if wtype == WidgetType.LABEL:
            raw_text     = child_xml.attrib.get("characters", "")
            text_content = sanitize_c_string(raw_text, UI_MAX_STRING_LENGTH)

        elif wtype == WidgetType.BUTTON:
            text_content = _find_button_label(child_xml)
            # No recursion — button label is an LVGL internal detail

        elif wtype == WidgetType.SLIDER:
            slider_min, slider_max = _parse_slider_range(name)

        elif wtype == WidgetType.DYNAMIC:
            pass  # firmware fills at runtime — stop recursion

        elif wtype == WidgetType.PANEL:
            children = _parse_children(child_xml, screen_name, depth + 1, seen_ids)

        result.append(ParsedNode(
            widget_type     = wtype,
            id              = node_id,
            raw_name        = name,
            x=x, y=y, w=w, h=h,
            style           = style,
            text_content    = text_content,
            slider_min      = slider_min,
            slider_max      = slider_max,
            depth           = depth,
            children        = children,
            event_modifiers = event_mods,
        ))

    return result


# ── ParsedScreen ──────────────────────────────────────────────────────────────

class ParsedScreen:
    def __init__(self, name: str):
        self.name     = name
        self.snake    = to_snake_case(name)
        self.children: list[ParsedNode] = []

    def get_required_assets(self) -> list[str]:
        """Return IDs of all IMAGE nodes (need matching PNG files)."""
        assets = []
        def walk(nodes):
            for node in nodes:
                if node.widget_type == WidgetType.IMAGE:
                    assets.append(node.id)
                walk(node.children)
        walk(self.children)
        return assets

    def all_nodes_bfs(self) -> list[tuple]:
        """
        BFS traversal — yields (node, struct_path, parent_lv_obj_expr) tuples.
        struct_path: dot-separated field path from the static struct root.
        parent_lv_obj_expr: C expression for the parent's lv_obj_t*.
        """
        sv    = f"s_{self.snake}"
        root  = f"{sv}.lv_screen"
        queue = [(node, f"{sv}.{node.id}", root) for node in self.children]
        result = []
        while queue:
            node, path, parent = queue.pop(0)
            result.append((node, path, parent))
            for child in node.children:
                queue.append((child, f"{path}.{child.id}", f"{path}.lv_obj"))
        return result


# ── Entry point ───────────────────────────────────────────────────────────────

def parse_screen(frame_node) -> ParsedScreen:
    frame_name = frame_node.attrib.get("name", "unnamed")
    screen     = ParsedScreen(frame_name)
    seen_ids: set[str] = set()
    screen.children = _parse_children(frame_node, frame_name, depth=0, seen_ids=seen_ids)
    return screen
