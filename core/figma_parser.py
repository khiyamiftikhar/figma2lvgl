# figma_parser.py

from core.utils.utils import normalize_id
from core.utils.utils import to_snake_case
from core.utils.figma_helpers import map_tag_to_child_type, int_attr


class ParsedChild:
    def __init__(self, type, id, x, y, w, h):
        self.type = type              # UI_CHILD_LABEL / ICON / BAR
        self.id = id                  # normalized id
        self.x = x
        self.y = y
        self.w = w
        self.h = h

class ParsedScreen:
    def __init__(self, name):
        self.name = name
        self.snake = to_snake_case(name)
        self.children = []


def parse_screen(frame_node):
    frame_name = frame_node.attrib.get("name", "unnamed")
    screen = ParsedScreen(frame_name)

    children_parent = frame_node.find("children")
    if children_parent is None:
        return screen

    for child in list(children_parent):

        mapped = map_tag_to_child_type(child)
        if mapped is None:
            continue

        x = int_attr(child, "x")
        y = int_attr(child, "y")
        w = int_attr(child, "width")
        h = int_attr(child, "height")

        raw_id = child.attrib.get("name", f"child_{len(screen.children)}")
        child_id = normalize_id(raw_id)

        existing_ids = {c.id for c in screen.children}
        if child_id in existing_ids:
            raise ValueError(
                f"Duplicate child id '{child_id}' in screen '{frame_name}'"
            )

        screen.children.append(
            ParsedChild(
                type=mapped,
                id=child_id,
                x=x,
                y=y,
                w=w,
                h=h
            )
        )
    return screen
