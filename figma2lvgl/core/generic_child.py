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
        setter_name_pattern="ui_{screen}_set_{child_id}",
        callback_name_pattern="",
    ):
        self.type_name             = type_name
        self.callback_template     = callback_template
        self.setter_template       = setter_template
        self.init_template         = init_template
        self.setter_args           = setter_args
        self.requires_asset        = requires_asset
        self.setter_name_pattern   = setter_name_pattern
        self.callback_name_pattern = callback_name_pattern

    def derive_setter_name(self, screen_snake: str, child_id: str) -> str:
        """
        Derive the C setter function name from the pattern.
        Encoding naming conventions here means generator.py needs no
        per-type if/elif branches — adding a new widget type requires
        zero changes to the generator.
        """
        return self.setter_name_pattern.format(
            screen=screen_snake, child_id=child_id
        )

    def derive_callback_name(self, screen_snake: str) -> str:
        """
        Derive the C callback function name from the pattern.
        Returns "" for types with no callback (empty pattern).
        """
        if not self.callback_name_pattern:
            return ""
        return self.callback_name_pattern.format(screen=screen_snake)
