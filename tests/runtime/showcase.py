"""Showcase metrics (design doc 12, section 7): information only.

- generated share: generated widgets / showcase layers that aren't hidden
  and aren't containers dropped by rule R12. Skipped layers count in the
  denominator; that is the point.
- similarity: share of pixels whose three RGB888 channels are each within
  10 of Figma's PNG, at the frame's size.

How layers are counted: a hidden layer counts with nothing inside it; a
component instance is one layer; a button's first Text child is its label,
counted with the button; a list's or grid's design-time children aren't
counted (they are never generated, by design); a Group or Frame without a
visible fill or stroke and without a panel/list/grid/button name is dropped
by R12 and not counted, but its children are.
"""

import re
import xml.etree.ElementTree as ET
from pathlib import Path

from tests.runtime.harness import _widgets
from tests.runtime.snapshots import read_png, rgb565_to_rows, similarity

FIGMA_PNGS = {
    "showcase": Path(__file__).resolve().parents[1] / "fixtures" / "showcase" / "figma",
    "features": Path(__file__).resolve().parents[1] / "fixtures" / "features" / "figma",
}


def _words(name: str) -> list:
    return [w for w in re.split(r"[^a-z0-9]+", name.lower()) if w]


def _visible_paint(node, tag) -> bool:
    paints = node.find(tag)
    return paints is not None and any(
        p.get("visible", "true") != "false" and p.get("opacity", "1") != "0" for p in paints)


def count_layers(frame) -> int:
    """Showcase layers of one screen frame (see the module docstring)."""
    return len(layer_names(frame))


def layer_names(frame) -> list:
    """Names of the counted layers of one screen frame, in XML order."""
    def walk(node) -> list:
        n = []
        kids = node.find("children")
        kids = list(kids) if kids is not None else []
        for c in kids:
            if c.get("visible", "true") == "false":
                continue
            kind, words = c.get("type"), _words(c.get("name", ""))
            inner = c.find("children")
            if kind == "INSTANCE" or inner is None or len(inner) == 0:
                n.append(c.get("name"))
                continue
            first = words[0] if words else ""
            if first in ("btn", "button"):
                n.append(c.get("name"))
                texts = [k for k in inner if k.get("type") == "TEXT"]
                rest = [k for k in inner if not texts or k is not texts[0]]
                n += walk_list(rest)
            elif first in ("list", "grid"):
                n.append(c.get("name"))
            elif (first == "panel" or _visible_paint(c, "fills") or _visible_paint(c, "strokes")):
                n += [c.get("name")] + walk(c)
            else:
                n += walk(c)        # dropped by R12
        return n

    def walk_list(nodes) -> list:
        holder = ET.Element("x")
        ch = ET.SubElement(holder, "children")
        ch.extend(nodes)
        return walk(holder)
    return walk(frame)


def metrics(result, xml: Path, figma_dir: Path) -> list:
    """[(screen, generated, layers, similarity or None)] for a rendered case."""
    root = ET.parse(xml).getroot()
    frames = {f.get("name"): f for f in root.find("children")
              if f.get("type") == "FRAME"}
    rows = []
    for screen in result.model:
        frame = frames.get(screen.name, frames.get(screen.snake))
        layers = count_layers(frame) if frame is not None else 0
        generated = len(_widgets(screen))
        sim = None
        png_path = figma_dir / f"{screen.snake}.png"
        if screen.snake in result.snapshots and png_path.exists():
            w, h, raw = result.snapshots[screen.snake]
            fig, fw, fh = read_png(png_path)
            if (fw, fh) == (w, h):
                sim = similarity(rgb565_to_rows(raw.read_bytes(), w, h), fig, w)
        rows.append((screen.snake, generated, layers, sim))
    return rows


def report(case: str, rows: list) -> str:
    lines = [f"{case}: generated share, similarity to Figma's PNG (information only)"]
    for screen, gen, layers, sim in rows:
        share = f"{gen}/{layers} = {gen / layers:.0%}" if layers else "-"
        lines.append(f"  {screen:18s} generated {share:14s} similarity "
                     + (f"{sim:.1%}" if sim is not None else "-"))
    return "\n".join(lines)
