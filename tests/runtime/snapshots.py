"""Pixels for L10: RGB565 frames, PNG references, comparison, difference images.

Design doc 12, section 5. Pixel values are compared, never PNG bytes: the
new frame and the decoded reference are both RGB888, expanded from RGB565 by
one fixed formula. PNG is lossless, so it only makes references viewable.
"""

import struct
from pathlib import Path

import png

SNAPSHOTS = Path(__file__).resolve().parents[1] / "snapshots"


def _expand(v: int, bits: int) -> int:
    m = (1 << bits) - 1
    return (v * 255 + m // 2) // m


def rgb565_to_rows(raw: bytes, w: int, h: int) -> list:
    """Rows of RGB888 bytes (flat r, g, b per pixel) from little-endian RGB565."""
    values = struct.unpack(f"<{w * h}H", raw[: w * h * 2])
    lut_r = [_expand(i, 5) for i in range(32)]
    lut_g = [_expand(i, 6) for i in range(64)]
    rows = []
    for y in range(h):
        row = bytearray(w * 3)
        for x, v in enumerate(values[y * w:(y + 1) * w]):
            row[3 * x] = lut_r[v >> 11]
            row[3 * x + 1] = lut_g[(v >> 5) & 63]
            row[3 * x + 2] = lut_r[v & 31]
        rows.append(bytes(row))
    return rows


def write_png(path: Path, rows: list, w: int, h: int) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with open(path, "wb") as f:
        png.Writer(w, h, greyscale=False, alpha=False, bitdepth=8).write(f, rows)


def read_png(path: Path) -> tuple:
    """(rows, w, h), RGB888 rows, whatever the PNG's own layout."""
    w, h, rows, _info = png.Reader(filename=str(path)).asRGB8()
    return [bytes(r) for r in rows], w, h


def changed_pixels(a: list, b: list, w: int) -> int:
    count = 0
    for ra, rb in zip(a, b):
        if ra != rb:
            count += sum(1 for x in range(w) if ra[3 * x:3 * x + 3] != rb[3 * x:3 * x + 3])
    return count


def diff_rows(new: list, ref: list, w: int) -> list:
    """Changed pixels in red over a faded copy of the reference."""
    out = []
    for rn, rr in zip(new, ref):
        row = bytearray(w * 3)
        for x in range(w):
            if rn[3 * x:3 * x + 3] != rr[3 * x:3 * x + 3]:
                row[3 * x:3 * x + 3] = b"\xff\x00\x00"
            else:
                for c in range(3):
                    row[3 * x + c] = 192 + rr[3 * x + c] // 4
        out.append(bytes(row))
    return out


def similarity(a: list, b: list, w: int, tolerance: int = 10) -> float:
    """Share of pixels whose three channels are each within `tolerance`
    (for information only: Figma's PNG vs the render, doc 12, section 7)."""
    close = total = 0
    for ra, rb in zip(a, b):
        for x in range(w):
            total += 1
            if all(abs(ra[3 * x + c] - rb[3 * x + c]) <= tolerance for c in range(3)):
                close += 1
    return close / total if total else 0.0
