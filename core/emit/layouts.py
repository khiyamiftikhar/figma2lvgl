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
// SCREEN LOAD
// ------------------------------

// ------------------------------
// SCREEN LOAD CB
// ------------------------------

static void {sc_fn_cb_name}(void *arg)
{{
    lv_scr_load(${screen_var}.lv_screen);
}}



// ------------------------------
// SCREEN LOAD
// ------------------------------

void {sc_fn_name}(void)
{{
    
    ui_worker_process_job({sc_fn_cb_name}, NULL, 0);

}}

// ------------------------------
// SCREEN INIT
// ------------------------------
// ------------------------------
// SCREEN INIT
// ------------------------------
void {init_fn}(void)
{{
    ${screen_var}.lv_screen = lv_obj_create(NULL);

    for (int i = 0; i < ${screen_var}.child_count; i++)
    {{
        ui_child_t *c = &${screen_var}.children[i];

        switch (c->type)
        {{
{init_body}
            default:
                break;
        }}
    }}
    
    ui_worker_init();
}}
"""


H_FILE_LAYOUT = """
#ifndef {guard}
#define {guard}

#ifdef __cplusplus
extern "C" {{
#endif
#include <stdint.h>
// ------------------------------
// API
// ------------------------------
void {init_fn}(void);
void {sc_fn_name}(void);
{setter_prototypes}

#ifdef __cplusplus
}}
#endif

#endif
"""
