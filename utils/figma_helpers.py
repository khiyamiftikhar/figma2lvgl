

def map_tag_to_child_type(tag, name_attr):
    if tag == "Text":
        return "UI_CHILD_LABEL"
    if tag.lower().startswith("icon_") or name_attr.lower().startswith("icon_"):
        return "UI_CHILD_ICON"
    # Potential extension: map "Bar" or other tags:
    if tag.lower() == "bar":
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
