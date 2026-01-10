from utils.template_loader import load_template
from utils.utils import to_snake_case

class ChildSpec:
    def __init__(
        self,
        *,
        type_name,
        job_template,
        callback_template,
        setter_template,
        init_template,
        setter_args,
    ):
        self.type_name = type_name
        self.job_template = job_template
        self.callback_template = callback_template
        self.setter_template = setter_template
        self.init_template = init_template
        self.setter_args = setter_args

    
    def emit_job_struct(self, screen):
        screen_snake = to_snake_case(screen.name)
        return load_template(self.job_template).format(
            job_struct=f"ui_{screen_snake}_{self.type_name.lower()}_job_t"
        )


    def emit_job_callback(self, screen, child, index):
        screen_snake = to_snake_case(screen.name)

        return load_template(self.callback_template).format(
            screen_var=screen_snake,
            child_id=child.id,
            job_struct=f"ui_{screen_snake}_{self.type_name.lower()}_job_t",
            child_index=index,
        )




    def emit_setter(self, screen, child, index):
        screen_snake = to_snake_case(screen.name)
        job_struct = f"ui_{screen_snake}_{self.type_name.lower()}_job_t"

        return load_template(self.setter_template).format(
            fn_name=f"ui_{screen_snake}_set_{child.id}",
            cb_name=f"ui_{screen_snake}_set_{child.id}_job",
            job_struct=job_struct,
            child_index=index,
        )


    def emit_init_case(self, screen):
        screen_snake = to_snake_case(screen.name)
        return load_template(self.init_template).format(
            screen_var=screen_snake
        )

    def emit_setter_prototype(self, screen, child):
        screen_snake = to_snake_case(screen.name)
        return f"void ui_{screen_snake}_set_{child.id}({self.setter_args});"
