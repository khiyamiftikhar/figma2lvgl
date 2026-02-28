# core/generic_child.py

class ChildSpec:
    """
    Describes how a child type is rendered.
    No job struct generation anymore.
    """

    def __init__(
        self,
        *,
        type_name,
        callback_template,
        setter_template,
        init_template,
        setter_args,
    ):
        self.type_name = type_name
        self.callback_template = callback_template
        self.setter_template = setter_template
        self.init_template = init_template
        self.setter_args = setter_args