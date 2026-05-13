# core/generator.py

from figma2lvgl.core.child_registry import CHILDREN
from figma2lvgl.core.emit.layouts import C_FILE_LAYOUT, H_FILE_LAYOUT
from figma2lvgl.core.utils.template_loader import load_template
from figma2lvgl.core.widget_type import WidgetType
from string import Template


# ── Style block renderer ──────────────────────────────────────────────────────

_ALIGN_MAP = {
    "LEFT":   "LV_TEXT_ALIGN_LEFT",
    "CENTER": "LV_TEXT_ALIGN_CENTER",
    "RIGHT":  "LV_TEXT_ALIGN_RIGHT",
}

def _format_sub_struct(name, fields, indent) -> str:
    field_indent = indent + "    "
    fields_str   = (",\n" + field_indent).join(fields)
    return (
        f".{name} = {{\n"
        f"{field_indent}{fields_str}\n"
        f"{indent}}}"
    )


def _render_style_block(style, indent="            ") -> str:
    """
    Convert a ParsedStyle into a C struct initializer block.
    Emits .style = {0} when no properties were found.
    Uses direct dict lookups (not Template substitution) — not affected
    by the safe_substitute → substitute migration.
    """
    if style.is_empty():
        return ".style = { .box = { 0 }, .text = { 0 }, .effects = { 0 } },"

    box = style.box
    box_fields = []
    if box.bg_color is not None:
        box_fields += [".has_bg = true", f".bg = 0x{box.bg_color:06X}"]
    if box.bg_opa is not None:
        box_fields += [".has_bg_opa = true", f".bg_opa = {box.bg_opa}"]
    if box.border_color is not None:
        box_fields += [".has_border_color = true", f".border_color = 0x{box.border_color:06X}"]
    if box.border_width is not None:
        box_fields += [".has_border_width = true", f".border_width = {box.border_width}"]
    if box.radius is not None:
        box_fields += [".has_radius = true", f".radius = {box.radius}"]

    text = style.text
    text_fields = []
    if text.color is not None:
        text_fields += [".has_color = true", f".color = 0x{text.color:06X}"]
    if text.size is not None:
        text_fields += [".has_size = true", f".size = {text.size}"]
    if text.align is not None:
        text_fields += [".has_align = true", f".align = {_ALIGN_MAP[text.align]}"]

    effects = style.effects
    effects_fields = []
    if effects.opacity is not None:
        effects_fields += [".has_opacity = true", f".opacity = {effects.opacity}"]

    sub_indent = indent + "    "
    inner = []
    if box_fields:
        inner.append(_format_sub_struct("box", box_fields, sub_indent))
    if text_fields:
        inner.append(_format_sub_struct("text", text_fields, sub_indent))
    if effects_fields:
        inner.append(_format_sub_struct("effects", effects_fields, sub_indent))

    if not inner:
        return ".style = { 0 },"

    sections = (",\n" + sub_indent).join(inner)
    return (
        f".style = {{\n"
        f"{sub_indent}{sections}\n"
        f"{indent}}},"
    )


# ── Screen generator ──────────────────────────────────────────────────────────

def generate_screen(screen):

    screen_snake    = screen.snake
    base            = screen_snake
    header_filename = f"ui_{base}.h"
    guard           = f"UI_{base.upper()}_H"
    init_fn         = f"ui_{base}_init"
    load_fn         = f"ui_{base}_load"
    load_cb         = f"ui_{base}_load_job"

    # ── Build screen struct ───────────────────────────────────────────────────

    child_entries = []

    for child in screen.children:

        # FIX-6: child.type is now WidgetType enum — use .c_enum_name() for C output
        if child.type == WidgetType.LABEL:
            data_block = f"""
                .data.label = {{
                    .text = "{child.text_content}"
                }}
    """
        elif child.type == WidgetType.IMAGE:
            data_block = """
                .data.image = {
                    .src = NULL
                }
    """
        elif child.type == WidgetType.BAR:
            data_block = """
                .data.bar = {
                    .value = 0
                }
    """
        else:
            data_block = ""

        style_block = _render_style_block(child.style)

        entry = f"""
            {{
                .type = {child.type.c_enum_name()},
                .id = "{child.id}",
                .lv_obj = NULL,
                .x = {child.x},
                .y = {child.y},
                .w = {child.w},
                .h = {child.h},
                {style_block}
        {data_block}
            }},
        """
        child_entries.append(entry)

    screen_struct = f"""
    ui_screen_t {screen_snake} = {{
        .name = "{screen.name}",
        .child_count = {len(screen.children)},
        .children = {{
            {"".join(child_entries)}
        }},
        .lv_screen = NULL
    }};
    """

    # ── FIX-9: Per-screen bar range TODO (one comment, lists all bar IDs) ────

    bar_ids = [c.id for c in screen.children if c.type == WidgetType.BAR]
    bars_comment = ""
    if bar_ids:
        bars_comment = (
            f"/* TODO: Bar range is hardcoded to 0-100 in {init_fn}() below.\n"
            f" * Adjust lv_bar_set_range() for: {', '.join(bar_ids)}\n"
            f" * If all bars share a range, consider making it a parameter. */\n"
        )

    # ── Child loop → one setter per widget instance ───────────────────────────

    job_callbacks     = []
    setters           = []
    setter_prototypes = []
    unique_types      = []   # list preserves first-seen order → deterministic output

    for index, child in enumerate(screen.children):

        spec = CHILDREN.get(child.type)
        if not spec:
            continue

        if child.type not in unique_types:
            unique_types.append(child.type)

        # FIX-7: naming derived from ChildSpec — no per-type if/elif needed
        fn_name = spec.derive_setter_name(screen_snake, child.id)
        cb_name = spec.derive_callback_name(screen_snake)

        setter_tpl = load_template(spec.setter_template)

        # FIX-5: substitute() raises KeyError on missing variables immediately
        # rather than leaving a ${variable} literal in the generated C file
        setters.append(
            Template(setter_tpl).substitute(
                fn_name=fn_name,
                child_index=index,
                screen_var=screen_snake,
                child_id=child.id,
                cb_name=cb_name,
            )
        )

        setter_prototypes.append(
            f"void {fn_name}({spec.setter_args});"
        )

    # ── Type loop → one callback + one init case per widget type ─────────────

    init_cases = []

    for type_name in unique_types:

        spec = CHILDREN.get(type_name)
        if not spec:
            continue

        # FIX-7: callback name derived from ChildSpec, no per-type branching
        cb_name = spec.derive_callback_name(screen_snake)

        callback_tpl = load_template(spec.callback_template)
        if callback_tpl:
            job_callbacks.append(
                Template(callback_tpl).substitute(
                    cb_name=cb_name,
                    screen_var=screen_snake,
                )
            )

        init_tpl = load_template(spec.init_template)
        init_cases.append(
            Template(init_tpl).substitute(
                screen_var=screen_snake,
            )
        )

    # ── Assemble C file ───────────────────────────────────────────────────────

    c_text = Template(C_FILE_LAYOUT).substitute(
        header_filename=header_filename,
        screen_struct=screen_struct,
        job_callbacks="\n".join(job_callbacks),
        setters="\n".join(setters),
        sc_fn_cb_name=load_cb,
        sc_fn_name=load_fn,
        init_fn=init_fn,
        screen_var=screen_snake,
        init_body="\n".join(init_cases),
        bars_comment=bars_comment,
    )

    # ── Assemble H file ───────────────────────────────────────────────────────

    h_text = Template(H_FILE_LAYOUT).substitute(
        guard=guard,
        init_fn=init_fn,
        sc_fn_name=load_fn,
        setter_prototypes="\n".join(setter_prototypes),
    )

    return (
        f"ui_{base}.c",
        header_filename,
        h_text,
        c_text,
    )



# ── Style block renderer ──────────────────────────────────────────────────────

_ALIGN_MAP = {
    "LEFT":   "LV_TEXT_ALIGN_LEFT",
    "CENTER": "LV_TEXT_ALIGN_CENTER",
    "RIGHT":  "LV_TEXT_ALIGN_RIGHT",
}

def _format_sub_struct(name, fields, indent) -> str:
    """
    Render one sub-struct with multiline field formatting.

    Result:
        .box = {
            .has_bg = true,
            .bg = lv_color_hex(0xFFFFFF),
        }
    """
    field_indent = indent + "    "
    fields_str   = (",\n" + field_indent).join(fields)
    return (
        f".{name} = {{\n"
        f"{field_indent}{fields_str}\n"
        f"{indent}}}"
    )


def _render_style_block(style, indent="            ") -> str:
    """
    Convert a ParsedStyle into a C struct initializer block.
    Emits .style = {0} when no properties were found.

    indent — base indentation of the .style field in the generated file.
             Passed in so this function never hardcodes whitespace.
    """
    if style.is_empty():
        return ".style = { .box = { 0 }, .text = { 0 }, .effects = { 0 } },"

    # ── box ──────────────────────────────────────────────────────────────────
    box = style.box
    box_fields = []
    if box.bg_color is not None:
        box_fields.append(f".has_bg = true")
        box_fields.append(f".bg = 0x{box.bg_color:06X}")
    if box.bg_opa is not None:
        box_fields.append(f".has_bg_opa = true")
        box_fields.append(f".bg_opa = {box.bg_opa}")
    if box.border_color is not None:
        box_fields.append(f".has_border_color = true")
        box_fields.append(f".border_color = 0x{box.border_color:06X}")
    if box.border_width is not None:
        box_fields.append(f".has_border_width = true")
        box_fields.append(f".border_width = {box.border_width}")
    if box.radius is not None:
        box_fields.append(f".has_radius = true")
        box_fields.append(f".radius = {box.radius}")

    # ── text ─────────────────────────────────────────────────────────────────
    text = style.text
    text_fields = []
    if text.color is not None:
        text_fields.append(f".has_color = true")
        text_fields.append(f".color = 0x{text.color:06X}")
    if text.size is not None:
        text_fields.append(f".has_size = true")
        text_fields.append(f".size = {text.size}")
    if text.align is not None:
        text_fields.append(f".has_align = true")
        text_fields.append(f".align = {_ALIGN_MAP[text.align]}")

    # ── effects ───────────────────────────────────────────────────────────────
    effects = style.effects
    effects_fields = []
    if effects.opacity is not None:
        effects_fields.append(f".has_opacity = true")
        effects_fields.append(f".opacity = {effects.opacity}")

    # ── assemble ──────────────────────────────────────────────────────────────
    inner = []
    sub_indent = indent + "    "

    if box_fields:
        inner.append(_format_sub_struct("box", box_fields, sub_indent))
    if text_fields:
        inner.append(_format_sub_struct("text", text_fields, sub_indent))
    if effects_fields:
        inner.append(_format_sub_struct("effects", effects_fields, sub_indent))

    if not inner:
        return ".style = { 0 },"

    sections = (",\n" + sub_indent).join(inner)
    return (
        f".style = {{\n"
        f"{sub_indent}{sections}\n"
        f"{indent}}},"
    )
