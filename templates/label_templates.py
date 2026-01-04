# templates/label_templates.py

LABEL_JOB_STRUCT = """
typedef struct {{
    uint8_t child_index;
    char text[UI_MAX_STRING_LENGTH];
}} {job_struct};
"""

LABEL_JOB_CALLBACK = """
static void {cb_name}(void *arg)
{{
    {job_struct} *job = ({job_struct} *)arg;
    ui_child_t *c = &{screen_var}.children[job->child_index];

    if(c->lv_obj)
    {{
        lv_label_set_text(c->lv_obj, job->text);
    }}
}}
"""

LABEL_SETTER = """
void {fn_name}(const char *text)
{{
    {job_struct} job;
    job.child_index = {child_index};
    snprintf(job.text, UI_MAX_STRING_LENGTH, "%s", text);

    ui_worker_process_job({cb_name}, &job, sizeof(job));
}}
"""

LABEL_INIT = """
    c->lv_obj = lv_label_create({screen_var}.lv_screen);
    lv_obj_set_pos(c->lv_obj, c->x, c->y);
    lv_obj_set_width(c->lv_obj, c->w);
    lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
"""
