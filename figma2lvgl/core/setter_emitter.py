# core/setter_emitter.py
#
# Emits setter functions and weak event callbacks for a screen.
# Setters are generated only for dynamic/interactive widgets.
# Callbacks use __attribute__((weak)) so firmware can override them.

from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.figma_parser import ParsedNode, ParsedScreen


# Every function name comes from node.symbols, assigned design-wide by
# figma_parser.assign_design_symbols through core/naming.py (design doc 10,
# section 4.6). This module only writes the bodies.


def _emit_label_setter(fn: str, path: str) -> tuple[str, str]:
    """Returns (setter_body, prototype)."""
    body = (
        f"void {fn}(const char *text)\n"
        f"{{\n"
        f"    if ({path}.lv_obj)\n"
        f"        lv_label_set_text({path}.lv_obj, text);\n"
        f"}}"
    )
    proto = f"void {fn}(const char *text);"
    return body, proto


def _emit_image_setter(fn: str, path: str, asset: str) -> tuple[str, str]:
    body = (
        f"void {fn}(void)\n"
        f"{{\n"
        f"    if (!{path}.lv_obj) return;\n"
        f"    {path}.src = &{asset};\n"
        f"    lv_image_set_src({path}.lv_obj, {path}.src);\n"
        f"}}"
    )
    proto = f"void {fn}(void);"
    return body, proto


def _emit_bar_setter(fn: str, path: str) -> tuple[str, str]:
    body = (
        f"void {fn}(int value, uint32_t duration_ms)\n"
        f"{{\n"
        f"    if (!{path}.lv_obj) return;\n"
        f"    if (duration_ms == 0) {{\n"
        f"        lv_bar_set_value({path}.lv_obj, value, LV_ANIM_OFF);\n"
        f"        return;\n"
        f"    }}\n"
        f"    lv_anim_t anim;\n"
        f"    lv_anim_init(&anim);\n"
        f"    lv_anim_set_var(&anim, {path}.lv_obj);\n"
        f"    lv_anim_set_exec_cb(&anim, bar_anim_exec_cb);\n"
        f"    lv_anim_set_values(&anim, lv_bar_get_value({path}.lv_obj), value);\n"
        f"    lv_anim_set_time(&anim, duration_ms);\n"
        f"    lv_anim_start(&anim);\n"
        f"}}"
    )
    proto = f"void {fn}(int value, uint32_t duration_ms);"
    return body, proto


def _emit_button_setter(fn: str, path: str) -> tuple[str, str]:
    body = (
        f"void {fn}(const char *text)\n"
        f"{{\n"
        f"    if (!{path}.lv_obj) return;\n"
        f"    lv_obj_t *lbl = lv_obj_get_child({path}.lv_obj, 0);\n"
        f"    if (lbl) lv_label_set_text(lbl, text);\n"
        f"}}"
    )
    proto = f"void {fn}(const char *text);"
    return body, proto


def _emit_slider_setter(fn: str, path: str) -> tuple[str, str]:
    body = (
        f"void {fn}(int32_t value)\n"
        f"{{\n"
        f"    if ({path}.lv_obj)\n"
        f"        lv_slider_set_value({path}.lv_obj, value, LV_ANIM_OFF);\n"
        f"}}"
    )
    proto = f"void {fn}(int32_t value);"
    return body, proto


def _emit_dynamic_accessor(fn: str, path: str) -> tuple[str, str]:
    body = (
        f"lv_obj_t *{fn}(void)\n"
        f"{{\n"
        f"    return {path}.lv_obj;\n"
        f"}}"
    )
    proto = f"lv_obj_t *{fn}(void);"
    return body, proto


def _emit_callback(cb_name: str, wt: WidgetType) -> tuple[str, str]:
    """
    Returns (callback_declaration, declaration).
    No definition is emitted — implement this function in your application .c file.
    If not implemented, the linker will report an undefined reference.
    """
    decl = f"void {cb_name}(lv_event_t *e);"
    return "", decl


def collect_setters_and_callbacks(screen: ParsedScreen) -> dict:
    """
    Walk the screen tree (BFS) and collect all setters, callbacks, prototypes.
    Returns dict with keys: setters, callbacks, prototypes, cb_declarations.
    Also returns bar_anim_needed flag.
    """
    from figma2lvgl.core.figma_parser import button_events
    sv             = f"s_{screen.snake}"
    setters        = []
    callbacks      = []
    prototypes     = []
    cb_declarations = []
    bar_anim_needed = False

    queue = [(node, f"{sv}.{node.id}") for node in screen.children]

    while queue:
        node, path = queue.pop(0)
        wt = node.widget_type
        sym = node.symbols

        if wt == WidgetType.LABEL:
            body, proto = _emit_label_setter(sym["set_text"], path)
            setters.append(body)
            prototypes.append(proto)

        elif wt == WidgetType.IMAGE:
            body, proto = _emit_image_setter(sym["display"], path, node.asset_name or node.id)
            setters.append(body)
            prototypes.append(proto)

        elif wt == WidgetType.BAR:
            body, proto = _emit_bar_setter(sym["set_value"], path)
            setters.append(body)
            prototypes.append(proto)
            bar_anim_needed = True

        elif wt == WidgetType.BUTTON:
            body, proto = _emit_button_setter(sym["set_label"], path)
            setters.append(body)
            prototypes.append(proto)
            for _lv, event in button_events(node):
                cb_declarations.append(f"void {sym[('cb', event)]}(lv_event_t *e);")

        elif wt == WidgetType.SLIDER:
            body, proto = _emit_slider_setter(sym["set_value"], path)
            setters.append(body)
            prototypes.append(proto)
            cb_body, cb_decl = _emit_callback(sym[("cb", None)], wt)
            callbacks.append(cb_body)
            cb_declarations.append(cb_decl)

        elif wt == WidgetType.DYNAMIC:
            body, proto = _emit_dynamic_accessor(sym["get"], path)
            setters.append(body)
            prototypes.append(proto)

        # Recurse into PANEL children
        for child in node.children:
            queue.append((child, f"{path}.{child.id}"))

    return {
        "setters":          "\n\n".join(setters),
        "callbacks":        "\n\n".join(callbacks),
        "prototypes":       "\n".join(prototypes),
        "cb_declarations":  "\n".join(cb_declarations),
        "bar_anim_needed":  bar_anim_needed,
    }
