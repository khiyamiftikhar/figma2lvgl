# core/generic_child.py

class ChildSpec:
    def __init__(
        self,
        *,
        type_name,
        callback_template,
        setter_template,
        init_template,
        setter_args,
        requires_asset=False,
    ):
        self.type_name = type_name
        self.callback_template = callback_template
        self.setter_template = setter_template
        self.init_template = init_template
        self.setter_args = setter_args
        self.requires_asset = requires_asset