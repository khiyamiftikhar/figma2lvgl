# core/setter_emitter.py
#
# Emits setter functions and weak event callbacks for a screen.
# Setters are generated only for dynamic/interactive widgets.
# Callbacks use __attribute__((weak)) so firmware can override them.

from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.figma_parser import ParsedNode, ParsedScreen


def _setter_fn_name(screen_snake: str, path_parts: list[str]) -> str:
    """
    Build the setter function name from the path through the hierarchy.
    Max 4 path segments (screen + 3 widget segments) to prevent name explosion.
    "home", ["panel_top", "time"] → "ui_home_panel_top_time_set_text"
    """
    if len(path_parts) > 3:
        # Truncate middle segments, keep last
        path_parts = path_parts[-3:]
    return f"ui_{screen_snake}_{'_'.join(path_parts)}"


def _on_fn_name(screen_snake: str, widget_id: str) -> str:
    return f"ui_{screen_snake}_on_{widget_id}"


def _emit_label_setter(fn_base: str, path: str) -> tuple[str, str]:
    """Returns (setter_body, prototype)."""
    fn   = fn_base + "_set_text"
    body = (
        f"void {fn}(const char *text)\n"
        f"{{\n"
        f"    if ({path}.lv_obj)\n"
        f"        lv_label_set_text({path}.lv_obj, text);\n"
        f"}}"
    )
    proto = f"void {fn}(const char *text);"
    return body, proto


def _emit_image_setter(fn_base: str, path: str, node_id: str) -> tuple[str, str]:
    fn   = fn_base + "_display"   # fn_base already contains node_id via the path
    body = (
        f"void {fn}(void)\n"
        f"{{\n"
        f"    if (!{path}.lv_obj) return;\n"
        f"    {path}.src = &{node_id};\n"
        f"    lv_image_set_src({path}.lv_obj, {path}.src);\n"
        f"}}"
    )
    proto = f"void {fn}(void);"
    return body, proto


def _emit_bar_setter(fn_base: str, path: str) -> tuple[str, str]:
    fn   = fn_base + "_set_value"
    body = (
        f"void {fn}(int value, uint32_t duration_ms)\n"
        f"{{\n"
        f"    if (!{path}.lv_obj) return;\n"
        f"    if (duration_ms == 0) {{\n"
        f"        lv_bar_set_value({path}.lv_obj, value, LV_ANIM_OFF);\n"
        f"        return;\n"
        f"    }}\n"
        f"    lv_anim_t _a;\n"
        f"    lv_anim_init(&_a);\n"
        f"    lv_anim_set_var(&_a, {path}.lv_obj);\n"
        f"    lv_anim_set_exec_cb(&_a, _bar_anim_exec_cb);\n"
        f"    lv_anim_set_values(&_a, lv_bar_get_value({path}.lv_obj), value);\n"
        f"    lv_anim_set_time(&_a, duration_ms);\n"
        f"    lv_anim_start(&_a);\n"
        f"}}"
    )
    proto = f"void {fn}(int value, uint32_t duration_ms);"
    return body, proto


def _emit_button_setter(fn_base: str, path: str) -> tuple[str, str]:
    fn   = fn_base + "_set_label"
    body = (
        f"void {fn}(const char *text)\n"
        f"{{\n"
        f"    if (!{path}.lv_obj) return;\n"
        f"    lv_obj_t *_lbl = lv_obj_get_child({path}.lv_obj, 0);\n"
        f"    if (_lbl) lv_label_set_text(_lbl, text);\n"
        f"}}"
    )
    proto = f"void {fn}(const char *text);"
    return body, proto


def _emit_slider_setter(fn_base: str, path: str) -> tuple[str, str]:
    fn   = fn_base + "_set_value"
    body = (
        f"void {fn}(int32_t value)\n"
        f"{{\n"
        f"    if ({path}.lv_obj)\n"
        f"        lv_slider_set_value({path}.lv_obj, value, LV_ANIM_OFF);\n"
        f"}}"
    )
    proto = f"void {fn}(int32_t value);"
    return body, proto


def _emit_dynamic_accessor(screen_snake: str, path: str, widget_id: str) -> tuple[str, str]:
    fn   = f"ui_{screen_snake}_get_{widget_id}"
    body = (
        f"lv_obj_t *{fn}(void)\n"
        f"{{\n"
        f"    return {path}.lv_obj;\n"
        f"}}"
    )
    proto = f"lv_obj_t *{fn}(void);"
    return body, proto


def _emit_callback(cb_name: str, wt: WidgetType) -> tuple[str, str]:
    """Returns (callback_definition, declaration)."""
    if wt == WidgetType.BUTTON:
        hint = "/* override: navigate, update state, etc. */"
    else:
        hint = "/* tip: int32_t val = lv_slider_get_value(lv_event_get_target(e)); */"

    body = (
        f"__attribute__((weak)) void {cb_name}(lv_event_t *e)\n"
        f"{{\n"
        f"    (void)e;\n"
        f"    {hint}\n"
        f"}}"
    )
    decl = f"void {cb_name}(lv_event_t *e);"
    return body, decl


def collect_setters_and_callbacks(screen: ParsedScreen) -> dict:
    """
    Walk the screen tree (BFS) and collect all setters, callbacks, prototypes.
    Returns dict with keys: setters, callbacks, prototypes, cb_declarations.
    Also returns bar_anim_needed flag.
    """
    sv             = f"s_{screen.snake}"
    setters        = []
    callbacks      = []
    prototypes     = []
    cb_declarations = []
    bar_anim_needed = False

    # BFS queue: (node, struct_path, path_parts_for_name)
    queue = [
        (node, f"{sv}.{node.id}", [node.id])
        for node in screen.children
    ]

    while queue:
        node, path, name_parts = queue.pop(0)
        wt = node.widget_type
        fn_base = _setter_fn_name(screen.snake, name_parts)

        if wt == WidgetType.LABEL:
            body, proto = _emit_label_setter(fn_base, path)
            setters.append(body)
            prototypes.append(proto)

        elif wt == WidgetType.IMAGE:
            body, proto = _emit_image_setter(fn_base, path, node.id)
            setters.append(body)
            prototypes.append(proto)

        elif wt == WidgetType.BAR:
            body, proto = _emit_bar_setter(fn_base, path)
            setters.append(body)
            prototypes.append(proto)
            bar_anim_needed = True

        elif wt == WidgetType.BUTTON:
            from figma2lvgl.core.utils.figma_helpers import EVENT_SUFFIX_MAP, BUTTON_DEFAULT_EVENT
            body, proto = _emit_button_setter(fn_base, path)
            setters.append(body)
            prototypes.append(proto)

            # Always generate clicked callback
            events = [("LV_EVENT_CLICKED", "clicked")]
            for mod in node.event_modifiers:
                lvgl_event = EVENT_SUFFIX_MAP.get(mod)
                if lvgl_event and lvgl_event != BUTTON_DEFAULT_EVENT:
                    cb_suffix = lvgl_event.replace("LV_EVENT_", "").lower()
                    events.append((lvgl_event, cb_suffix))

            for _, cb_suffix in events:
                cb_name  = f"ui_{screen.snake}_on_{node.id}_{cb_suffix}"
                cb_body  = (
                    f"__attribute__((weak)) void {cb_name}(lv_event_t *e)\n"
                    f"{{\n"
                    f"    (void)e;\n"
                    f"}}"
                )
                cb_decl  = f"void {cb_name}(lv_event_t *e);"
                callbacks.append(cb_body)
                cb_declarations.append(cb_decl)

        elif wt == WidgetType.SLIDER:
            body, proto = _emit_slider_setter(fn_base, path)
            setters.append(body)
            prototypes.append(proto)
            cb_name = _on_fn_name(screen.snake, node.id)
            cb_body, cb_decl = _emit_callback(cb_name, wt)
            callbacks.append(cb_body)
            cb_declarations.append(cb_decl)

        elif wt == WidgetType.DYNAMIC:
            body, proto = _emit_dynamic_accessor(screen.snake, path, node.id)
            setters.append(body)
            prototypes.append(proto)

        # Recurse into PANEL children
        for child in node.children:
            queue.append((
                child,
                f"{path}.{child.id}",
                name_parts + [child.id],
            ))

    return {
        "setters":          "\n\n".join(setters),
        "callbacks":        "\n\n".join(callbacks),
        "prototypes":       "\n".join(prototypes),
        "cb_declarations":  "\n".join(cb_declarations),
        "bar_anim_needed":  bar_anim_needed,
    }
