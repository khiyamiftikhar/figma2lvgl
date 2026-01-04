# emit/layouts.py

C_FILE_LAYOUT = """
#include "{header_filename}"
#include "ui_defs.h"
#include "ui_worker.h"
#include <stdio.h>

// ------------------------------
// UI SCREEN STRUCTURE
// ------------------------------
{screen_struct}

// ------------------------------
// UI JOB DATA STRUCTS
// ------------------------------
{job_structs}

// ------------------------------
// UI JOB CALLBACKS
// ------------------------------
{job_callbacks}

// ------------------------------
// UI SETTERS
// ------------------------------
{setters}

// ------------------------------
// SCREEN INIT
// ------------------------------
// ------------------------------
// SCREEN INIT
// ------------------------------
void {init_fn}(void)
{{
    {screen_var}.lv_screen = lv_obj_create(NULL);

    for (int i = 0; i < {screen_var}.child_count; i++)
    {{
        ui_child_t *c = &{screen_var}.children[i];

        switch (c->type)
        {{
{init_body}
            default:
                break;
        }}
    }}
}}
"""


H_FILE_LAYOUT = """
#ifndef {guard}
#define {guard}

#ifdef __cplusplus
extern "C" {{
#endif

// ------------------------------
// API
// ------------------------------
void {init_fn}(void);
{setter_prototypes}

#ifdef __cplusplus
}}
#endif

#endif
"""
