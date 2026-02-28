# core/context.py

class GenerationContext:
    def __init__(self):
        self.struct_entries = []
        self.callback_blocks = []
        self.setter_blocks = []
        self.init_blocks = []

    def add_struct(self, block):
        self.struct_entries.append(block)

    def add_callback(self, block):
        self.callback_blocks.append(block)

    def add_setter(self, block):
        self.setter_blocks.append(block)

    def add_init(self, block):
        self.init_blocks.append(block)

    def render_struct_entries(self):
        return "\n".join(self.struct_entries)

    def render_callbacks(self):
        return "\n".join(self.callback_blocks)

    def render_setters(self):
        return "\n".join(self.setter_blocks)

    def render_inits(self):
        return "\n".join(self.init_blocks)