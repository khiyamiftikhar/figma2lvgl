# children/icon.py

from children.base import ChildEmitter
from templates.icon_templates import (
    ICON_JOB_STRUCT,
    ICON_JOB_CALLBACK,
    ICON_SETTER,
    ICON_INIT,
)

from utils.utils import to_snake_case


class IconEmitter(ChildEmitter):
    """
    Emits code for UI_CHILD_ICON
    """
    type_name = "UI_CHILD_ICON"

    # -------------------------------------------------
    # ui_child_t initializer
    # -------------------------------------------------

    def emit_child_initializer(self, child, index):
        return f"""        {{
            .type = UI_CHILD_ICON,
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
        job_struct = f"ui_{screen_snake}_icon_job_t"

        return ICON_JOB_STRUCT.format(
            job_struct=job_struct
        )

    # -------------------------------------------------
    # Job callback
    # -------------------------------------------------

    def emit_job_callback(self, child, screen):
        screen_snake = to_snake_case(screen.name)

        cb_name = f"ui_{screen_snake}_set_{child.id}_job"
        job_struct = f"ui_{screen_snake}_icon_job_t"
        screen_var = screen_snake

        return ICON_JOB_CALLBACK.format(
            cb_name=cb_name,
            job_struct=job_struct,
            screen_var=screen_var
        )

    # -------------------------------------------------
    # Setter prototype
    # -------------------------------------------------

    def emit_setter_prototype(self, child, screen):
        screen_snake = to_snake_case(screen.name)
        return f"void ui_{screen_snake}_set_{child.id}(uint8_t state);"

    # -------------------------------------------------
    # Setter implementation
    # -------------------------------------------------

    def emit_setter(self, child, screen):
        screen_snake = to_snake_case(screen.name)

        fn_name = f"ui_{screen_snake}_set_{child.id}"
        cb_name = f"ui_{screen_snake}_set_{child.id}_job"
        job_struct = f"ui_{screen_snake}_icon_job_t"

        return ICON_SETTER.format(
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

        return ICON_INIT.format(
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
                case UI_CHILD_ICON:
                    c->lv_obj = lv_img_create({screen_var}.lv_screen);
                    lv_obj_set_pos(c->lv_obj, c->x, c->y);
                    lv_obj_set_size(c->lv_obj, c->w, c->h);
                    lv_obj_set_style_clip_corner(
                        c->lv_obj,
                        true,
                        LV_PART_MAIN | LV_STATE_DEFAULT
                    );
                    lv_image_set_inner_align(c->lv_obj, LV_IMAGE_ALIGN_CENTER);
                    break;
        """
