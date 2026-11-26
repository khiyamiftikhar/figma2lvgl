# core/figma_parser.py  (v0.4.0)
#
# Replaces flat ParsedChild list with a ParsedNode tree.
# The parser now recurses into Figma's child hierarchy, correctly
# representing containers, buttons, sliders, and dynamic lists.

import logging

from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.utils.utils import (
    normalize_id, to_snake_case, sanitize_c_string,
    UI_MAX_STRING_LENGTH, int_attr,
)
from figma2lvgl.core import naming

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
        bar_min:        int  = 0,
        bar_max:        int  = 100,
        depth:          int  = 0,
        children:       list = None,
        event_modifiers: list = None,   # e.g. ["lp"] → long press registered
        clips:          bool = False,   # PANEL: clip children (Figma clipsContent)
    ):
        self.widget_type     = widget_type
        self.id              = id
        self.raw_name        = raw_name
        self.x = x; self.y = y; self.w = w; self.h = h
        self.style           = style
        self.text_content    = text_content
        self.slider_min      = slider_min
        self.slider_max      = slider_max
        self.bar_min         = bar_min
        self.bar_max         = bar_max
        self.depth           = depth
        self.children        = children if children is not None else []
        self.event_modifiers = event_modifiers if event_modifiers is not None else []
        self.clips           = clips
        self.layer:        str = id          # Figma layer name, for messages
        self.asset_name:   str | None = None # IMAGE: symbol and PNG stem (4.4)
        self.component_id: str | None = None # IMAGE: mainComponent id
        self.symbols:      dict = {}         # external symbols (assign_design_symbols)

    @property
    def is_dynamic_text(self) -> bool:
        """True → char[] in RAM + setter. False → const char * in Flash."""
        return self.widget_type not in (WidgetType.BUTTON,)


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

# Nodes with their own coordinate space (design doc 10, section 3.5). The
# exporter writes x/y relative to the nearest of these; Groups are
# transparent. Hand-written XML without a type attribute: the Frame tag.
_FRAME_LIKE_TYPES = {"FRAME", "COMPONENT", "INSTANCE"}


def _is_frame_like(node) -> bool:
    node_type = node.attrib.get("type")
    if node_type:
        return node_type in _FRAME_LIKE_TYPES
    return node.tag == "Frame"


def _float_attr(node, key) -> float:
    try:
        return float(node.attrib.get(key) or 0)
    except ValueError:
        return 0.0


def _event_key(info) -> list:
    """The 0.4.6 event_modifiers list for a button: [] unless the name had an
    explicit suffix (then the suffix key, e.g. ["lp"])."""
    if info.widget_type != WidgetType.BUTTON or info.base == info.layer:
        return []
    for key, (_lv, name) in naming.EVENTS.items():
        if name == info.event:
            return [key]
    return []


def _parse_children(parent_xml, screen_name: str, depth: int, counters,
                    frame_origin=(0.0, 0.0), parent_origin=(0.0, 0.0)):
    """Parse the children of one XML node.

    frame_origin:  screen position of the nearest frame-like ancestor, which
                   the children's x/y are relative to.
    parent_origin: screen position of the generated parent (a PANEL or the
                   screen), which generated positions are relative to.
    counters:      naming.Counters for this screen (made-up names, 4.2).

    Returned nodes carry candidate member names; the owner of the scope
    (parse_screen, or the PANEL branch) makes them unique with
    _claim_members, so children promoted from a dropped container share
    their new parent's scope.
    """
    children_el = parent_xml.find("children")
    if children_el is None:
        return []

    result = []
    for child_xml in children_el:
        info  = naming.classify(child_xml)
        wtype = info.widget_type
        name  = child_xml.attrib.get("name", "")
        for d in info.diagnostics:
            logger.warning("In screen '%s': %s", screen_name, d.message)

        # Screen position of this node, and of the space its children use.
        abs_x = frame_origin[0] + _float_attr(child_xml, "x")
        abs_y = frame_origin[1] + _float_attr(child_xml, "y")
        own_frame = (abs_x, abs_y) if _is_frame_like(child_xml) else frame_origin

        # Hidden in Figma: not generated, with its whole subtree
        if info.rule is naming.Rule.HIDDEN:
            logger.info("In screen '%s': skipping hidden layer '%s'.", screen_name, name)
            continue

        # Structural containers: dropped, children promoted to the parent
        if wtype == WidgetType.STRUCTURAL:
            result.extend(_parse_children(child_xml, screen_name, depth, counters,
                                          own_frame, parent_origin))
            continue

        # Skipped nodes (the diagnostic above says why)
        if wtype is None:
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

        # Struct member name before sibling uniqueness (doc 10, 4.2-4.3).
        event_mods = _event_key(info)
        node_id, diags = naming.member_candidate(info, counters)
        for d in diags:
            logger.warning("In screen '%s': %s", screen_name, d.message)

        # Image: the asset (symbol and PNG) comes from the layer's base name,
        # not the member name, so repeated layers share one picture (4.4).
        asset, component_id = None, None
        if wtype == WidgetType.IMAGE:
            if normalize_id(info.base.lower()):
                asset, diags = naming.asset_name(info.base, counters, name)
                for d in diags:
                    logger.warning("In screen '%s': %s", screen_name, d.message)
            else:
                asset = node_id
            main = child_xml.find("mainComponent")
            component_id = main.attrib.get("id") if main is not None else None

        # Position relative to the generated parent (doc 10, section 3.5).
        x = int(round(abs_x - parent_origin[0], 6))
        y = int(round(abs_y - parent_origin[1], 6))
        w = int_attr(child_xml, "width")
        h = int_attr(child_xml, "height")

        style        = parse_style(child_xml, wtype)
        text_content = ""
        slider_min, slider_max = 0, 100
        bar_min, bar_max = 0, 100
        children = []

        if wtype == WidgetType.LABEL:
            raw_text     = child_xml.attrib.get("characters", "")
            text_content = sanitize_c_string(raw_text, UI_MAX_STRING_LENGTH)

        elif wtype == WidgetType.BUTTON:
            text_content = _find_button_label(child_xml)
            # No recursion — button label is an LVGL internal detail

        elif wtype == WidgetType.SLIDER:
            slider_min, slider_max = info.value_range or (0, 100)

        elif wtype == WidgetType.BAR:
            bar_min, bar_max = info.value_range or (0, 100)

        elif wtype == WidgetType.DYNAMIC:
            pass  # firmware fills at runtime — stop recursion

        elif wtype == WidgetType.PANEL:
            children = _parse_children(child_xml, screen_name, depth + 1, counters,
                                       own_frame, (abs_x, abs_y))
            _claim_members(children, screen_name)

        result.append(ParsedNode(
            widget_type     = wtype,
            id              = node_id,
            raw_name        = name,
            x=x, y=y, w=w, h=h,
            style           = style,
            text_content    = text_content,
            slider_min      = slider_min,
            slider_max      = slider_max,
            bar_min         = bar_min,
            bar_max         = bar_max,
            depth           = depth,
            children        = children,
            event_modifiers = event_mods,
            clips           = (wtype == WidgetType.PANEL
                               and _is_frame_like(child_xml)
                               and child_xml.attrib.get("clipsContent", "").lower() == "true"),
        ))
        result[-1].layer        = name
        result[-1].asset_name   = asset
        result[-1].component_id = component_id

    return result


# ── ParsedScreen ──────────────────────────────────────────────────────────────

class ParsedScreen:
    def __init__(self, name: str, snake: str | None = None):
        self.name     = name
        # Screen name (doc 10, 4.2): from parse_design when the whole design
        # is parsed (validated, unique, reserved stems avoided); otherwise
        # 0.4.6's to_snake_case.
        self.snake    = snake if snake is not None else to_snake_case(name)
        self.children: list[ParsedNode] = []
        self.symbols:  dict = {}         # "init", "load" (assign_design_symbols)

    def get_required_assets(self) -> list[str]:
        """Return IDs of all IMAGE nodes (need matching PNG files)."""
        assets = []
        def walk(nodes):
            for node in nodes:
                if node.widget_type == WidgetType.IMAGE:
                    assets.append(node.asset_name or node.id)
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

def _claim_members(nodes, screen_name: str) -> None:
    """Make the member names of one struct's children unique (doc 10, 4.5)."""
    names, diags = naming.Allocator().claim_all([(n.id, n.layer) for n in nodes])
    for node, new in zip(nodes, names):
        node.id = new
    for d in diags:
        logger.warning("In screen '%s': %s", screen_name, d.message)


def parse_screen(frame_node, snake: str | None = None) -> ParsedScreen:
    frame_name = frame_node.attrib.get("name", "unnamed")
    screen     = ParsedScreen(frame_name, snake)
    counters   = naming.Counters()
    screen.children = _parse_children(frame_node, frame_name, depth=0, counters=counters)
    _claim_members(screen.children, frame_name)
    return screen


# Setter operations per widget type (setter_emitter adds the bodies).
SETTER_OPS = {
    WidgetType.LABEL:  ("set_text",),
    WidgetType.IMAGE:  ("display",),
    WidgetType.BAR:    ("set_value",),
    WidgetType.BUTTON: ("set_label",),
    WidgetType.SLIDER: ("set_value",),
}


def button_events(node) -> list[tuple[str, str]]:
    """(LVGL event, callback event name) for a button: clicked always, plus
    the event of an explicit suffix."""
    events = [("LV_EVENT_CLICKED", "clicked")]
    for key in node.event_modifiers:
        lv, name = naming.EVENTS[key]
        if lv != "LV_EVENT_CLICKED":
            events.append((lv, name))
    return events


def assign_design_symbols(screens) -> None:
    """Every external symbol of the design, in one allocator (doc 10, 4.6).

    Stores the names in screen.symbols ("init", "load") and node.symbols
    (setter op, ("cb", event or None), "get"). The emitters only read them.
    """
    specs = []
    for screen in screens:
        widgets = []
        for node, node_path in _nodes_dfs(screen):
            wt = node.widget_type
            widgets.append(naming.WidgetSymbols(
                key=node, path=node_path, layer=node.layer,
                setter_ops=SETTER_OPS.get(wt, ()),
                events=tuple(e for _lv, e in button_events(node)) if wt == WidgetType.BUTTON else (),
                slider_callback=(wt == WidgetType.SLIDER),
                accessor=(wt == WidgetType.DYNAMIC),
            ))
        specs.append(naming.ScreenSymbols(screen.snake, tuple(widgets)))
    names, diags = naming.assign_symbols(specs, naming.Allocator(code="flattened_collision"))
    for d in diags:
        logger.warning("%s", d.message)
    for screen in screens:
        screen.symbols = {"init": names[(screen.snake, "init")],
                          "load": names[(screen.snake, "load")]}
        for node, _path in _nodes_dfs(screen):
            node.symbols = {}
    for key, name in names.items():
        if isinstance(key[0], ParsedNode):
            node, rest = key[0], key[1:]
            node.symbols[rest if len(rest) > 1 else rest[0]] = name


def _nodes_dfs(screen):
    """(node, member path) in traversal order: depth-first, XML order (doc 10)."""
    def walk(nodes, prefix):
        for n in nodes:
            path = prefix + (n.id,)
            yield n, path
            yield from walk(n.children, path)
    return list(walk(screen.children, ()))


def parse_design(frames) -> list:
    """Parse all screens of a design, with the design-wide naming steps:
    screen names (C4, reserved stems), image identity (4.4) and external
    symbols (4.6). Raises naming.NamingError for a design the contract
    rejects."""
    raws = [f.attrib.get("name", "unnamed") for f in frames]
    snakes, diags = naming.assign_screen_names(raws)
    for d in diags:
        logger.warning("%s", d.message)
    screens = [parse_screen(f, snake) for f, snake in zip(frames, snakes)]

    uses = []
    for screen in screens:
        for node, _path in _nodes_dfs(screen):
            if node.widget_type == WidgetType.IMAGE:
                uses.append(naming.ImageUse(node.asset_name, node.layer,
                                            node.component_id, (node.w, node.h)))
    for d in naming.check_asset_identity(uses):
        logger.warning("%s", d.message)

    assign_design_symbols(screens)
    return screens
