

def map_tag_to_child_type(tag, name_attr):
    """
    Determine UI child type purely from name semantics.
    Name tokens are '_' delimited; position does not matter.
    """

    if not name_attr:
        return None

    tokens = name_attr.lower().split("_")

    if "label" in tokens:
        return "UI_CHILD_LABEL"

    if "icon" in tokens:
        return "UI_CHILD_ICON"

    if "bar" in tokens:
        return "UI_CHILD_BAR"

    return None




def int_attr(node, key):
    v = node.attrib.get(key)
    if v is None or v == "":
        return 0
    try:
        return int(float(v))
    except:
        return 0
