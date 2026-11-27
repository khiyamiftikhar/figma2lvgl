# Feature fixtures and showcase (round 3)

Design doc 12, section 7. Built by the Figma plugin in
`tools/figma_fixture_plugin/round3/` at commit `5976091`, run and exported by
the maintainer on 2026-11-27 (FigML exporter).

| File | What |
|---|---|
| `features.xml` | The 14 `f_…` frames, one per construct (test case `features`) |
| `../showcase/showcase.xml` | The 480×320 `showcase` frame (test case `showcase`) |
| `figma/<frame>.png`, `../showcase/figma/showcase.png` | Figma's own render of each frame, 1x |
| `icon_star.png`, `icon_heart.png` | The two icon components |
| `figma_positions.json` | The plugin's report: where Figma places every node |

How the files were made from the export:

- The page was exported as one XML. `features.xml` is that file without the
  `showcase` frame; `showcase.xml` is the same page with only the two icon
  components, the `showcase` frame and the variant component set. No other
  edit. (The `showcase` frame exported on its own has a `<frame>` root, which
  figma2lvgl reads as a page: each child becomes a screen. Recorded as a
  finding; not used here.)
- The frame PNGs are Figma's own 1x exports (Export panel), sent on
  2026-11-27. They replace first versions made from a 2x page export
  (cropped and averaged over 2×2 pixels), which matched them on 98.4–100 %
  of pixels; the differences were in text edges.
- The icon PNGs are Figma's own 1x exports (24×24, with transparency),
  sent separately on 2026-11-27. Generated code only shows images after
  `…_display()`, so they don't appear in the L10 references.

What the tests check:

- L9 (run under the sanitizers): both cases.
- L10 (reference images in `tests/snapshots/features/`): `features` only.
  The references are what LVGL 9.3 draws today, including differences from
  Figma that are recorded as findings, not approved looks.
- L11 (touch): `features` only. `f_covering`: `btn_under_button` must be
  reported covered by `btn_on_top`; `btn_under_panel` must be pressed.
- Progress report (`tests/runtime/test_showcase.py`): per frame, generated
  share and similarity to Figma's PNG, printed in CI. Information only.
