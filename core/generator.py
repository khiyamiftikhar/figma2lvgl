# generator.py

from core.utils.utils import to_snake_case, base_name_for_header
from core.emit.layouts import C_FILE_LAYOUT, H_FILE_LAYOUT
from core.child_registry import CHILDREN


# -------------------------------------------------
# Helper: ui_child_t initializer (single source)
# -------------------------------------------------

def make_child_initializer(child):
    return f"""        {{
            .type = {child.type},
            .id = "{child.id}",
            .lv_obj = NULL,
            .x = {child.x}, .y = {child.y},
            .w = {child.w}, .h = {child.h},
            .icon = NULL,
            .current_state = 0,
            .text = "",
            .initial_value = 0
        }}"""


# -------------------------------------------------
# Main generator entry point
# -------------------------------------------------

def generate_screen(screen):
    """
    Takes Screen (semantic model)
    Returns:
        c_filename,
        h_filename,
        h_text,
        c_text
    """

    # ------------------------------
    # Screen-level naming
    # ------------------------------

    frame_name = screen.name
    frame_snake = to_snake_case(frame_name)
    base = base_name_for_header(frame_snake)

    c_filename = f"{frame_snake}.c"
    h_filename = f"ui_{base}.h"

    screen_var = frame_snake
    init_fn = f"ui_{base}_init"
    guard = f"UI_{base.upper()}_H"

    # ------------------------------
    # Collected sections
    # ------------------------------

    child_entries = []
    job_structs = []
    job_callbacks = []
    setter_functions = []
    setter_prototypes = []
    init_cases = []

    emitted_job_structs = set()

    # ------------------------------
    # Init switch cases (PER TYPE)
    # ------------------------------

    for spec in CHILDREN.values():
        case = spec.emit_init_case(screen)
        if case:
            init_cases.append(case)

    # ------------------------------
    # Per-child generation
    # ------------------------------

    for index, child in enumerate(screen.children):
        spec = CHILDREN.get(child.type)
        if not spec:
            continue

        # ui_child_t initializer
        child_entries.append(
            make_child_initializer(child)
        )

        # Job struct (deduplicated)
        job_struct = spec.emit_job_struct(screen)
        if job_struct and job_struct not in emitted_job_structs:
            emitted_job_structs.add(job_struct)
            job_structs.append(job_struct)

        # Job callback
        cb = spec.emit_job_callback(screen, child)
        if cb:
            job_callbacks.append(cb)

        # Setter prototype + implementation
        proto = spec.emit_setter_prototype(screen, child)
        impl = spec.emit_setter(screen, child, index)

        if proto:
            setter_prototypes.append(proto)

        if impl:
            setter_functions.append(impl)

    # ------------------------------
    # Screen struct
    # ------------------------------

    screen_struct_lines = []
    screen_struct_lines.append(f"ui_screen_t {screen_var} = {{")
    screen_struct_lines.append(f'    .name = "{frame_name}",')
    screen_struct_lines.append(f"    .child_count = {len(child_entries)},")
    screen_struct_lines.append("    .children = {")

    for entry in child_entries:
        screen_struct_lines.append(entry + ",")

    screen_struct_lines.append("    },")
    screen_struct_lines.append("    .lv_screen = NULL")
    screen_struct_lines.append("};")

    screen_struct_def = "\n".join(screen_struct_lines)

    # ------------------------------
    # Assemble final C file
    # ------------------------------

    c_text = C_FILE_LAYOUT.format(
        header_filename=h_filename,
        screen_struct=screen_struct_def,
        job_structs="\n\n".join(job_structs),
        job_callbacks="\n\n".join(job_callbacks),
        setters="\n\n".join(setter_functions),
        init_fn=init_fn,
        screen_var=screen_var,
        init_body="\n".join(init_cases),
    )

    # ------------------------------
    # Assemble final H file
    # ------------------------------

    h_text = H_FILE_LAYOUT.format(
        guard=guard,
        init_fn=init_fn,
        setter_prototypes="\n".join(setter_prototypes),
    )

    return (
        c_filename,
        h_filename,
        h_text,
        c_text,
    )
