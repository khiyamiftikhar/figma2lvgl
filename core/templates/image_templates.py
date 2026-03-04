# templates/icon_templates.py

#ICON_JOB_STRUCT = """
#typedef struct {
#    uint8_t child_index;
#    uint8_t state;
#{ {job_struct};
#"""

# REMOVED: IMAGE_JOB_CALLBACK — logic moves inline to setter

IMAGE_SETTER = """
void ${fn_name}(void)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (c->type != UI_CHILD_IMAGE || c->lv_obj == NULL)
        return;
    c->data.image.src = &${child_id};
    lv_image_set_src(c->lv_obj, c->data.image.src);
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