# children/bar.py

from children.base import ChildEmitter
from templates.bar_templates import (
    BAR_JOB_STRUCT,
    BAR_JOB_CALLBACK,
    BAR_SETTER,
    BAR_INIT,
)

from utils.utils import to_snake_case


class BarEmitter(ChildEmitter):
    """
    Emits code for UI_CHILD_BAR
    """
    type_name = "UI_CHILD_BAR"

    # -------------------------------------------------
    # ui_child_t initializer
    # -------------------------------------------------

    def emit_child_initializer(self, child, index):
        return f"""        {{
            .type = UI_CHILD_BAR,
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
    # Job struct
    # -------------------------------------------------

    def emit_job_struct_def(self, screen):
        screen_snake = to_snake_case(screen.name)
        job_struct = f"ui_{screen_snake}_bar_job_t"

        return BAR_JOB_STRUCT.format(
            job_struct=job_struct
        )

    # -------------------------------------------------
    # Job callback
    # -------------------------------------------------

    def emit_job_callback(self, child, screen):
        screen_snake = to_snake_case(screen.name)

        cb_name = f"ui_{screen_snake}_set_{child.id}_job"
        job_struct = f"ui_{screen_snake}_bar_job_t"
        screen_var = screen_snake

        return BAR_JOB_CALLBACK.format(
            cb_name=cb_name,
            job_struct=job_struct,
            screen_var=screen_var
        )

    # -------------------------------------------------
    # Setter prototype
    # -------------------------------------------------

    def emit_setter_prototype(self, child, screen):
        screen_snake = to_snake_case(screen.name)
        return f"void ui_{screen_snake}_set_{child.id}(int value);"

    # -------------------------------------------------
    # Setter implementation
    # -------------------------------------------------

    def emit_setter(self, child, screen):
        screen_snake = to_snake_case(screen.name)

        fn_name = f"ui_{screen_snake}_set_{child.id}"
        cb_name = f"ui_{screen_snake}_set_{child.id}_job"
        job_struct = f"ui_{screen_snake}_bar_job_t"

        return BAR_SETTER.format(
            fn_name=fn_name,
            cb_name=cb_name,
            job_struct=job_struct,
            child_index="{child_index}"
        ).replace(
            "{child_index}", str(self._resolve_child_index(child, screen))
        )

    # -------------------------------------------------
    # Init switch case
    # -------------------------------------------------

    def emit_init_case(self, screen):
        screen_var = to_snake_case(screen.name)

        return BAR_INIT.format(
            screen_var=screen_var
        )

    # -------------------------------------------------
    # Helper
    # -------------------------------------------------

    def _resolve_child_index(self, child, screen):
        for idx, c in enumerate(screen.children):
            if c is child:
                return idx
        return 0
        
    def emit_init_case(self, screen):
        screen_var = to_snake_case(screen.name)

        return f"""
                case UI_CHILD_BAR:
                    c->lv_obj = lv_bar_create({screen_var}.lv_screen);
                    lv_obj_set_pos(c->lv_obj, c->x, c->y);
                    lv_obj_set_size(c->lv_obj, c->w, c->h);
                    lv_bar_set_value(
                        c->lv_obj,
                        c->initial_value,
                        LV_ANIM_OFF
                    );
                    break;
        """

