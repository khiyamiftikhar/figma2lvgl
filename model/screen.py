# model/screen.py

from model.child import Child

class Screen:
    """
    Pure semantic representation of a screen.
    """

    def __init__(self, name):
        self.name = name
        self.children = []     # list[Child]

    def add_child(self, child: Child):
        self.children.append(child)

    def __repr__(self):
        return f"Screen(name={self.name}, children={len(self.children)})"
