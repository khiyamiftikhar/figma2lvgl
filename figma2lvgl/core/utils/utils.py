import sys
import os
import re
import xml.etree.ElementTree as ET

# ---------------------------
# Configuration / constants
# ---------------------------
UI_MAX_STRING_LENGTH = 30   # must match UI_MAX_STRING_LENGTH in ui_defs.h / ui_config.h
INDENT = "    "

# Mapping simple tags -> child types
# "Text" -> label
# tags starting with "icon_" -> icon
# If you want to add more mappings, change here.

# ---------------------------
# Helpers
# ---------------------------


def normalize_id(name):
    """
    Convert a Figma node name to a valid C identifier in snake_case.
    Handles: camelCase, PascalCase, spaces, hyphens, and non-alnum chars.

    Examples:
        "BatteryBar"   → "battery_bar"
        "Progress Bar" → "progress_bar"
        "icon-wifi"    → "icon_wifi"
        "Time (Label)" → "time_label"
        "HTTPStatus"   → "http_status"
    """
    # Insert underscores at camelCase / PascalCase boundaries
    name = re.sub(r'(?<=[a-z])(?=[A-Z])', '_', name)        # camelCase
    name = re.sub(r'(?<=[A-Z])(?=[A-Z][a-z])', '_', name)   # HTTPStatus → HTTP_Status
    name = name.lower()
    name = re.sub(r'[-\s]+', '_', name)                      # spaces and hyphens
    name = re.sub(r'[^a-z0-9_]', '_', name)                  # remaining non-alnum
    name = re.sub(r'_+', '_', name)                           # collapse multiples
    return name.strip('_')

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
    """
    Escape a Python string for safe embedding inside a C string literal.
    Handles quotes, backslashes, and control characters that would produce
    invalid C (e.g. a literal newline from FigML's characters attribute).
    Truncates to maxlen-1 to leave room for the null terminator.
    """
    if s is None:
        return ""
    s = s.replace('\\', '\\\\')   # must be first
    s = s.replace('"',  '\\"')
    s = s.replace('\n', '\\n')
    s = s.replace('\r', '\\r')
    s = s.replace('\t', '\\t')
    # FigML sometimes uses Unicode line/paragraph separators instead of \n
    s = s.replace('\u2028', '\\n')   # LINE SEPARATOR → LVGL newline
    s = s.replace('\u2029', '\\n')   # PARAGRAPH SEPARATOR → LVGL newline
    return s[:maxlen - 1]

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
