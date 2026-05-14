# core/init_emitter.py
#
# Emits the flat _init() function body using BFS ordering.
# Parents are always created before their children.
# No recursion in the generated C — just sequential lv_*_create() calls.

from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.figma_parser import ParsedNode, ParsedScreen
from figma2lvgl.core.node_emitter import render_style_init


def _emit_node_init(node: ParsedNode, path: str, parent_lv: str) -> str:
    """
    Emit the LVGL creation and configuration calls for one node.
    path      — C struct path, e.g. "s_home.panel_top.time"
    parent_lv — C expression for parent lv_obj_t*, e.g. "s_home.panel_top.lv_obj"
    """
    wt    = node.widget_type
    lines = []

    # ── LABEL ────────────────────────────────────────────────────────────────
    if wt == WidgetType.LABEL:
        lines += [
            f"    /* {node.id} (label) */",
            f"    {path}.lv_obj = lv_label_create({parent_lv});",
            f"    lv_obj_set_pos({path}.lv_obj, {node.x}, {node.y});",
            f"    lv_obj_set_width({path}.lv_obj, {node.w});",
            f"    lv_label_set_long_mode({path}.lv_obj, LV_LABEL_LONG_CLIP);",
            f"    lv_label_set_text({path}.lv_obj, {path}.text);",
            f"    ui_apply_style({path}.lv_obj, UI_CHILD_LABEL, &{path}.style);",
        ]

    # ── BAR ──────────────────────────────────────────────────────────────────
    elif wt == WidgetType.BAR:
        lines += [
            f"    /* {node.id} (bar) */",
            f"    {path}.lv_obj = lv_bar_create({parent_lv});",
            f"    lv_obj_set_pos({path}.lv_obj, {node.x}, {node.y});",
            f"    lv_obj_set_size({path}.lv_obj, {node.w}, {node.h});",
            f"    lv_bar_set_range({path}.lv_obj, 0, 100); /* TODO: adjust range if needed */",
            f"    lv_bar_set_value({path}.lv_obj, {path}.value, LV_ANIM_OFF);",
            f"    ui_apply_style({path}.lv_obj, UI_CHILD_BAR, &{path}.style);",
        ]

    # ── IMAGE ─────────────────────────────────────────────────────────────────
    elif wt == WidgetType.IMAGE:
        lines += [
            f"    /* {node.id} (image) */",
            f"    {path}.lv_obj = lv_image_create({parent_lv});",
            f"    lv_obj_set_pos({path}.lv_obj, {node.x}, {node.y});",
            f"    lv_obj_set_size({path}.lv_obj, {node.w}, {node.h});",
            f"    if ({path}.src)",
            f"        lv_image_set_src({path}.lv_obj, {path}.src);",
            f"    ui_apply_style({path}.lv_obj, UI_CHILD_IMAGE, &{path}.style);",
        ]

    # ── BUTTON ────────────────────────────────────────────────────────────────
    elif wt == WidgetType.BUTTON:
        from figma2lvgl.core.utils.figma_helpers import EVENT_SUFFIX_MAP, BUTTON_DEFAULT_EVENT
        # Build list of (event_constant, callback_suffix) pairs.
        # LV_EVENT_CLICKED is always registered.
        events = [("LV_EVENT_CLICKED", "clicked")]
        for mod in node.event_modifiers:
            lvgl_event = EVENT_SUFFIX_MAP.get(mod)
            if lvgl_event and lvgl_event != BUTTON_DEFAULT_EVENT:
                cb_suffix = lvgl_event.replace("LV_EVENT_", "").lower()
                events.append((lvgl_event, cb_suffix))

        screen_snake = path.split(".")[0].lstrip("s_")
        lines += [
            f"    /* {node.id} (button) */",
            f"    {path}.lv_obj = lv_button_create({parent_lv});",
            f"    lv_obj_set_pos({path}.lv_obj, {node.x}, {node.y});",
            f"    lv_obj_set_size({path}.lv_obj, {node.w}, {node.h});",
            f"    {{",
            f"        lv_obj_t *_lbl = lv_label_create({path}.lv_obj);",
            f"        lv_label_set_text(_lbl, {path}.label_text);",
            f"        lv_obj_center(_lbl);",
            f"        /* text color/font applied to the label directly, not the container */",
            f"        ui_apply_style(_lbl, UI_CHILD_LABEL, &{path}.style);",
            f"    }}",
        ]
        for lvgl_event, cb_suffix in events:
            cb_name = f"ui_{screen_snake}_on_{node.id}_{cb_suffix}"
            lines += [
                f"    lv_obj_add_event_cb({path}.lv_obj, {cb_name},",
                f"                        {lvgl_event}, NULL);",
            ]
        lines.append(
            f"    ui_apply_style({path}.lv_obj, UI_CHILD_BUTTON, &{path}.style);"
        )

    # ── SLIDER ────────────────────────────────────────────────────────────────
    elif wt == WidgetType.SLIDER:
        cb_name = _callback_name_for(path)
        lines += [
            f"    /* {node.id} (slider) */",
            f"    {path}.lv_obj = lv_slider_create({parent_lv});",
            f"    lv_obj_set_pos({path}.lv_obj, {node.x}, {node.y});",
            f"    lv_obj_set_size({path}.lv_obj, {node.w}, {node.h});",
            f"    lv_slider_set_range({path}.lv_obj, {path}.min, {path}.max);",
            f"    lv_slider_set_value({path}.lv_obj, {path}.value, LV_ANIM_OFF);",
            f"    lv_obj_add_event_cb({path}.lv_obj, {cb_name},",
            f"                        LV_EVENT_VALUE_CHANGED, NULL);",
            f"    ui_apply_style({path}.lv_obj, UI_CHILD_SLIDER, &{path}.style);",
        ]

    # ── PANEL ─────────────────────────────────────────────────────────────────
    elif wt == WidgetType.PANEL:
        lines += [
            f"    /* {node.id} (panel) */",
            f"    {path}.lv_obj = lv_obj_create({parent_lv});",
            f"    lv_obj_set_pos({path}.lv_obj, {node.x}, {node.y});",
            f"    lv_obj_set_size({path}.lv_obj, {node.w}, {node.h});",
            f"    lv_obj_clear_flag({path}.lv_obj, LV_OBJ_FLAG_CLICKABLE);",
            f"    lv_obj_set_scrollbar_mode({path}.lv_obj, LV_SCROLLBAR_MODE_OFF);",
            f"    ui_apply_style({path}.lv_obj, UI_CHILD_PANEL, &{path}.style);",
        ]

    # ── DYNAMIC CONTAINER ─────────────────────────────────────────────────────
    elif wt == WidgetType.DYNAMIC:
        lines += [
            f"    /* {node.id} (dynamic container — firmware fills at runtime) */",
            f"    {path}.lv_obj = lv_obj_create({parent_lv});",
            f"    lv_obj_set_pos({path}.lv_obj, {node.x}, {node.y});",
            f"    lv_obj_set_size({path}.lv_obj, {node.w}, {node.h});",
            f"    lv_obj_set_scrollbar_mode({path}.lv_obj, LV_SCROLLBAR_MODE_AUTO);",
            f"    ui_apply_style({path}.lv_obj, UI_CHILD_PANEL, &{path}.style);",
        ]

    return "\n".join(lines)


def _callback_name_for(path: str) -> str:
    """
    Derive the callback function name from the struct path.
    "s_home.btn_ok" → "ui_home_on_btn_ok"
    "s_home.panel_top.brightness_slider" → "ui_home_on_brightness_slider"
    """
    parts = path.split(".")
    # parts[0] = "s_home" → "home"
    screen = parts[0].lstrip("s_")
    widget = parts[-1]
    return f"ui_{screen}_on_{widget}"


def emit_init_body(screen: ParsedScreen) -> str:
    """
    BFS traversal — emit a flat sequence of LVGL creation calls.
    Parents always appear before children.
    """
    sv     = f"s_{screen.snake}"
    lines  = []
    # BFS queue: (node, struct_path, parent_lv_obj_expr)
    queue  = [
        (node, f"{sv}.{node.id}", f"{sv}.lv_screen")
        for node in screen.children
    ]
    while queue:
        node, path, parent_lv = queue.pop(0)
        lines.append(_emit_node_init(node, path, parent_lv))
        for child in node.children:
            queue.append((child, f"{path}.{child.id}", f"{path}.lv_obj"))

    return "\n\n".join(lines)
