# templates/bar_templates.py

#BAR_JOB_STRUCT = """
#typedef struct {
#    uint8_t child_index;
#    int value;
#} {job_struct};
#"""

# REMOVED: BAR_JOB_CALLBACK outer shell — logic moves inline to setter
# KEPT: _exec_cb as a static LVGL animation helper, renamed to be standalone

BAR_CALLBACK = """
static void ${cb_name}_exec_cb(void *obj, int32_t v)
{
    lv_bar_set_value(obj, v, LV_ANIM_OFF);
}
"""

BAR_SETTER = """
void ${fn_name}(int value, uint32_t duration_ms)
{
    ui_child_t *c = &${screen_var}.children[${child_index}];
    if (!c->lv_obj || c->type != UI_CHILD_BAR)
        return;
    if (duration_ms == 0)
    {
        lv_bar_set_value(c->lv_obj, value, LV_ANIM_OFF);
        return;
    }
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, c->lv_obj);
    lv_anim_set_exec_cb(&a, ${cb_name}_exec_cb);
    lv_anim_set_values(&a, lv_bar_get_value(c->lv_obj), value);
    lv_anim_set_time(&a, duration_ms);
    lv_anim_start(&a);
}
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