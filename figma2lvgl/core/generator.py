# core/generator.py

from figma2lvgl.core.child_registry import CHILDREN
from figma2lvgl.core.emit.c_file import CFile
from figma2lvgl.core.emit.h_file import HFile
from figma2lvgl.core.emit.layouts import C_FILE_LAYOUT, H_FILE_LAYOUT
from figma2lvgl.core.utils.utils import to_snake_case
from figma2lvgl.core.utils.template_loader import load_template
from string import Template

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

        data_block = ""

        if child.type == "UI_CHILD_LABEL":
            data_block = """
                .data.label = {
                    .text = ""
                }
    """

        elif child.type == "UI_CHILD_IMAGE":
            data_block = """
                .data.image = {
                    .src = NULL
                }
    """

        elif child.type == "UI_CHILD_BAR":
            data_block = """
                .data.bar = {
                    .value = 0
                }
    """

        entry = f"""
            {{
                .type = {child.type},
                .id = "{child.id}",
                .lv_obj = NULL,
                .x = {child.x},
                .y = {child.y},
                .w = {child.w},
                .h = {child.h},
        {data_block}
            }},
        """
        child_entries.append(entry)

    children_block = "".join(child_entries)

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

    # --------------------------
    # Callbacks / setters
    # --------------------------

    job_callbacks = []
    setters = []
    setter_prototypes = []

    unique_types = set()

    # --------------------------
    # CHILD LOOP → setters only
    # --------------------------

    for index, child in enumerate(screen.children):

        spec = CHILDREN.get(child.type)
        if not spec:
            continue

        unique_types.add(child.type)

        # Determine callback + API name per type
        if child.type == "UI_CHILD_LABEL":
            cb_name = ""#f"ui_{screen_snake}_label_job_cb"
            fn_name = f"ui_{screen_snake}_set_{child.id}"

        elif child.type == "UI_CHILD_IMAGE":
            cb_name = ""#f"ui_{screen_snake}_display_image_job_cb"
            fn_name = f"ui_{screen_snake}_display_{child.id}"

        elif child.type == "UI_CHILD_BAR":
            cb_name = f"ui_{screen_snake}_bar_job_cb"
            fn_name = f"ui_{screen_snake}_set_{child.id}"

        else:
            continue

        # ----- Setter generation (PER CHILD) -----
        setter_tpl = load_template(spec.setter_template)

        setters.append(
            Template(setter_tpl).safe_substitute(
                fn_name=fn_name,
                child_index=index,
                screen_var=screen_snake,
                child_id=child.id,
                cb_name=cb_name
            )
        )

        setter_prototypes.append(
            f"void {fn_name}({spec.setter_args});"
        )


    # --------------------------
    # TYPE LOOP → callbacks only
    # --------------------------

    for type_name in unique_types:

        spec = CHILDREN.get(type_name)
        if not spec:
            continue

        if type_name == "UI_CHILD_LABEL":
            cb_name = ""#f"ui_{screen_snake}_label_job_cb"

        elif type_name == "UI_CHILD_IMAGE":
            cb_name = ""#f"ui_{screen_snake}_display_image_job_cb"

        elif type_name == "UI_CHILD_BAR":
            cb_name = f"ui_{screen_snake}_bar_job_cb"

        else:
           continue

        callback_tpl = load_template(spec.callback_template)

        job_callbacks.append(
            Template(callback_tpl).safe_substitute(
                cb_name=cb_name,
                screen_var=screen_snake
            )
        )


    # --------------------------
    # Init cases (ONE PER TYPE)
    # --------------------------

    init_cases = []

    for type_name in unique_types:

        spec = CHILDREN.get(type_name)
        if not spec:
            continue

        init_tpl = load_template(spec.init_template)

        init_cases.append(
            Template(init_tpl).safe_substitute(
                screen_var=screen_snake
            )
        )
    # --------------------------
    # Assemble C file
    # --------------------------

#    from string import Template

    # --------------------------
    # Assemble C file
    # --------------------------

    c_text = Template(C_FILE_LAYOUT).safe_substitute(
        header_filename=header_filename,
        screen_struct=screen_struct,
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

    h_text = Template(H_FILE_LAYOUT).safe_substitute(
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