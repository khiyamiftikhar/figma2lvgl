"""Line breaks in exported text survive parsing (N-finding, PR 4 fixtures)."""

from figma2lvgl.core.figma_parser import parse_style
from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.xml_input import protect_attribute_whitespace, read_design


def test_newlines_in_attributes_are_kept(tmp_path):
    xml = tmp_path / "d.xml"
    # Bytes, so the line endings are exactly these on every OS; CRLF reads
    # as LF, as XML itself does.
    xml.write_bytes(b'<?xml version="1.0" encoding="UTF-8"?>\r\n<page>\r\n'
                    b'  <Text name="t" characters="First\nSecond\r\n\tThird"/>\n</page>\n')
    root = read_design(xml)
    assert root.find("Text").get("characters") == "First\nSecond\n\tThird"


def test_only_attribute_values_change():
    text = ('<!-- a "quote\n" in a comment -->\n<a x="1\n2" y=\'3\n4\'>body\n'
            '<![CDATA[ "x\n" ]]></a>')
    assert protect_attribute_whitespace(text) == (
        '<!-- a "quote\n" in a comment -->\n<a x="1&#10;2" y=\'3&#10;4\'>body\n'
        '<![CDATA[ "x\n" ]]></a>')


def _text(attrs):
    from xml.etree.ElementTree import fromstring
    return fromstring(f'<Text type="TEXT" fontSize="14" {attrs}/>')


def test_alignment_and_wrapping_are_read():
    s = parse_style(_text('textAlignHorizontal="CENTER" textAutoResize="HEIGHT"'),
                    WidgetType.LABEL).text
    assert s.align == "CENTER" and s.wrap
    s = parse_style(_text('textAutoResize="WIDTH_AND_HEIGHT"'), WidgetType.LABEL).text
    assert s.align is None and not s.wrap
    assert not parse_style(_text(""), WidgetType.LABEL).text.wrap
