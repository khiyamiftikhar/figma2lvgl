"""Reading the exported XML without losing line breaks in text.

The FigML exporter writes a text layer's line breaks as raw newline
characters inside the `characters` attribute. An XML parser must turn those
into spaces (attribute-value normalization, XML 1.0 section 3.3.3), so
"First line\nSecond line" would arrive as "First line Second line". Before
parsing, newlines, carriage returns and tabs inside attribute values are
written as character references (&#10;, &#13;, &#9;), which XML keeps.
Text outside attribute values, comments, CDATA sections and processing
instructions are left as they are.
"""

import xml.etree.ElementTree as ET
from pathlib import Path

_REFS = {"\n": "&#10;", "\r": "&#13;", "\t": "&#9;"}


def protect_attribute_whitespace(text: str) -> str:
    out, i, n = [], 0, len(text)
    while i < n:
        if text.startswith("<!--", i):
            end = text.find("-->", i)
            end = n if end < 0 else end + 3
        elif text.startswith("<![CDATA[", i):
            end = text.find("]]>", i)
            end = n if end < 0 else end + 3
        elif text.startswith("<?", i) or text.startswith("<!", i):
            end = text.find(">", i)
            end = n if end < 0 else end + 1
        elif text[i] == "<":
            # A tag: quoted values are attribute values.
            j, quote, tag = i, None, []
            while j < n:
                c = text[j]
                if quote:
                    if c == quote:
                        quote = None
                        tag.append(c)
                    else:
                        tag.append(_REFS.get(c, c))
                elif c in "\"'":
                    quote = c
                    tag.append(c)
                elif c == ">":
                    tag.append(c)
                    j += 1
                    break
                else:
                    tag.append(c)
                j += 1
            out.append("".join(tag))
            i = j
            continue
        else:
            end = text.find("<", i)
            end = n if end < 0 else end
        out.append(text[i:end])
        i = end
    return "".join(out)


def read_design(path) -> ET.Element:
    """The root element of an exported design, line breaks in text kept."""
    raw = Path(path).read_bytes()
    try:
        text = raw.decode("utf-8-sig")     # what the exporter writes
    except UnicodeDecodeError:
        return ET.parse(path).getroot()    # another encoding: parse as is
    return ET.fromstring(protect_attribute_whitespace(text))
