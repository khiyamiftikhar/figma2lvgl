# core/node_emitter.py
#
# Emits the C struct type definition and static initializer for a screen.
# The struct mirrors the Figma node hierarchy exactly.

from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.figma_parser import ParsedNode, ParsedStyle

_ALIGN_MAP = {
    "LEFT":   "LV_TEXT_ALIGN_LEFT",
    "CENTER": "LV_TEXT_ALIGN_CENTER",
    "RIGHT":  "LV_TEXT_ALIGN_RIGHT",
}


# ── Style block renderer ──────────────────────────────────────────────────────

def render_style_init(style: ParsedStyle, indent: str = "        ") -> str:
    """Emit the .style = { ... } initializer fragment for a node."""
    if style.is_empty():
        return f"{indent}.style = {{ .box = {{0}}, .text = {{0}}, .effects = {{0}} }},"

    sub = indent + "    "
    box_fields, text_fields, effect_fields = [], [], []

    b = style.box
    if b.bg_color     is not None: box_fields += [".has_bg = true",           f".bg = 0x{b.bg_color:06X}"]
    if b.bg_opa       is not None: box_fields += [".has_bg_opa = true",        f".bg_opa = {b.bg_opa}"]
    if b.border_color is not None: box_fields += [".has_border_color = true",  f".border_color = 0x{b.border_color:06X}"]
    if b.border_width is not None: box_fields += [".has_border_width = true",  f".border_width = {b.border_width}"]
    if b.radius       is not None: box_fields += [".has_radius = true",        f".radius = {b.radius}"]

    t = style.text
    if t.color is not None: text_fields += [".has_color = true", f".color = 0x{t.color:06X}"]
    if t.size  is not None: text_fields += [".has_size = true",  f".size = {t.size}"]
    if t.align is not None: text_fields += [".has_align = true", f".align = {_ALIGN_MAP[t.align]}"]

    e = style.effects
    if e.opacity is not None: effect_fields += [".has_opacity = true", f".opacity = {e.opacity}"]

    parts = []
    if box_fields:
        parts.append(f".box = {{ {', '.join(box_fields)} }}")
    if text_fields:
        parts.append(f".text = {{ {', '.join(text_fields)} }}")
    if effect_fields:
        parts.append(f".effects = {{ {', '.join(effect_fields)} }}")

    if not parts:
        return f"{indent}.style = {{0}},"

    body = (",\n" + sub).join(parts)
    return f"{indent}.style = {{\n{sub}{body}\n{indent}}},"


# ── Struct field block (type definition, recursive) ───────────────────────────

def emit_struct_fields(node: ParsedNode, indent: str = "    ") -> str:
    """
    Emit the struct fields for one node — used inside the screen struct definition.
    Recurses into children for PANEL nodes.
    """
    lines = []
    lines.append(f"{indent}lv_obj_t   *lv_obj;")
    lines.append(f"{indent}ui_style_t  style;")

    wt = node.widget_type

    if wt == WidgetType.LABEL:
        if node.is_dynamic_text:
            lines.append(f"{indent}char        text[UI_MAX_STRING_LENGTH];")
        else:
            lines.append(f"{indent}const char *text;")

    elif wt == WidgetType.IMAGE:
        lines.append(f"{indent}const lv_image_dsc_t *src;")

    elif wt == WidgetType.BAR:
        lines.append(f"{indent}int32_t value;")

    elif wt == WidgetType.BUTTON:
        lines.append(f"{indent}const char *label_text;")

    elif wt == WidgetType.SLIDER:
        lines.append(f"{indent}int32_t value;")
        lines.append(f"{indent}int32_t min;")
        lines.append(f"{indent}int32_t max;")

    # Recurse into children (PANEL)
    child_indent = indent + "    "
    for child in node.children:
        child_body = emit_struct_fields(child, child_indent)
        lines.append(f"{indent}struct {{")
        lines.append(child_body)
        lines.append(f"{indent}}} {child.id};")

    return "\n".join(lines)


def emit_screen_struct_type(screen) -> str:
    """
    Emit the full static struct type + variable declaration for a screen.
    """
    sv   = f"s_{screen.snake}"
    ind  = "    "
    ind2 = "        "
    lines = [f"static struct {{"]
    lines.append(f"{ind}lv_obj_t *lv_screen;")

    for node in screen.children:
        node_body = emit_struct_fields(node, ind2)
        lines.append(f"{ind}struct {{")
        lines.append(node_body)
        lines.append(f"{ind}}} {node.id};")

    lines.append(f"}} {sv} = {{")
    # initializer
    init = emit_screen_initializer(screen)
    lines.append(init)
    lines.append("};")
    return "\n".join(lines)


# ── Struct initializer (recursive) ───────────────────────────────────────────

def emit_node_initializer(node: ParsedNode, indent: str = "    ") -> str:
    """
    Emit the designated initializer block for one node.
    Only emits fields that have non-default values.
    """
    lines = []
    wt = node.widget_type

    if wt == WidgetType.LABEL and node.text_content:
        if node.is_dynamic_text:
            lines.append(f'{indent}.text = "{node.text_content}",')
        else:
            lines.append(f'{indent}.text = "{node.text_content}",')

    elif wt == WidgetType.BUTTON and node.text_content:
        lines.append(f'{indent}.label_text = "{node.text_content}",')

    elif wt == WidgetType.SLIDER:
        lines.append(f"{indent}.value = 0,")
        lines.append(f"{indent}.min   = {node.slider_min},")
        lines.append(f"{indent}.max   = {node.slider_max},")

    # Recurse for children
    child_ind = indent + "    "
    for child in node.children:
        child_body = emit_node_initializer(child, child_ind)
        if child_body.strip():
            lines.append(f"{indent}.{child.id} = {{")
            lines.append(child_body)
            lines.append(f"{indent}}},")

    return "\n".join(lines)


def emit_screen_initializer(screen) -> str:
    """Emit the = { ... } initializer body for the screen struct."""
    ind  = "    "
    ind2 = "        "
    lines = []
    for node in screen.children:
        body = emit_node_initializer(node, ind2)
        if body.strip():
            lines.append(f"{ind}.{node.id} = {{")
            lines.append(body)
            lines.append(f"{ind}}},")
    return "\n".join(lines)
