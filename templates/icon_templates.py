# templates/icon_templates.py

ICON_JOB_STRUCT = """
typedef struct {{
    uint8_t child_index;
    uint8_t state;
}} {job_struct};
"""

ICON_JOB_CALLBACK = """
static void {cb_name}(void *arg)
{{
    {job_struct} *job = ({job_struct} *)arg;
    ui_child_t *c = &{screen_var}.children[job->child_index];

    if(c->lv_obj && c->icon)
    {{
        c->current_state = job->state;
        lv_img_set_src(c->lv_obj, c->icon->state_src[job->state]);
    }}
}}
"""

ICON_SETTER = """
void {fn_name}(uint8_t state)
{{
    {job_struct} job;
    job.child_index = {child_index};
    job.state = state;

    ui_worker_process_job({cb_name}, &job, sizeof(job));
}}
"""

ICON_INIT = """
    c->lv_obj = lv_img_create({screen_var}.lv_screen);
    lv_obj_set_pos(c->lv_obj, c->x, c->y);
    lv_obj_set_size(c->lv_obj, c->w, c->h);
    lv_obj_set_style_clip_corner(c->lv_obj, true, LV_PART_MAIN);
"""
