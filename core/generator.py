# core/generator.py

from core.child_registry import CHILDREN
from core.emit.c_file import CFile
from core.emit.h_file import HFile
from core.emit.layouts import C_FILE_LAYOUT, H_FILE_LAYOUT
from core.utils.utils import to_snake_case
from core.utils.template_loader import load_template

def generate_screen(screen):

    screen_snake = screen.snake
    base = screen_snake
    header_filename = f"ui_{base}.h"
    guard = f"UI_{base.upper()}_H"

    init_fn = f"ui_{base}_init"
    load_fn = f"ui_{base}_load"
    load_cb = f"ui_{base}_load_job"

    # --------------------------
    # Build screen struct
    # --------------------------

    child_entries = []

    for child in screen.children:
        entry = f"""
        {{
            .type = {child.type},
            .id = "{child.id}",
            .lv_obj = NULL,
            .x = {child.x},
            .y = {child.y},
            .w = {child.w},
            .h = {child.h}
        }},"""
        child_entries.append(entry)

    screen_struct = f"""
ui_screen_t {screen_snake} = {{
    .name = "{screen.name}",
    .child_count = {len(screen.children)},
    .children = {{
        {''.join(child_entries)}
    }},
    .lv_screen = NULL
}};
"""

    # --------------------------
    # Callbacks / setters / init
    # --------------------------

    job_callbacks = []
    setters = []
    setter_prototypes = []
    init_cases = []

    for index, child in enumerate(screen.children):

        spec = CHILDREN.get(child.type)
        if not spec:
            continue

        cb_name = f"ui_{screen_snake}_set_{child.id}_job"
        fn_name = f"ui_{screen_snake}_set_{child.id}"

        # Callback
        callback_tpl = load_template(spec.callback_template)

        job_callbacks.append(
            callback_tpl.format(
                cb_name=cb_name,
                screen_var=screen_snake
            )
        )

        # Setter
        setter_tpl = load_template(spec.setter_template)

        setters.append(
            setter_tpl.format(
                fn_name=fn_name,
                child_index=index,
                screen_var=screen_snake
            )
        )
        # Prototype
        setter_prototypes.append(
            f"void {fn_name}({spec.setter_args});"
        )

        # Init
        init_tpl = load_template(spec.init_template)

        init_cases.append(
            init_tpl.format(
                screen_var=screen_snake
            )
        )

    # --------------------------
    # Assemble C file
    # --------------------------

    c_text = C_FILE_LAYOUT.format(
        header_filename=header_filename,
        screen_struct=screen_struct,
        job_structs="",  # removed permanently
        job_callbacks="\n".join(job_callbacks),
        setters="\n".join(setters),
        sc_fn_cb_name=load_cb,
        sc_fn_name=load_fn,
        init_fn=init_fn,
        screen_var=screen_snake,
        init_body="\n".join(init_cases)
    )

    # --------------------------
    # Assemble H file
    # --------------------------

    h_text = H_FILE_LAYOUT.format(
        guard=guard,
        init_fn=init_fn,
        sc_fn_name=load_fn,
        setter_prototypes="\n".join(setter_prototypes)
    )

    return (
        f"ui_{base}.c",
        header_filename,
        h_text,
        c_text
    )