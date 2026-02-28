# templates/label_templates.py


#LABEL_JOB_STRUCT = """
#typedef struct {{
#    uint8_t child_index;
#    char text[UI_MAX_STRING_LENGTH];
#}} {job_struct};
#"""

LABEL_JOB_CALLBACK = """
static void ui_{screen_var}_label_job_cb(ui_job_t *job)
{{
    uint8_t child_index = job->child_index;
    ui_label_job_t *lbl = &job->data.label;
    

    //printf("\n Updating label at index %d with text: %s", child_index, lbl->text);
    ui_child_t *c = &{screen_var}.children[child_index];

    

    if (c->lv_obj) {{
        lv_label_set_text(c->lv_obj, lbl->text);
    }}
}}


"""

LABEL_SETTER = """
void {fn_name}(const char *text)
{{
    ui_job_t job = {{0}};
    job.cb = ui_{screen_var}_label_job_cb;
    job.child_index = {child_index};
    job.type = UI_JOB_SET_LABEL;

    snprintf(job.data.label.text, UI_MAX_STRING_LENGTH, "%s", text);

    ui_worker_post_job(&job);
}}
"""

LABEL_INIT = """
    case UI_CHILD_LABEL:
        c->lv_obj = lv_label_create({screen_var}.lv_screen);
        lv_obj_set_pos(c->lv_obj, c->x, c->y);
        lv_obj_set_width(c->lv_obj, c->w);
        lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
        break;
"""
