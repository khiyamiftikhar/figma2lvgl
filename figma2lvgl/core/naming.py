"""Identifier and widget-classification contract.

Implements design doc 10 ("Generated Identifier and Widget-Classification
Contract", rev 4). The section numbers in comments refer to that document.

This module is the only place that decides what a Figma node becomes and
what it is called. It is pure: it reads XML elements and strings and returns
values and diagnostics; it never writes files or logs.

v0.5.0 PR 1 adds the module and its tests only. The parser and the emitters
start calling it in PRs 2 and 3, so in this PR generated output is unchanged.
"""

from __future__ import annotations

import re
from dataclasses import dataclass, field
from enum import Enum
from typing import Iterable, Optional, Sequence

from figma2lvgl.core.utils.utils import normalize_id, to_snake_case
from figma2lvgl.core.widget_type import WidgetType


# ── Diagnostics ────────────────────────────────────────────────────────────────

@dataclass(frozen=True)
class Diagnostic:
    """One warning or error, tied to the Figma layers it is about (C8).

    Tests assert on ``code`` and ``layers``, never on ``message``.
    """
    code: str
    layers: tuple[str, ...]
    message: str
    old: Optional[str] = None
    new: Optional[str] = None


class NamingError(Exception):
    """A design the contract rejects. Carries the diagnostic."""

    def __init__(self, diagnostic: Diagnostic):
        super().__init__(diagnostic.message)
        self.diagnostic = diagnostic


# ── Fixed name sets ────────────────────────────────────────────────────────────

# C99, C11, C23 and C++20 keywords, including C++ alternative tokens (C1).
# Names are lowercase only, so the C keywords starting with "_" can't occur.
KEYWORDS = frozenset("""
    auto break case char const continue default do double else enum extern
    float for goto if inline int long register restrict return short signed
    sizeof static struct switch typedef union unsigned void volatile while
    alignas alignof bool constexpr false nullptr static_assert thread_local
    true typeof typeof_unqual
    and and_eq asm bitand bitor catch char8_t char16_t char32_t class co_await
    co_return co_yield compl concept const_cast consteval constinit decltype
    delete dynamic_cast explicit export friend mutable namespace new noexcept
    not not_eq operator or or_eq private protected public reinterpret_cast
    requires static_cast template this throw try typeid typename using
    virtual wchar_t xor xor_eq
""".split())

# Field names of the generated structs, reserved in every member scope
# (4.3). tests/test_naming.py checks this against what node_emitter emits.
RESERVED_FIELDS = frozenset({
    "lv_obj", "style", "text", "src", "value", "min", "max", "label_text",
    "lv_screen",
})

# File stems figma2lvgl writes under fixed names (5).
RESERVED_FILE_STEMS = frozenset({"ui_style", "ui_defs", "ui_config", "assets"})

# Name used for each type when a name has to be made up or prefixed (4.2).
TYPE_PREFIX = {
    WidgetType.LABEL: "label",
    WidgetType.BUTTON: "button",
    WidgetType.SLIDER: "slider",
    WidgetType.BAR: "bar",
    WidgetType.IMAGE: "image",
    WidgetType.PANEL: "panel",
    WidgetType.DYNAMIC: "list",
}
SCREEN_PREFIX = "screen"

# Event suffix -> (LVGL event, name used in callback names) (4.1).
EVENTS = {
    "click": ("LV_EVENT_CLICKED", "clicked"),
    "press": ("LV_EVENT_PRESSED", "pressed"),
    "release": ("LV_EVENT_RELEASED", "released"),
    "lp": ("LV_EVENT_LONG_PRESSED", "long_pressed"),
    "lpr": ("LV_EVENT_LONG_PRESSED_REPEAT", "long_pressed_repeat"),
}
DEFAULT_EVENT = "clicked"

# Type keywords and the type each stands for (3.4). Used to report keywords
# that a higher rule overrode.
KEYWORD_TYPES = {
    "btn": WidgetType.BUTTON,
    "button": WidgetType.BUTTON,
    "slider": WidgetType.SLIDER,
    "list": WidgetType.DYNAMIC,
    "grid": WidgetType.DYNAMIC,
    "bar": WidgetType.BAR,
    "icon": WidgetType.IMAGE,
    "image": WidgetType.IMAGE,
    "panel": WidgetType.PANEL,
}

INT32_MIN, INT32_MAX = -(2 ** 31), 2 ** 31 - 1

_IDENT = re.compile(r"[a-z][a-z0-9_]*\Z")
_RANGE_TOKEN = re.compile(r"n?\d+\Z")


def is_valid_identifier(name: str) -> bool:
    """C1: ``[a-z][a-z0-9_]*``, no ``__``, not a keyword."""
    return bool(_IDENT.match(name)) and "__" not in name and name not in KEYWORDS


# ── Tokens (3.1) ───────────────────────────────────────────────────────────────

@dataclass(frozen=True)
class Token:
    text: str   # lowercase
    start: int  # index in the raw name
    end: int


def tokenize(raw: str) -> list[Token]:
    """Split a raw layer name into lowercase tokens, with their positions.

    Separators: any character that isn't an ASCII letter or digit, a
    lower→upper camelCase boundary, and the boundary before the last capital
    of a run followed by a lowercase letter (``HTTPStatus`` → http, status).
    Digits stay attached to the letters around them.
    """
    tokens: list[Token] = []
    start = None
    for i, ch in enumerate(raw):
        alnum = ch.isascii() and ch.isalnum()
        if not alnum:
            if start is not None:
                tokens.append(Token(raw[start:i].lower(), start, i))
                start = None
            continue
        if start is not None and i > start:
            prev = raw[i - 1]
            nxt = raw[i + 1] if i + 1 < len(raw) else ""
            camel = prev.islower() and ch.isupper()
            acronym_end = (prev.isupper() and ch.isupper()
                           and nxt.isascii() and nxt.islower())
            if camel or acronym_end:
                tokens.append(Token(raw[start:i].lower(), start, i))
                start = i
        if start is None:
            start = i
    if start is not None:
        tokens.append(Token(raw[start:].lower(), start, len(raw)))
    return tokens


def _range_value(token: str) -> int:
    return -int(token[1:]) if token.startswith("n") else int(token)


def _strip_end(raw: str, first_removed: Token) -> str:
    """The raw name up to a removed token, without trailing separators."""
    base = raw[:first_removed.start]
    return re.sub(r"[^A-Za-z0-9]+\Z", "", base)


# ── Visibility (3.2) ───────────────────────────────────────────────────────────

def _paint_visible(paint) -> bool:
    if paint.attrib.get("visible", "true").lower() == "false":
        return False
    opacity = paint.attrib.get("opacity")
    if opacity is not None:
        try:
            if float(opacity) == 0:
                return False
        except ValueError:
            pass
    return True


def has_visible_fill(node) -> bool:
    fills = node.find("fills")
    return fills is not None and any(_paint_visible(f) for f in fills.findall("fill"))


def has_visible_stroke(node) -> bool:
    strokes = node.find("strokes")
    return strokes is not None and any(_paint_visible(s) for s in strokes.findall("stroke"))


def has_visual_properties(node) -> bool:
    """A visible fill or stroke. Corner radius alone doesn't count."""
    return has_visible_fill(node) or has_visible_stroke(node)


def is_hidden(node) -> bool:
    return node.attrib.get("visible", "true").lower() == "false"


def _children(node) -> list:
    el = node.find("children")
    return list(el) if el is not None else []


# ── Classification (3.3, 3.4) ──────────────────────────────────────────────────

class Rule(Enum):
    HIDDEN = "hidden"
    R1 = "R1"     # Text → LABEL
    R2 = "R2"     # first token btn/button → BUTTON
    R3 = "R3"     # first or last token slider → SLIDER
    R4 = "R4"     # first token list/grid → DYNAMIC
    R5 = "R5"     # token bar → BAR
    R6 = "R6"     # token icon/image → IMAGE
    R7 = "R7"     # Vector → skipped
    R8 = "R8"     # INSTANCE with Vector children → IMAGE
    R9 = "R9"     # other INSTANCE → skipped
    R10 = "R10"   # first token panel → PANEL
    R11 = "R11"   # children + visual → PANEL
    R12 = "R12"   # children, no visual → STRUCTURAL
    R13 = "R13"   # visual Rectangle/Frame leaf → PANEL
    R14 = "R14"   # other leaf → skipped


_KEYWORD_RULES = {Rule.R2, Rule.R3, Rule.R4, Rule.R5, Rule.R6, Rule.R10}


@dataclass(frozen=True)
class NodeInfo:
    """Everything 3.3 decides for one node."""
    widget_type: Optional[WidgetType]      # None = skipped (with its subtree)
    rule: Rule
    layer: str                             # raw layer name, for diagnostics
    base: str                              # raw name minus modifiers
    event: Optional[str] = None            # BUTTON: callback event name
    value_range: Optional[tuple[int, int]] = None   # SLIDER, BAR
    diagnostics: tuple[Diagnostic, ...] = ()

    @property
    def lvgl_event(self) -> Optional[str]:
        for lv, name in EVENTS.values():
            if name == self.event:
                return lv
        return None


def _match_keyword_rule(kw: list[str]) -> Optional[tuple[Rule, WidgetType]]:
    if not kw:
        return None
    if kw[0] in ("btn", "button"):
        return Rule.R2, WidgetType.BUTTON
    if kw[0] == "slider" or kw[-1] == "slider":
        return Rule.R3, WidgetType.SLIDER
    if kw[0] in ("list", "grid"):
        return Rule.R4, WidgetType.DYNAMIC
    if "bar" in kw:
        return Rule.R5, WidgetType.BAR
    if "icon" in kw or "image" in kw:
        return Rule.R6, WidgetType.IMAGE
    return None


def classify(node) -> NodeInfo:
    """Classify one XML node and split its name (3.3).

    Raises NamingError for a range outside int32 (4.1).
    """
    raw = node.attrib.get("name", "")
    node_type = node.attrib.get("type", "")
    is_instance = node_type == "INSTANCE"   # an instance's tag is its name
    tag = None if is_instance else node.tag
    diags: list[Diagnostic] = []

    # 1. Hidden layer: skipped with its subtree.
    if is_hidden(node):
        return NodeInfo(None, Rule.HIDDEN, raw, raw)

    # 2-3. Tokens; a trailing pair of integers is a candidate range.
    tokens = tokenize(raw)
    words = [t.text for t in tokens]
    candidate = None
    if len(words) >= 2 and _RANGE_TOKEN.match(words[-1]) and _RANGE_TOKEN.match(words[-2]):
        candidate = (tokens[-2], tokens[-1])
        kw = words[:-2]
    else:
        kw = words

    # 4. Rules in priority order.
    keyword_hit = _match_keyword_rule(kw)
    has_kids = bool(_children(node))
    if tag == "Text":
        rule, wtype = Rule.R1, WidgetType.LABEL
    elif keyword_hit and keyword_hit[0] != Rule.R10:
        rule, wtype = keyword_hit
    elif tag == "Vector":
        rule, wtype = Rule.R7, None
    elif is_instance and any(c.tag == "Vector" for c in _children(node)):
        rule, wtype = Rule.R8, WidgetType.IMAGE
    elif is_instance:
        rule, wtype = Rule.R9, None
        diags.append(Diagnostic(
            "skipped_instance", (raw,),
            f"skipping component instance '{raw}': no Vector children and no "
            f"type keyword"))
    elif kw and kw[0] == "panel":
        rule, wtype = Rule.R10, WidgetType.PANEL
    elif has_kids and has_visual_properties(node):
        rule, wtype = Rule.R11, WidgetType.PANEL
    elif has_kids:
        rule, wtype = Rule.R12, WidgetType.STRUCTURAL
    elif tag in ("Rectangle", "Frame") and has_visual_properties(node):
        rule, wtype = Rule.R13, WidgetType.PANEL
    else:
        rule, wtype = Rule.R14, None
        if has_visual_properties(node):
            why = (f"{node.tag} layers aren't drawn as boxes (only Rectangle "
                   f"and Frame are); export it as an image named icon_* or image_*")
        else:
            why = ("no type keyword and nothing visible. Rename it (btn_, "
                   "slider_, list_, bar, icon, image, panel) or give it a fill "
                   "or stroke")
        diags.append(Diagnostic(
            "skipped_leaf", (raw,), f"skipping '{raw}' (tag {node.tag}): {why}"))

    # Keywords a keyword rule overrode (3.4, "Mixed keywords").
    if rule in _KEYWORD_RULES:
        ignored = sorted({w for w in kw if w in KEYWORD_TYPES
                          and KEYWORD_TYPES[w] != wtype}, key=kw.index)
        if ignored:
            diags.append(Diagnostic(
                "keyword_conflict", (raw,),
                f"'{raw}' matches {rule.value} ({wtype.name}); ignoring "
                f"keyword(s) {', '.join(ignored)}",
                old=",".join(ignored), new=rule.value))

    # 5. The candidate range belongs to sliders and bars only.
    value_range = None
    base = raw
    rest = tokens
    if candidate and wtype in (WidgetType.SLIDER, WidgetType.BAR):
        lo, hi = _range_value(candidate[0].text), _range_value(candidate[1].text)
        for v in (lo, hi):
            if not INT32_MIN <= v <= INT32_MAX:
                raise NamingError(Diagnostic(
                    "range_out_of_bounds", (raw,),
                    f"'{raw}': range value {v} doesn't fit in int32_t"))
        if lo == hi:
            diags.append(Diagnostic(
                "range_degenerate", (raw,),
                f"'{raw}': range {lo}..{hi} is empty; the widget can't move"))
        value_range = (lo, hi)
        base = _strip_end(raw, candidate[0])
        rest = tokens[:-2]

    # 6. Event suffix, buttons only, never leaving just the keyword.
    event = None
    if wtype == WidgetType.BUTTON:
        event = DEFAULT_EVENT
        if rest and rest[-1].text in EVENTS:
            if len(rest) >= 3:
                event = EVENTS[rest[-1].text][1]
                base = _strip_end(raw, rest[-1])
            else:
                diags.append(Diagnostic(
                    "event_suffix_kept", (raw,),
                    f"'{raw}': '{rest[-1].text}' kept as part of the name "
                    f"(removing it would leave only the keyword); default "
                    f"event 'clicked'"))

    # Rotation isn't supported (3.5).
    rotation = node.attrib.get("rotation")
    if wtype is not None and wtype != WidgetType.STRUCTURAL and rotation:
        try:
            if abs(float(rotation)) > 1e-6:
                diags.append(Diagnostic(
                    "rotation_ignored", (raw,),
                    f"'{raw}' is rotated {float(rotation):g}°; drawn unrotated"))
        except ValueError:
            pass

    return NodeInfo(wtype, rule, raw, base, event, value_range, tuple(diags))


# ── Identifiers (4.2, 4.3) ─────────────────────────────────────────────────────

class Counters:
    """Empty-name counters, one per type. Create one per screen (4.2).

    A number is used only when an empty name is replaced; valid names and
    skipped nodes don't consume numbers.
    """

    def __init__(self):
        self._next: dict[str, int] = {}

    def next(self, prefix: str) -> int:
        n = self._next.get(prefix, 1)
        self._next[prefix] = n + 1
        return n


def to_identifier(base: str, prefix: str, counters: Counters,
                  layer: Optional[str] = None) -> tuple[str, list[Diagnostic]]:
    """Step 2: the 0.4.6 normalization, then the C1/C5 fixes (4.2).

    ``prefix`` is the type name used for fixes (TYPE_PREFIX or "screen").
    """
    layer = base if layer is None else layer
    name = normalize_id(base.lower())
    fixed, why = name, None
    if not name:
        fixed, why = f"{prefix}_{counters.next(prefix)}", "empty"
    elif name[0].isdigit():
        fixed, why = f"{prefix}_{name}", "starts with a digit"
    elif name.startswith("lv_"):
        fixed, why = f"{prefix}_{name}", "LVGL namespace"
    elif name in KEYWORDS:
        # A prefix, not a trailing "_": member names are joined with "_" into
        # function names, and "default_" + "_set_text" would contain "__" (C1).
        fixed, why = f"{prefix}_{name}", "C/C++ keyword"
    if why is None:
        return name, []
    return fixed, [Diagnostic(
        "renamed_invalid", (layer,),
        f"layer '{layer}': name '{name}' is not usable in C ({why}); "
        f"using '{fixed}'", old=name, new=fixed)]


def member_candidate(info: NodeInfo, counters: Counters) -> tuple[str, list[Diagnostic]]:
    """Member name before sibling uniqueness: 4.2 plus reserved fields (4.3)."""
    name, diags = to_identifier(info.base, TYPE_PREFIX[info.widget_type],
                                counters, info.layer)
    if name in RESERVED_FIELDS:
        new = f"{TYPE_PREFIX[info.widget_type]}_{name}"
        diags.append(Diagnostic(
            "renamed_reserved", (info.layer,),
            f"layer '{info.layer}': '{name}' is a generated field name; "
            f"using '{new}'", old=name, new=new))
        name = new
    return name, diags


# ── Allocator (4.5) ────────────────────────────────────────────────────────────

class Allocator:
    """One per name space. Natural names first, then suffixes (4.5).

    ``reserved`` names are never handed out; a candidate equal to one goes to
    the suffix pass. With ``casefold`` names are compared case-insensitively.
    """

    def __init__(self, reserved: Iterable[str] = (), casefold: bool = False,
                 code: str = "suffixed_duplicate"):
        self._casefold = casefold
        self._reserved = {self._key(n) for n in reserved}
        self._taken: dict[str, str] = {}   # key -> layer that holds it
        self._code = code

    def _key(self, name: str) -> str:
        return name.casefold() if self._casefold else name

    def reserve(self, names: Iterable[str]) -> None:
        self._reserved.update(self._key(n) for n in names)

    def __contains__(self, name: str) -> bool:
        return self._key(name) in self._taken

    def claim_all(self, candidates: Sequence[tuple[str, str]]
                  ) -> tuple[list[str], list[Diagnostic]]:
        """Claim (name, layer) pairs in order. Returns names in the same order."""
        result: list[Optional[str]] = [None] * len(candidates)
        deferred = []
        for i, (name, layer) in enumerate(candidates):
            key = self._key(name)
            if key in self._reserved or key in self._taken:
                deferred.append(i)
            else:
                self._taken[key] = layer
                result[i] = name
        diags = []
        for i in deferred:
            name, layer = candidates[i]
            k = 2
            while (self._key(f"{name}_{k}") in self._taken
                   or self._key(f"{name}_{k}") in self._reserved):
                k += 1
            new = f"{name}_{k}"
            self._taken[self._key(new)] = layer
            result[i] = new
            other = self._taken.get(self._key(name))
            layers = (layer,) if other is None else (layer, other)
            reason = ("is reserved" if self._key(name) in self._reserved
                      else f"is already used by '{other}'")
            diags.append(Diagnostic(
                self._code, layers,
                f"layer '{layer}': '{name}' {reason}; using '{new}'",
                old=name, new=new))
        return result, diags  # type: ignore[return-value]


# ── Screens (4.2, 5) ───────────────────────────────────────────────────────────

def screen_name_candidate(raw: str, counters: Counters) -> tuple[str, list[Diagnostic]]:
    """0.4.6's to_snake_case (Q9), then the C1/C5 fixes."""
    return to_identifier(to_snake_case(raw), SCREEN_PREFIX, counters, raw)


def assign_screen_names(raws: Sequence[str]) -> tuple[list[str], list[Diagnostic]]:
    """Names for all screens, in XML order.

    Two frames with the same name are an error (C4). A name whose files would
    hit a reserved stem (``style`` → ``ui_style.c``) gets a suffix.
    """
    counters = Counters()
    cands, diags = [], []
    for raw in raws:
        name, d = screen_name_candidate(raw, counters)
        cands.append((name, raw))
        diags += d
    seen: dict[str, str] = {}
    for name, raw in cands:
        if name in seen:
            raise NamingError(Diagnostic(
                "screen_collision", (seen[name], raw),
                f"frames '{seen[name]}' and '{raw}' both map to screen name "
                f"'{name}' (files ui_{name}.c/.h)"))
        seen[name] = raw
    reserved = {stem[3:] for stem in RESERVED_FILE_STEMS if stem.startswith("ui_")}
    names, d = Allocator(reserved, code="renamed_reserved").claim_all(cands)
    d = [Diagnostic(x.code, x.layers,
                    f"frame '{x.layers[0]}': screen name '{x.old}' would write "
                    f"ui_{x.old}.c/.h, which are figma2lvgl's own files; using "
                    f"'{x.new}'", x.old, x.new) for x in d]
    return names, diags + d


# ── Images (4.4) ───────────────────────────────────────────────────────────────

@dataclass(frozen=True)
class ImageUse:
    asset: str
    layer: str
    component_id: Optional[str]
    size: tuple[int, int]


def asset_name(base: str, counters: Counters, layer: Optional[str] = None
               ) -> tuple[str, list[Diagnostic]]:
    """Image symbol and PNG stem: from the base name, not the member (4.4)."""
    layer = base if layer is None else layer
    name, diags = to_identifier(base, TYPE_PREFIX[WidgetType.IMAGE], counters, layer)
    if name in RESERVED_FILE_STEMS:
        new = f"image_{name}"
        diags.append(Diagnostic(
            "renamed_reserved", (layer,),
            f"layer '{layer}': image file '{name}.c' would replace a "
            f"figma2lvgl file; using '{new}'", old=name, new=new))
        name = new
    return name, diags


def check_asset_identity(uses: Sequence[ImageUse]) -> list[Diagnostic]:
    """Layers sharing an asset name must show the same picture (4.4).

    Raises NamingError when two uses name different components.
    """
    by_asset: dict[str, list[ImageUse]] = {}
    for u in uses:
        by_asset.setdefault(u.asset, []).append(u)
    diags = []
    for asset, group in by_asset.items():
        if len(group) < 2:
            continue
        ids = {u.component_id for u in group if u.component_id is not None}
        layers = tuple(u.layer for u in group)
        if len(ids) > 1:
            raise NamingError(Diagnostic(
                "asset_conflict", layers,
                f"layers {', '.join(layers)} share the image name '{asset}' "
                f"but are instances of different components; rename one"))
        if any(u.component_id is None for u in group):
            sizes = {u.size for u in group}
            extra = (" Their sizes differ: " + ", ".join(
                f"{w}x{h}" for w, h in sorted(sizes))) if len(sizes) > 1 else ""
            diags.append(Diagnostic(
                "asset_unverified", layers,
                f"layers {', '.join(layers)} all show {asset}.png; figma2lvgl "
                f"can't check they are the same picture.{extra}"))
    return diags


# ── External symbols (4.6) ─────────────────────────────────────────────────────

@dataclass(frozen=True)
class WidgetSymbols:
    """What one widget needs, for assign_symbols."""
    key: object                     # caller's handle for the widget
    path: tuple[str, ...]           # member names from the screen down
    layer: str
    setter_ops: tuple[str, ...] = ()    # "set_text", "set_value", ...
    events: tuple[str, ...] = ()        # button callback event names
    slider_callback: bool = False
    accessor: bool = False


@dataclass(frozen=True)
class ScreenSymbols:
    screen: str
    widgets: tuple[WidgetSymbols, ...]


def setter_name(screen: str, path: Sequence[str], op: str) -> str:
    """Full path, no truncation (N4)."""
    return f"ui_{screen}_{'_'.join(path)}_{op}"


def _short_names(screen: str, w: WidgetSymbols, q: int) -> list[tuple[str, str]]:
    member = "_".join(w.path[-q:])
    out = [(("cb", e), f"ui_{screen}_on_{member}_{e}") for e in w.events]
    if w.slider_callback:
        out.append((("cb", None), f"ui_{screen}_on_{member}"))
    if w.accessor:
        out.append((("get", None), f"ui_{screen}_get_{member}"))
    return out


def _qualify(screen: str, widgets: Sequence[WidgetSymbols]) -> dict[object, int]:
    """How many path parts each widget's callback/accessor name uses (4.6)."""
    q = {w.key: 1 for w in widgets}
    while True:
        owners: dict[str, list[WidgetSymbols]] = {}
        for w in widgets:
            for _, name in _short_names(screen, w, q[w.key]):
                owners.setdefault(name, []).append(w)
        changed = False
        for group in owners.values():
            if len({id(w) for w in group}) < 2:
                continue
            for w in group:
                if q[w.key] < len(w.path):
                    q[w.key] += 1
                    changed = True
        if not changed:
            return q


def assign_symbols(screens: Sequence[ScreenSymbols], symbols: Allocator
                   ) -> tuple[dict[tuple, str], list[Diagnostic]]:
    """Every external symbol of the design (4.6), claimed in one allocator.

    Keys: (screen, "init"), (screen, "load"), (widget key, op) for setters,
    (widget key, "cb", event or None) for callbacks, (widget key, "get").
    All candidates are collected first, so natural names win over suffixes
    across the whole design (4.5).
    """
    keys, cands = [], []
    for s in screens:
        keys += [(s.screen, "init"), (s.screen, "load")]
        cands += [(f"ui_{s.screen}_init", s.screen), (f"ui_{s.screen}_load", s.screen)]
        q = _qualify(s.screen, s.widgets)
        for w in s.widgets:
            for op in w.setter_ops:
                keys.append((w.key, op))
                cands.append((setter_name(s.screen, w.path, op), w.layer))
            for (kind, ev), name in _short_names(s.screen, w, q[w.key]):
                keys.append((w.key, kind, ev) if kind == "cb" else (w.key, kind))
                cands.append((name, w.layer))
    names, diags = symbols.claim_all(cands)
    return dict(zip(keys, names)), diags


# ── Whole-design check (5) ─────────────────────────────────────────────────────

@dataclass
class OutputManifest:
    """Every generated name, with the layer or file that produced it."""
    symbols: list[tuple[str, str]] = field(default_factory=list)
    macros: list[tuple[str, str]] = field(default_factory=list)
    files: list[tuple[str, str]] = field(default_factory=list)   # (path, origin)


def check_namespaces(manifest: OutputManifest) -> list[Diagnostic]:
    """Repeats in any name space. With sections 3-4 there should be none, so
    each is an internal error. Files are compared case-insensitively."""
    diags = []
    for space, entries, fold in (("symbol", manifest.symbols, False),
                                 ("macro", manifest.macros, False),
                                 ("file", manifest.files, True)):
        seen: dict[str, str] = {}
        for name, origin in entries:
            key = name.casefold() if fold else name
            if key in seen:
                diags.append(Diagnostic(
                    "internal_collision", (seen[key], origin),
                    f"internal error: {space} '{name}' generated twice "
                    f"(from '{seen[key]}' and '{origin}')", old=name))
            else:
                seen[key] = origin
    return diags
