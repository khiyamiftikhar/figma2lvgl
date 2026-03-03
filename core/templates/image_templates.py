# templates/icon_templates.py

#ICON_JOB_STRUCT = """
#typedef struct {
#    uint8_t child_index;
#    uint8_t state;
#{ {job_struct};
#"""

IMAGE_JOB_CALLBACK = """
static void ${cb_name}(ui_job_t *job)
{
    

    ui_child_t *c = &${screen_var}.children[job->child_index];
    if (c->type != UI_CHILD_IMAGE || c->lv_obj == NULL)
        return;

    // Store source inside child
    c->data.image.src = job->data.image.src;

    // Apply to LVGL object
    lv_image_set_src(c->lv_obj, job->data.image.src);
}
"""

IMAGE_SETTER = """
void ${fn_name}(void)
{
    ui_job_t job = {0};
    job.child_index = ${child_index};
    job.type = UI_JOB_SET_IMAGE;
    job.cb = ui_${screen_var}_display_image_job_cb;
    job.data.image.src = &${child_id};
    ui_worker_post_job(&job);
}
"""

IMAGE_INIT = """
    case UI_CHILD_IMAGE:
        c->lv_obj = lv_image_create(${screen_var}.lv_screen);
        lv_obj_set_pos(c->lv_obj, c->x, c->y);

        if(c->data.image.src)
            lv_image_set_src(c->lv_obj, c->data.image.src);

        break;
"""