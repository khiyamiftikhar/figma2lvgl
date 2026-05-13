# figma_parser.py

from figma2lvgl.core.utils.utils import normalize_id, to_snake_case, sanitize_c_string, UI_MAX_STRING_LENGTH, int_attr
from figma2lvgl.core.utils.figma_helpers import map_tag_to_child_type
from figma2lvgl.core.widget_type import WidgetType
import logging

logger = logging.getLogger(__name__)


# ── Color helpers ─────────────────────────────────────────────────────────────

def _hex_to_int(hex_color: str):
    """
    "#d9d9d9" → 0xd9d9d9
    Returns None if invalid or missing.
    """
    if not hex_color:
        return None
    hex_color = hex_color.strip().lstrip("#")
    try:
        return int(hex_color, 16)
    except ValueError:
        return None


def _first_visible_fill(node):
    """
    Returns the first <fill> under <fills> that is visible.
    Skips fills with visible="false".
    """
    fills = node.find("fills")
    if fills is None:
        return None
    for fill in fills.findall("fill"):
        if fill.attrib.get("visible", "true").lower() == "false":
            continue
        return fill
    return None


def _first_stroke(node):
    """Returns the first <stroke> under <strokes>."""
    strokes = node.find("strokes")
    if strokes is None:
        return None
    return strokes.find("stroke")


# ── Style data classes ────────────────────────────────────────────────────────

class ParsedStyleBox:
    def __init__(self):
        self.bg_color       = None   # int hex e.g. 0xFFFFFF, or None
        self.bg_opa         = None   # 0-255 or None
        self.border_color   = None   # int hex or None
        self.border_width   = None   # int or None
        self.radius         = None   # int or None


class ParsedStyleText:
    def __init__(self):
        self.color          = None   # int hex or None
        self.size           = None   # int or None
        self.align          = None   # "LEFT" / "CENTER" / "RIGHT" or None


class ParsedStyleEffects:
    def __init__(self):
        self.opacity        = None   # 0-255 or None


class ParsedStyle:
    def __init__(self):
        self.box     = ParsedStyleBox()
        self.text    = ParsedStyleText()
        self.effects = ParsedStyleEffects()

    def is_empty(self):
        """Returns True if no style properties were found — emit .style = {0}"""
        return all([
            self.box.bg_color       is None,
            self.box.bg_opa         is None,
            self.box.border_color   is None,
            self.box.border_width   is None,
            self.box.radius         is None,
            self.text.color         is None,
            self.text.size          is None,
            self.text.align         is None,
            self.effects.opacity    is None,
        ])


# ── Style extraction ──────────────────────────────────────────────────────────

def parse_style(node, child_type) -> ParsedStyle:
    """
    Type-aware style extraction from a Figma XML node.

    node        — the XML element
    child_type  — mapped UI_CHILD_* string e.g. "UI_CHILD_LABEL"
    """
    style     = ParsedStyle()
    node_type = node.attrib.get("type", "")

    # Infer text context from either Figma node type or mapped child type
    # so labels are handled correctly even if node type attribute is absent.
    # CRITICAL: child_type is now a WidgetType enum — must use WidgetType.LABEL,
    # not the string "UI_CHILD_LABEL". If this comparison is wrong, every label's
    # fill color routes to bg_color instead of text.color.
    is_text = (node_type == "TEXT") or (child_type == WidgetType.LABEL)

    # ── Fill color ────────────────────────────────────────────────────────────
    fill = _first_visible_fill(node)
    if fill is not None:
        color_int = _hex_to_int(fill.attrib.get("color"))
        if color_int is not None:
            if is_text:
                # fills on TEXT nodes = text color, not background
                style.text.color = color_int
            else:
                style.box.bg_color = color_int

        # fill-level opacity e.g. <fill opacity="0.5" ... />
        fill_opa = fill.attrib.get("opacity")
        if fill_opa is not None and not is_text:
            try:
                style.box.bg_opa = round(float(fill_opa) * 255)
            except ValueError:
                pass

    # ── Stroke / border ───────────────────────────────────────────────────────
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

    # if color was found but no width, default to 1 so border is visible in LVGL
    if style.box.border_color is not None and style.box.border_width is None:
        style.box.border_width = 1

    # ── Corner radius ─────────────────────────────────────────────────────────
    radius = node.attrib.get("cornerRadius")
    if radius is not None:
        try:
            style.box.radius = round(float(radius))
        except ValueError:
            pass

    # ── Text-only properties ──────────────────────────────────────────────────
    if is_text:
        font_size = node.attrib.get("fontSize")
        if font_size is not None:
            try:
                style.text.size = round(float(font_size))
            except ValueError:
                pass

        # FigML does not export horizontal text alignment — the attribute is
        # absent from all Text nodes. LVGL default (left) matches FigML default.
        # lv_obj_set_style_text_align is therefore not emitted for labels.

    # ── Whole-widget opacity ──────────────────────────────────────────────────
    opacity = node.attrib.get("opacity")
    if opacity is not None:
        try:
            style.effects.opacity = round(float(opacity) * 255)
        except ValueError:
            pass

    return style


# ── Parsed data classes ───────────────────────────────────────────────────────

class ParsedChild:
    def __init__(self, type, id, x, y, w, h, style=None, text_content=""):
        self.type         = type
        self.id           = id
        self.x            = x
        self.y            = y
        self.w            = w
        self.h            = h
        self.style        = style or ParsedStyle()
        self.text_content = text_content   # design-time default from Figma characters attr


class ParsedScreen:
    def __init__(self, name):
        self.name     = name
        self.snake    = to_snake_case(name)
        self.children = []

    def get_required_assets(self, child_registry):
        assets = []
        for child in self.children:
            spec = child_registry.get(child.type)
            if spec and getattr(spec, "requires_asset", False):
                assets.append(child.id)
        return assets


# ── Screen parser ─────────────────────────────────────────────────────────────

def parse_screen(frame_node):
    frame_name = frame_node.attrib.get("name", "unnamed")
    screen     = ParsedScreen(frame_name)

    children_parent = frame_node.find("children")
    if children_parent is None:
        return screen

    for child in list(children_parent):
        mapped = map_tag_to_child_type(child)
        if mapped is None:
            # FIX-2: warn with frame name so user knows exactly where to look
            logger.warning(
                "In screen '%s': skipping node '%s' (tag='%s', type='%s'). "
                "To generate a widget from this node, rename it in Figma to "
                "include one of: 'icon' or 'image' (-> lv_image), "
                "'bar' (-> lv_bar). Example: rename 'Wifi_off' -> 'icon_wifi_off'.",
                frame_name,
                child.attrib.get("name", "?"),
                child.tag,
                child.attrib.get("type", "?"),
            )
            continue

        x = int_attr(child, "x")
        y = int_attr(child, "y")
        w = int_attr(child, "width")
        h = int_attr(child, "height")

        raw_id   = child.attrib.get("name", f"child_{len(screen.children)}")
        child_id = normalize_id(raw_id)

        existing_ids = {c.id for c in screen.children}
        if child_id in existing_ids:
            raise ValueError(
                f"Duplicate child id '{child_id}' in screen '{frame_name}'"
            )

        style = parse_style(child, mapped)

        # FIX-1: extract text content from characters attribute.
        # In FigML, text content is a node attribute, not child text or element.
        # sanitize_c_string escapes quotes, backslashes, and control characters
        # (including embedded newlines which FigML may place in the attribute).
        text_content = ""
        if child.tag == "Text":
            raw_text = child.attrib.get("characters", "")
            text_content = sanitize_c_string(raw_text, UI_MAX_STRING_LENGTH)

        screen.children.append(
            ParsedChild(
                type=mapped,
                id=child_id,
                x=x, y=y, w=w, h=h,
                style=style,
                text_content=text_content,
            )
        )

    return screen