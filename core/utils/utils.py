import sys
import os
import re
import xml.etree.ElementTree as ET

# ---------------------------
# Configuration / constants
# ---------------------------
UI_MAX_STRING_LENGTH = 64
UI_MAX_CHILDREN = 64  # if you want, adjust this
INDENT = "    "

# Mapping simple tags -> child types
# "Text" -> label
# tags starting with "icon_" -> icon
# If you want to add more mappings, change here.

# ---------------------------
# Helpers
# ---------------------------


def normalize_id(name):
    return name.lower().replace("-", "_").replace(" ", "_")

def to_snake_case(s):
    # normalize whitespace and punctuation, then to snake_case
    s = s.strip()
    # replace non-alnum with underscore
    s = re.sub(r'[^0-9a-zA-Z]+', '_', s)
    # collapse multiple underscores
    s = re.sub(r'__+', '_', s)
    return s.lower().strip('_')

def base_name_for_header(frame_name_snake):
    # If frame name ends with "_screen", remove it for base
    if frame_name_snake.endswith("_screen"):
        return frame_name_snake[:-7]
    return frame_name_snake

def sanitize_c_string(s, maxlen=UI_MAX_STRING_LENGTH):
    if s is None:
        return ""
    s = s.replace('"', '\\"')
    if len(s) >= maxlen:
        return s[:maxlen-1]
    return s

def int_attr(node, key):
    v = node.attrib.get(key)
    if v is None or v == "":
        return 0
    try:
        return int(float(v))
    except:
        return 0

def write_file(path, text):
    with open(path, 'w', encoding='utf-8') as f:
        f.write(text)
