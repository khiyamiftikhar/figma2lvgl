#!/usr/bin/env python3
"""
parser.py -- Generate C source/header files from Figma-like XML export.

Usage:
    python3 parser.py diagram.xml

Behavior based on user choices:
 - Filenames are snake_case (frame name -> e.g. Home_Screen -> home_screen.c)
 - Any tag starting with "icon_" -> UI_CHILD_ICON
 - LABEL text is left empty in generated structs (text set at runtime)
 - Each generated .c ONLY includes its own header file (ui_<base>.h)
"""

import sys
import os
import re
import xml.etree.ElementTree as ET

# ---------------------------
# Configuration / constants
# ---------------------------
UI_MAX_STRING_LENGTH = 64
UI_MAX_CHILDREN = 64  # if you want, adjust this
INDENT = "    "

# Mapping simple tags -> child types
# "Text" -> label
# tags starting with "icon_" -> icon
# If you want to add more mappings, change here.
def map_tag_to_child_type(tag, name_attr):
    if tag == "Text":
        return "UI_CHILD_LABEL"
    if tag.lower().startswith("icon_") or name_attr.lower().startswith("icon_"):
        return "UI_CHILD_ICON"
    # Potential extension: map "Bar" or other tags:
    if tag.lower() == "bar":
        return "UI_CHILD_BAR"
    return None

# ---------------------------
# Helpers
# ---------------------------


def normalize_id(name):
    return name.lower().replace("-", "_").replace(" ", "_")

def to_snake_case(s):
    # normalize whitespace and punctuation, then to snake_case
    s = s.strip()
    # replace non-alnum with underscore
    s = re.sub(r'[^0-9a-zA-Z]+', '_', s)
    # collapse multiple underscores
    s = re.sub(r'__+', '_', s)
    return s.lower().strip('_')

def base_name_for_header(frame_name_snake):
    # If frame name ends with "_screen", remove it for base
    if frame_name_snake.endswith("_screen"):
        return frame_name_snake[:-7]
    return frame_name_snake

def sanitize_c_string(s, maxlen=UI_MAX_STRING_LENGTH):
    if s is None:
        return ""
    s = s.replace('"', '\\"')
    if len(s) >= maxlen:
        return s[:maxlen-1]
    return s

def int_attr(node, key):
    v = node.attrib.get(key)
    if v is None or v == "":
        return 0
    try:
        return int(float(v))
    except:
        return 0

def write_file(path, text):
    with open(path, 'w', encoding='utf-8') as f:
        f.write(text)

# ---------------------------
# Code generation templates
# ---------------------------

#label_job_struct = """
#typedef struct {{
#    uint8_t child_index;
#    char text[UI_MAX_STRING_LENGTH];
#}} {struct_name};
#"""

#icon_job_struct = """
#typedef struct {{
#    uint8_t child_index;
#    uint8_t state;
#}} {struct_name};
#"""

#bar_job_struct = """
#typedef struct {{
#    uint8_t child_index;
#    int value;
#}} {struct_name};
#"""



header_template = """#ifndef {GUARD}
#define {GUARD}

#ifdef __cplusplus
extern "C" {{
#endif


// API
void {init_fn}(void);
{setters}

#ifdef __cplusplus
}}
#endif

#endif
"""

c_template = """#include "{header_filename}"

// Screen structure (auto-generated)
#include <stdint.h>
#include <stddef.h>

#extern void lv_obj_t; /* forward-declare type for readability; include lvgl headers in your project */

{screen_struct_def}

// ------------------------------
// {screen_title} INIT
// ------------------------------
void {init_fn}(void)
{{
    {screen_var}.lv_screen = lv_obj_create(NULL);

    for(int i = 0; i < {screen_var}.child_count; i++)
    {{
        ui_child_t *c = &{screen_var}.children[i];

        switch(c->type)
        {{
            case UI_CHILD_ICON:
                c->lv_obj = lv_img_create(${screen_var}.lv_screen);
                lv_obj_set_pos(c->lv_obj, c->x, c->y);

                /* enforce icon boundaries */
                lv_obj_set_size(c->lv_obj, c->w, c->h);
                lv_obj_set_style_clip_corner(
                    c->lv_obj,
                    true,
                    LV_PART_MAIN | LV_STATE_DEFAULT
                );

                lv_image_set_inner_align(c->lv_obj, LV_IMAGE_ALIGN_CENTER);

                // don't set src here; do it in separate loader
                break;


            case UI_CHILD_LABEL:
                c->lv_obj = lv_label_create({screen_var}.lv_screen);
                lv_obj_set_pos(c->lv_obj, c->x, c->y);
                lv_obj_set_style_pad_all(c->lv_obj, 0, 0);
                lv_obj_set_width(c->lv_obj, c->w);
                lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
                // text will be set separately at runtime
                break;

            case UI_CHILD_BAR:
                // Create a bar (progress) object
                c->lv_obj = lv_bar_create(${screen_var}.lv_screen);
                lv_obj_set_pos(c->lv_obj, c->x, c->y);
                lv_obj_set_size(c->lv_obj, c->w, c->h);
                lv_bar_set_value(c->lv_obj, c->initial_value, LV_ANIM_OFF);
                break;

            default:
                break;
        }}
    }}
}}

"""


# label_job_cb_template = """
# static void {cb_name}(void *arg)
# {{
#     {job_struct} *job = ({job_struct} *)arg;
#     ui_child_t *c = &${screen_var}.children[job->child_index];

#     if(c->lv_obj)
#     {{
#         lv_label_set_text(c->lv_obj, job->text);
#     }}
# }}
# """


# icon_job_cb_template = """
# static void {cb_name}(void *arg)
# {{
#     {job_struct} *job = ({job_struct} *)arg;
#     ui_child_t *c = &${screen_var}.children[job->child_index];

#     if(c->lv_obj && c->icon)
#     {{
#         c->current_state = job->state;
#         lv_img_set_src(
#             c->lv_obj,
#             c->icon->state_src[job->state]
#         );
#     }}
# }}
# """


# bar_job_cb_template = """
# static void {cb_name}(void *arg)
# {{
#     {job_struct} *job = ({job_struct} *)arg;
#     ui_child_t *c = &${screen_var}.children[job->child_index];

#     if(c->lv_obj)
#     {{
#         lv_bar_set_value(
#             c->lv_obj,
#             job->value,
#             LV_ANIM_OFF
#         );
#     }}
# }}
# """

# ---------------------------
# Main parser + generator
# ---------------------------
def generate_screen_c_and_h(frame_node):
    setter_prototypes = []
    job_callbacks = []
    setter_functions = []
    job_struct_defs = set()

    frame_name = frame_node.attrib.get('name', 'unnamed')
    frame_name_snake = to_snake_case(frame_name)
    base = base_name_for_header(frame_name_snake)

    # filenames
    c_filename = f"{frame_name_snake}.c"
    header_filename = f"ui_{base}.h"

    # header guard
    guard = f"UI_{base.upper()}_H"

    # init function name
    init_fn = f"ui_{base}_init"

    # screen variable name
    screen_var = f"{frame_name_snake}"

    # collect children
    children_parent = frame_node.find("children")
    children_nodes = []
    if children_parent is not None:
        # find direct child elements under <children>
        for child in list(children_parent):
            children_nodes.append(child)

    # make children C entries
    child_entries = []
    for child in children_nodes:
        tag = child.tag
        name_attr = child.attrib.get("name", "")
        mapped = map_tag_to_child_type(tag, name_attr)
        if mapped is None:
            # skip unknown widgets
            continue

        x = int_attr(child, "x")
        y = int_attr(child, "y")
        w = int_attr(child, "width")
        h = int_attr(child, "height")

        # per Q3: store empty text always
        text_field = ""

        # icon pointer is NULL by default
        icon_ptr = "NULL"
        current_state = 0
        initial_value = 0

        entry_lines = []
        entry_lines.append(INDENT + "{")
        entry_lines.append(INDENT*2 + f".type = {mapped},")
        entry_lines.append(INDENT*2 + f'.id = "{name_attr}",')   # NEW LINE
        entry_lines.append(INDENT*2 + ".lv_obj = NULL,")
        entry_lines.append(INDENT*2 + f".x = {x}, .y = {y},")
        entry_lines.append(INDENT*2 + f".w = {w}, .h = {h},")
        entry_lines.append(INDENT*2 + f".icon = {icon_ptr},")
        entry_lines.append(INDENT*2 + f".current_state = {current_state},")
        # text attribute (always empty as per Option B)
        entry_lines.append(INDENT*2 + f'.text = "{text_field}",')
        entry_lines.append(INDENT*2 + f".initial_value = {initial_value}")
        entry_lines.append(INDENT + "}")

        child_entries.append("\n".join(entry_lines))
        
        
        child_index = len(child_entries)
        
        raw_id = child.attrib.get("name", f"child_${child_index}")
        
        child_id = normalize_id(raw_id)

        
        base_fn = f"ui_{base}_set_{child_id}"
        
        #for label child
        if mapped == "UI_CHILD_LABEL":
            cb_name = f"{base_fn}_label_job"
            
            job_struct = f"ui_{base}_label_job_t"

            job_struct_defs.add(
                label_job_struct.format(struct_name=job_struct)
            )

            job_callbacks.append(
            label_job_cb_template.format(
                cb_name=cb_name,
                screen_var=screen_var,
                job_struct=job_struct
            )
)


            setter_prototypes.append(
                f"void {base_fn}_text(const char *text);"
            )
            
            

            setter_functions.append(f"""
            void {base_fn}_text(const char *text)
            {{
                {job_struct} job;
                job.child_index = ${child_index};
                snprintf(job.text, UI_MAX_STRING_LENGTH, "%s", text);

                ui_worker_process_job({cb_name}, &job, sizeof(job));
            }}
            """)

        #for icon child
        if mapped == "UI_CHILD_ICON":
            cb_name = f"{base_fn}_icon_job"
            
            job_struct = f"ui_{base}_icon_job_t"

            job_struct_defs.add(
                icon_job_struct.format(struct_name=job_struct)
            )

            job_callbacks.append(
                icon_job_cb_template.format(
                    cb_name=cb_name,
                    screen_var=screen_var,
                    job_struct=job_struct
                )
            )

            setter_prototypes.append(
                f"void {base_fn}_state(uint8_t state);"
            )

            

            setter_functions.append(f"""
            void {base_fn}_state(uint8_t state)
            {{
                {job_struct} job;
                job.child_index = ${child_index};
                job.state = state;

                ui_worker_process_job({cb_name}, &job);
            }}
            """)

        
        if mapped == "UI_CHILD_BAR":
            cb_name = f"{base_fn}_bar_job"
            
            job_struct = f"ui_{base}_bar_job_t"

            job_struct_defs.add(
                bar_job_struct.format(struct_name=job_struct)
            )

            job_callbacks.append(
                bar_job_cb_template.format(
                    cb_name=cb_name,
                    screen_var=screen_var,
                    job_struct=job_struct
                )
            )

            setter_prototypes.append(
                f"void {base_fn}_value(int value);"
            )

            

            setter_functions.append(f"""
            void {base_fn}_value(int value)
            {{
                {job_struct} job;
                job.child_index = ${child_index};
                job.value = value;

                ui_worker_process_job({cb_name}, &job);
            }}
            """)



        

        
        
        
        
        
        
        
        
        
        
        
        


    child_count = len(child_entries)
    children_block = ",\n".join(child_entries)
    if children_block == "":
        children_block = ""

    # Create screen struct definition string
    screen_struct_lines = []
    screen_struct_lines.append(f"ui_screen_t ${screen_var} = {{")
    screen_struct_lines.append(INDENT + f'.name = "{frame_name}",')
    screen_struct_lines.append(INDENT + f".child_count = {child_count},")
    screen_struct_lines.append(INDENT + ".children = {")
    if child_count > 0:
        screen_struct_lines.append(children_block)
    screen_struct_lines.append(INDENT + "},")
    screen_struct_lines.append(INDENT + ".lv_screen = NULL")
    screen_struct_lines.append("};")

    screen_struct_def = "\n".join(screen_struct_lines)

    # Fill header content
 
    header_text = header_template.format(
    GUARD=guard,
    init_fn=init_fn,
    setters="\n".join(setter_prototypes)
)

    # Fill c content
    c_text = c_template.format(
        header_filename=header_filename,
        screen_struct_def=screen_struct_def,
        screen_title=frame_name,
        init_fn=init_fn,
        screen_var=screen_var
    )
    
    # ------------------------------
    # Append JOB structs
    # ------------------------------

    c_text += "\n\n// ------------------------------\n"
    c_text += "// UI JOB DATA STRUCTS\n"
    c_text += "// ------------------------------\n"
    c_text += "\n".join(job_struct_defs)
    
      # ------------------------------
    # Append UI job callbacks
    # ------------------------------
    c_text += "\n\n// ------------------------------\n"
    c_text += "// UI JOB CALLBACKS\n"
    c_text += "// ------------------------------\n"
    c_text += "\n".join(job_callbacks)

    # ------------------------------
    # Append UI setter functions
    # ------------------------------
    c_text += "\n\n// ------------------------------\n"
    c_text += "// UI SETTERS\n"
    c_text += "// ------------------------------\n"
    c_text += "\n".join(setter_functions)
    
    
    return c_filename, header_filename, header_text, c_text

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 parser.py diagram.xml")
        sys.exit(1)

    xml_path = sys.argv[1]
    if not os.path.isfile(xml_path):
        print("File not found:", xml_path)
        sys.exit(1)

    try:
        tree = ET.parse(xml_path)
        root = tree.getroot()
    except Exception as e:
        print("Failed to parse XML:", e)
        sys.exit(1)

    # locate page -> children -> Frame nodes
    page_children = root.find("children")
    if page_children is None:
        print("No <children> found at top level in XML.")
        sys.exit(1)

    frames = [n for n in page_children.findall("Frame")]
    if not frames:
        print("No <Frame> nodes found in XML.")
        sys.exit(1)

    generated = []
    for frame in frames:
        c_fname, h_fname, h_text, c_text = generate_screen_c_and_h(frame)
        print(f"Generating {c_fname} and {h_fname} ...")
        write_file(h_fname, h_text)
        write_file(c_fname, c_text)
        generated.append((c_fname, h_fname))

    print("\nGenerated files:")
    for c,h in generated:
        print(f" - {c}")
        print(f" - {h}")

if __name__ == "__main__":
    main()
