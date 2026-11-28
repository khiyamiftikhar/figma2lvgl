"""Run a design through the real pipeline in memory and check doc 10's
invariants. Used by tests/test_property_naming.py and
tests/compile/test_property_compile.py.
"""

import logging
import re
import shutil
import tempfile
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from pathlib import Path

from figma2lvgl.core import naming
from figma2lvgl.core.widget_type import WidgetType
from figma2lvgl.core.config_writer import write_ui_config
from figma2lvgl.core.figma_parser import parse_design
from figma2lvgl.core.generator import generate_screen
from figma2lvgl.main import STATIC_SRC, check_names

PARSER_LOGGER = "figma2lvgl.core.figma_parser"


@dataclass
class Run:
    screens: list = None
    files: dict = field(default_factory=dict)   # relative path -> text
    log: str = ""
    error: naming.NamingError = None


class _Collect(logging.Handler):
    def __init__(self):
        super().__init__(logging.DEBUG)
        self.lines = []

    def emit(self, record):
        self.lines.append(record.getMessage())


def run_design(page) -> Run:
    """Parse and generate every screen; never writes to disk."""
    handler = _Collect()
    log = logging.getLogger(PARSER_LOGGER)
    old_level = log.level
    log.addHandler(handler)
    log.setLevel(logging.DEBUG)
    try:
        frames = page.find("children").findall("Frame")
        try:
            screens = parse_design(frames)
        except naming.NamingError as e:
            return Run(log="\n".join(handler.lines), error=e)
        files = {}
        for s in screens:
            c_name, h_name, h_text, c_text = generate_screen(s)
            files[f"include/{h_name}"] = h_text
            files[f"src/{c_name}"] = c_text
        return Run(screens, files, "\n".join(handler.lines))
    finally:
        log.removeHandler(handler)
        log.setLevel(old_level)


def _nodes(nodes):
    for n in nodes:
        yield n
        yield from _nodes(n.children)


def check_invariants(run: Run, page) -> None:
    """C1-C5, C8 on a successful run; a NamingError must name a layer."""
    if run.error is not None:
        d = run.error.diagnostic
        assert d.layers, d
        assert d.code in {"screen_collision", "range_out_of_bounds", "asset_conflict"}, d
        return

    screens = run.screens
    # C4: screen names unique and valid; their files avoid reserved stems.
    snakes = [s.snake for s in screens]
    assert len(snakes) == len(set(snakes)), snakes
    for s in snakes:
        assert naming.is_valid_identifier(s) and not s.startswith("lv_"), s
        assert f"ui_{s}" not in naming.RESERVED_FILE_STEMS, s

    symbols = []
    for screen in screens:
        symbols += screen.symbols.values()

        def check_scope(nodes):
            ids = [n.id for n in nodes]
            assert len(ids) == len(set(ids)), ids                      # C2
            for n in nodes:
                assert naming.is_valid_identifier(n.id), n.id          # C1
                # C2 (doc 10 rev 6): no reserved name, except label_text,
                # which the type prefix gives a Text named "text"; it is a
                # field only of buttons, and buttons hold no members.
                assert n.id not in naming.RESERVED_FIELDS - {"label_text"}, n.id
                if n.id == "label_text":
                    assert n.widget_type == WidgetType.LABEL, n.id
                if n.widget_type == WidgetType.BUTTON:                  # why that's safe
                    assert not n.children, n.id
                if n.id.startswith("lv_"):
                    raise AssertionError(n.id)                         # C5
                if n.id != n.natural_id:                                # C8
                    assert f"'{n.layer}'" in run.log, (n.layer, n.id)
                if n.asset_name is not None:
                    assert naming.is_valid_identifier(n.asset_name), n.asset_name
                    assert n.asset_name not in naming.RESERVED_FILE_STEMS
                check_scope(n.children)

        check_scope(screen.children)
        for n in _nodes(screen.children):
            symbols += n.symbols.values()

    # C1, C3, C5 on every external symbol.
    for name in symbols:
        assert naming.is_valid_identifier(name), name
        assert not name.startswith("lv_"), name
    assert len(symbols) == len(set(symbols)), sorted(symbols)

    # The promised form (4.6): suffixes and qualification only when needed.
    for screen in screens:
        check_symbol_forms(screen)

    # Names in the generated text itself, not just in the parsed model.
    for path, text in run.files.items():
        if path.endswith(".h"):
            for fn in re.findall(r"\b(ui_\w+)\(", text):
                assert naming.is_valid_identifier(fn), fn
        else:
            for member in re.findall(r"^\s*\}\s*(\w+);", text, re.M):
                assert naming.is_valid_identifier(member), member

    # Section 5: symbols, macros, files (no images in these designs).
    with tempfile.TemporaryDirectory() as empty:
        assert check_names(screens, Path(empty)) == []


def _with_paths(nodes, prefix=()):
    for n in nodes:
        path = prefix + (n.id,)
        yield n, path
        yield from _with_paths(n.children, path)


def _strip_suffix(name, base):
    """True if name is base or base plus an allocator suffix _2, _3, ..."""
    return name == base or re.fullmatch(re.escape(base) + r"_[2-9]\d*", name) is not None


def check_symbol_forms(screen) -> None:
    """Setters use the full path; callbacks and accessors use the member
    name, qualified only when it would clash; a suffix only for real
    clashes of the unsuffixed form (doc 10, 4.5-4.6)."""
    s = screen.snake
    widgets = list(_with_paths(screen.children))

    # Setters: exactly the full path, or suffixed when two full paths
    # flatten alike.
    canon_count = {}
    for n, path in widgets:
        for key in n.symbols:
            if isinstance(key, str) and key != "get":
                canon = f"ui_{s}_{'_'.join(path)}_{key}"
                canon_count[canon] = canon_count.get(canon, 0) + 1
    for n, path in widgets:
        for key, name in n.symbols.items():
            if isinstance(key, str) and key != "get":
                canon = f"ui_{s}_{'_'.join(path)}_{key}"
                assert name == canon or (_strip_suffix(name, canon) and canon_count[canon] > 1), \
                    (name, canon)

    # Callbacks and accessors: form(q) uses the last q path parts.
    def form(key, path, q):
        member = "_".join(path[-q:])
        if key == "get":
            return f"ui_{s}_get_{member}"
        ev = key[1]
        return f"ui_{s}_on_{member}" + (f"_{ev}" if ev else "")

    all_forms = {}
    for n, path in widgets:
        for key in n.symbols:
            if key == "get" or isinstance(key, tuple):
                for q in range(1, len(path) + 1):
                    all_forms.setdefault(form(key, path, q), []).append(n)
    for n, path in widgets:
        for key, name in n.symbols.items():
            if not (key == "get" or isinstance(key, tuple)):
                continue
            qs = [q for q in range(1, len(path) + 1) if _strip_suffix(name, form(key, path, q))]
            assert qs, (name, path)
            q = qs[0]
            if q > 1:
                # The shorter form must clash with another widget's name.
                shorter = form(key, path, q - 1)
                assert any(o is not n for o in all_forms.get(shorter, [])), (name, shorter)
            if name != form(key, path, q):
                # A suffix only after qualification is exhausted (the full
                # path), and only if another widget wants the same name.
                assert q == len(path), (name, path)
                assert any(o is not n for o in all_forms.get(form(key, path, q), [])), name


def write_tree(run: Run, root: Path) -> Path:
    """Write the generated files plus what they include, for compiling.
    Images get declarations only (assets.h), which -fsyntax-only needs."""
    ui = root / "ui_src"
    for rel, text in run.files.items():
        p = ui / rel
        p.parent.mkdir(parents=True, exist_ok=True)
        p.write_text(text, encoding="utf-8")
    inc = ui / "priv_include"
    inc.mkdir(parents=True, exist_ok=True)
    for h in STATIC_SRC.glob("*.h"):
        shutil.copy2(h, inc / h.name)
    write_ui_config(run.screens, inc)
    assets = sorted({n.asset_name for s in run.screens for n in _nodes(s.children)
                     if n.asset_name})
    (inc / "assets.h").write_text(
        "#pragma once\n#include \"lvgl.h\"\n"
        + "".join(f"LV_IMAGE_DECLARE({a});\n" for a in assets), encoding="utf-8")
    return ui


def to_xml(page) -> str:
    """The design as XML text, for failure messages."""
    return ET.tostring(page, encoding="unicode")
