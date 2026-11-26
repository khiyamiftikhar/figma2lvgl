# Coordinate fixtures

Two Figma screens, built by the plugins in `tools/figma_fixture_plugin/`
and exported with the FigML exporter. They settled how the exporter writes
positions (design doc 10, section 3.5) and they are the regression tests for
findings N16, N18, N19, N20 and N23.

| File | What it is |
|---|---|
| `coords.xml` | Round 1 export, screen `coords_test` |
| `figma_positions.json` | Round 1 plugin report: Figma's own position of every node. Transcribed from the report text pasted into the conversation (the plugin shows it in a window) |
| `coords2.xml` | Round 2 export, screen `coords_test_2` |
| `figma_positions2.json` | Round 2 plugin report, as saved from the plugin. Written **before** export, with `predicted_xml_x/y` from the rule; all 24 nodes matched the export |

## The rule they established

A node's XML `x`/`y` is relative to its nearest **Frame, Component or
Instance** ancestor (the screen counts). **Groups have no coordinate space**:
a Group's children are in the enclosing frame's space, like the Group itself.
Auto-layout frames are exported with their children's laid-out positions.
`clipsContent="true"` appears only on frames that clip. Hidden layers are
exported with `visible="false"`, children included.

## Round 1: before and after

Screen positions in pixels. "0.4.6 draws" was measured by building 0.4.6's
output with LVGL 9.3 (default theme, 130 DPI) and reading LVGL's coordinates.

| Button | Containers above it (XML x, y) | Button XML x, y | Figma | 0.4.6 draws | v0.5.0 |
|---|---|---|---|---|---|
| `btn_a` | Group (20, 30) | (20, 30) | (20, 30) | (55, 75), **clipped, not drawn** | (20, 30) |
| `btn_b` | Frame_12 (160, 20) | (15, 25) | (175, 45) | (190, 60) | (175, 45) |
| `btn_c` | Group (30, 140) → Frame_13 (30, 140) | (10, 20) | (40, 160) | (100, 330), **not drawn** | (40, 160) |
| `btn_d` | Frame_14 (170, 140) → Group (25, 35) | (25, 35) | (195, 175) | (250, 240), **not drawn** | (195, 175) |
| `btn_e` | hidden_box, **hidden** (30, 270) | (20, 15) | hidden | (65, 300), drawn | not generated |
| `btn_hidden` | — (hidden itself) | (180, 280) | hidden | (180, 280), drawn | not generated |
| `btn_ghost` | — (opacity 0) | (180, 360) | (180, 360), invisible | (180, 360), invisible | (180, 360), invisible |

## Round 2: before and after

| Button | Case | Figma | 0.4.6 draws | v0.5.0 |
|---|---|---|---|---|
| `btn_g2` | Group → Group | (20, 20) | (90, 90), **not drawn** | (20, 20) |
| `btn_f2` | Frame → Frame | (120, 33) | (150, 63) | (120, 33) |
| `btn_mix` | Frame → Group → Frame | (236, 47) | (291, 122), **not drawn** | (236, 47) |
| `btn_al1` | auto-layout frame (filled) | (20, 120) | (35, 135) | (20, 120) |
| `btn_al2` | auto-layout frame (filled) | (20, 150) | (35, 165) | (20, 150) |
| `btn_al3` | auto-layout frame (filled) | (20, 180) | (35, 195) | (20, 180) |
| `btn_card` | filled frame | (190, 145) | (205, 160) | (190, 145) |
| `btn_pg1` | Group with the `panel` marker | (30, 250) | (75, 515), **not drawn** | (30, 250) |
| `btn_pg2` | Group with the `panel` marker | (90, 280) | (135, 545), **not drawn** | (90, 280) |
| `btn_overflow` | non-clipping frame (no fill) | (210, 315) | (225, 330), partly cut off | (210, 315) |
| `btn_rot` | rotated 30° | (60, 380), rotated | (60, 380), unrotated | (60, 380), unrotated, warning |
| `btn_chip` | inside a component instance | (190, 225) | not generated | not generated (rule R9) |

## Tests

- `tests/test_coords.py`: parses both exports and checks every generated
  widget's screen position against the Figma report, plus hidden layers,
  dropped containers, group panels and clipping. Runs everywhere.
- `tests/compile/test_positions.py`: builds the generated code with LVGL 9.3
  and checks where LVGL draws each button and that it isn't clipped. Runs in
  the gcc compile job.

Both fail on 0.4.6 with the numbers in the tables above.
