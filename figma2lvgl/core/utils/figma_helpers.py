

def map_tag_to_child_type(node):

    name = node.attrib.get("name", "").lower()

    if node.tag == "Text":
        return "UI_CHILD_LABEL"

    if "bar" in name:
        return "UI_CHILD_BAR"

    if "icon" in name or "image" in name:
        return "UI_CHILD_IMAGE"

    return "UI_CHILD_LABEL"



def int_attr(node, key):
    v = node.attrib.get(key)
    if v is None or v == "":
        return 0
    try:
        return int(float(v))
    except:
        return 0
