# templates/label_templates.py


#LABEL_JOB_STRUCT = """
#typedef struct {
#    uint8_t child_index;
#    char text[UI_MAX_STRING_LENGTH];
#} {job_struct};
#"""

# REMOVED: LABEL_JOB_CALLBACK — no longer needed, logic moves inline to setter

LABEL_SETTER = """
void ${fn_name}(const char *text)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (c->lv_obj) {
        lv_label_set_text(c->lv_obj, text);
    }
}
"""

LABEL_INIT = """
    case UI_CHILD_LABEL:
        c->lv_obj = lv_label_create(${screen_var}.lv_screen);
        lv_obj_set_pos(c->lv_obj, c->x, c->y);
        lv_obj_set_width(c->lv_obj, c->w);
        lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
        break;
"""