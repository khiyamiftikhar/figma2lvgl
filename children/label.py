# children/label.py

from children.base import ChildEmitter
from templates.label_templates import (
    LABEL_JOB_STRUCT,
    LABEL_JOB_CALLBACK,
    LABEL_SETTER,
    LABEL_INIT,
)

from utils.utils import to_snake_case


class LabelEmitter(ChildEmitter):
    """
    Emits code for UI_CHILD_LABEL
    """
    type_name = "UI_CHILD_LABEL"

    # -------------------------------------------------
    # ui_child_t initializer
    # -------------------------------------------------

    def emit_child_initializer(self, child, index):
        return f"""        {{
            .type = UI_CHILD_LABEL,
            .id = "{child.id}",
            .lv_obj = NULL,
            .x = {child.x}, .y = {child.y},
            .w = {child.w}, .h = {child.h},
            .icon = NULL,
            .current_state = 0,
            .text = "",
            .initial_value = 0
        }}"""

    # -------------------------------------------------
    # Job struct (one per screen + type)
    # -------------------------------------------------

    def emit_job_struct_def(self, screen):
        screen_snake = to_snake_case(screen.name)
        job_struct = f"ui_{screen_snake}_label_job_t"

        return LABEL_JOB_STRUCT.format(
            job_struct=job_struct
        )

    # -------------------------------------------------
    # Job callback
    # -------------------------------------------------

    def emit_job_callback(self, child, screen):
        screen_snake = to_snake_case(screen.name)

        cb_name = f"ui_{screen_snake}_set_{child.id}_job"
        job_struct = f"ui_{screen_snake}_label_job_t"
        screen_var = screen_snake

        return LABEL_JOB_CALLBACK.format(
            cb_name=cb_name,
            job_struct=job_struct,
            screen_var=screen_var
        )

    # -------------------------------------------------
    # Setter prototype (header)
    # -------------------------------------------------

    def emit_setter_prototype(self, child, screen):
        screen_snake = to_snake_case(screen.name)
        return f"void ui_{screen_snake}_set_{child.id}(const char *text);"

    # -------------------------------------------------
    # Setter implementation
    # -------------------------------------------------

    def emit_setter(self, child, screen):
        screen_snake = to_snake_case(screen.name)

        fn_name = f"ui_{screen_snake}_set_{child.id}"
        cb_name = f"ui_{screen_snake}_set_{child.id}_job"
        job_struct = f"ui_{screen_snake}_label_job_t"

        return LABEL_SETTER.format(
            fn_name=fn_name,
            cb_name=cb_name,
            job_struct=job_struct,
            child_index="{child_index}"  # resolved by generator via format
        ).replace(
            "{child_index}", str(self._resolve_child_index(child, screen))
        )

    # -------------------------------------------------
    # LVGL object creation (init function)
    # -------------------------------------------------

    def emit_lvgl_create(self, child, screen):
        screen_snake = to_snake_case(screen.name)

        return LABEL_INIT.format(
            screen_var=screen_snake
        )

    # -------------------------------------------------
    # Helper
    # -------------------------------------------------

    def _resolve_child_index(self, child, screen):
        """
        Generator determines index order, but emitters
        need the same index. This helper ensures consistency.
        """
        for idx, c in enumerate(screen.children):
            if c is child:
                return idx
        return 0


    def emit_init_case(self, screen):
        screen_var = to_snake_case(screen.name)

        return f"""
            case UI_CHILD_LABEL:
                c->lv_obj = lv_label_create({screen_var}.lv_screen);
                lv_obj_set_pos(c->lv_obj, c->x, c->y);
                lv_obj_set_width(c->lv_obj, c->w);
                lv_label_set_long_mode(c->lv_obj, LV_LABEL_LONG_CLIP);
                break;
                """



