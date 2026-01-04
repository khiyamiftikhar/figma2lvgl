# generator.py

from utils.utils import to_snake_case, base_name_for_header

from emit.layouts import C_FILE_LAYOUT, H_FILE_LAYOUT

from children.label import LabelEmitter
from children.icon import IconEmitter
from children.bar import BarEmitter


# -------------------------------------------------
# Child emitters registry
# -------------------------------------------------

EMITTERS = {
    e.type_name: e
    for e in (
        LabelEmitter(),
        IconEmitter(),
        BarEmitter(),
    )
}


# -------------------------------------------------
# Main generator entry point
# -------------------------------------------------

def generate_screen(screen):
    """
    Takes ParsedScreen
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
        # Init switch cases (per type)
        # ------------------------------

    for emitter in EMITTERS.values():
        case = emitter.emit_init_case(screen)
        if case:
            init_cases.append(case)

    # ------------------------------
    # Emit per-child content
    # ------------------------------

    for index, child in enumerate(screen.children):
        emitter = EMITTERS.get(child.type)
        if not emitter:
            continue



        
                
        

        # ui_child_t initializer
        child_entries.append(
            emitter.emit_child_initializer(child, index)
        )

        # Job struct (deduplicated)
        job_struct = emitter.emit_job_struct_def(screen)
        if job_struct and job_struct not in emitted_job_structs:
            emitted_job_structs.add(job_struct)
            job_structs.append(job_struct)

        # Job callback
        cb = emitter.emit_job_callback(child, screen)
        if cb:
            job_callbacks.append(cb)

        # Setter prototype + implementation
        proto = emitter.emit_setter_prototype(child, screen)
        impl = emitter.emit_setter(child, screen)

        if proto:
            setter_prototypes.append(proto)

        if impl:
            setter_functions.append(impl)

        # LVGL init lines
        

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
