# templates/bar_templates.py

#BAR_JOB_STRUCT = """
#typedef struct {{
#    uint8_t child_index;
#    int value;
#}} {job_struct};
#"""

BAR_JOB_CALLBACK = """
static void {cb_name}_exec_cb(void *obj, int32_t v)
{{
    lv_bar_set_value(obj, v, LV_ANIM_OFF);
    //printf("\nCallback called with value %d", v);
}}

static void {cb_name}(ui_job_t *job)
{{
    uint8_t idx = job->child_index;
    ui_child_t *c = &${screen_var}.children[idx];

    if (!c->lv_obj || c->type != UI_CHILD_BAR)
        return;

    ui_bar_job_t *bar_job = &job->data.bar;

    if (bar_job->duration_ms == 0)
    {{
        lv_bar_set_value(c->lv_obj,
                         bar_job->value,
                         LV_ANIM_OFF);
        return;
    }}

    lv_anim_t a;
    lv_anim_init(&a);

    lv_anim_set_var(&a, c->lv_obj);
    lv_anim_set_exec_cb(&a, ${cb_name}_exec_cb);

    lv_anim_set_values(&a,
                       lv_bar_get_value(c->lv_obj),
                       bar_job->value);

    lv_anim_set_time(&a, bar_job->duration_ms);
    //printf("\nCallback bar with value %d and duration %d", bar_job->value, bar_job->duration_ms);

    lv_anim_start(&a);
}}
"""

BAR_SETTER = """
void ${fn_name}(int value, uint32_t duration_ms)
{{
    ui_job_t job = {0};
    job.child_index = ${child_index};
    job.type = UI_JOB_SET_BAR;

    job.data.bar.value = value;
    job.data.bar.duration_ms = duration_ms;

    ui_worker_post_job(&job);
}}
"""

BAR_INIT = """
    case UI_CHILD_BAR:
        c->lv_obj = lv_bar_create(${screen_var}.lv_screen);
        lv_obj_set_pos(c->lv_obj, c->x, c->y);
        lv_obj_set_size(c->lv_obj, c->w, c->h);

        lv_bar_set_range(c->lv_obj, 0, 100);
        lv_bar_set_value(c->lv_obj, c->data.bar.value, LV_ANIM_OFF);

        break;
"""