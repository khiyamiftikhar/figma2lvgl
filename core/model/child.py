# model/child.py

class Child:
    """
    Pure semantic representation of a UI child.
    No XML, no LVGL, no C.
    """

    def __init__(self, *, type, id, x, y, w, h):
        self.type = type        # "UI_CHILD_LABEL", "UI_CHILD_ICON", ...
        self.id = id            # normalized id (string)
        self.x = x
        self.y = y
        self.w = w
        self.h = h

    def __repr__(self):
        return (
            f"Child(type={self.type}, id={self.id}, "
            f"x={self.x}, y={self.y}, w={self.w}, h={self.h})"
        )
