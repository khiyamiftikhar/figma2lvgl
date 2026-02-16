# templates/bar_templates.py

BAR_JOB_STRUCT = """
typedef struct {{
    uint8_t child_index;
    int value;
}} {job_struct};
"""

BAR_JOB_CALLBACK = """
static void {cb_name}(void *arg)
{{
    {job_struct} *job = ({job_struct} *)arg;
    ui_child_t *c = &{screen_var}.children[job->child_index];

    if(c->lv_obj)
    {{
        lv_bar_set_value(c->lv_obj, job->value, LV_ANIM_OFF);
    }}
}}
"""

BAR_SETTER = """
void {fn_name}(int value)
{{
    {job_struct} job;
    job.child_index = {child_index};
    job.value = value;

    ui_worker_process_job({cb_name}, &job, sizeof(job));
}}
"""

BAR_INIT = """
    case UI_CHILD_BAR:
        c->lv_obj = lv_bar_create({screen_var}.lv_screen);
        lv_obj_set_pos(c->lv_obj, c->x, c->y);
        lv_obj_set_size(c->lv_obj, c->w, c->h);
        lv_bar_set_value(c->lv_obj, c->initial_value, LV_ANIM_OFF);
        break;
"""
