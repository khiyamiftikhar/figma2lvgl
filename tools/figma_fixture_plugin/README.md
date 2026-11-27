# Figma fixture plugins

Small Figma plugins that build test screens with exact positions and report
where Figma itself places every node. Used to make the coordinate fixtures in
`tests/fixtures/coords/` (see its README).

- `round1/`: Group, Frame, Group→Frame, Frame→Group, hidden layers, opacity 0.
- `round2/`: nested groups and frames, auto-layout, filled frame, `panel`
  group, component instance, non-clipping frame, rotation. Also writes the
  XML positions the rule predicts, before export.
- `round3/`: the v0.6.0 feature fixtures (one small `f_…` frame per
  construct) and the 480×320 `showcase` (design doc 12, section 7); see
  `tests/fixtures/features/README.md` for what to export.

To run one (Figma desktop app): Plugins → Development → Import plugin from
manifest, pick its `manifest.json`, run it on an empty page, copy the report
from the window that opens, then export the new frame with the FigML exporter.
