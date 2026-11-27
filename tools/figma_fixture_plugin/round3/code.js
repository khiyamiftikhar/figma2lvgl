// Round 3 of the figma2lvgl fixture plugin (design doc 12, section 7).
// Builds, on the current (empty) page:
//   - one small frame per construct, the feature fixtures ("f_..."), and
//   - "showcase", a 480x320 screen with all of them plus constructs the
//     tool doesn't support yet,
// then reports where Figma places every node, for the fixtures' README.
// Run once on an empty page. Export:
//   1. the f_* frames as one XML (features.xml) with the FigML exporter,
//   2. the showcase frame as its own XML (showcase.xml),
//   3. every frame as PNG at 1x (named after the frame),
//   4. the two icon components ("icon_star", "icon_heart") as PNG at 1x.

const FONT = { family: "Montserrat", style: "Regular" };
const CUSTOM_FONT = { family: "Inter", style: "Regular" };   // not Montserrat: unsupported

function solid(hex, opacity) {
  const n = parseInt(hex.slice(1), 16);
  const p = { type: "SOLID", color: { r: (n >> 16 & 255) / 255, g: (n >> 8 & 255) / 255, b: (n & 255) / 255 } };
  if (opacity !== undefined) p.opacity = opacity;
  return [p];
}
function place(node, parent, x, y) { parent.appendChild(node); node.x = x; node.y = y; return node; }
function rect(name, parent, x, y, w, h, color) {
  const r = figma.createRectangle();
  r.name = name; r.resize(w, h); r.fills = color ? solid(color) : [];
  return place(r, parent, x, y);
}
function frame(name, parent, x, y, w, h, fill) {
  const f = figma.createFrame();
  f.name = name; f.resize(w, h); f.fills = fill ? solid(fill) : [];
  return place(f, parent, x, y);
}
function text(name, parent, x, y, chars, size, color, font) {
  const t = figma.createText();
  t.fontName = font || FONT; t.fontSize = size || 14;
  t.characters = chars; t.name = name; t.fills = solid(color || "#212121");
  return place(t, parent, x, y);
}
// A button: a filled frame with a label child (the button's text).
function button(name, parent, x, y, w, h, label, color, opts) {
  const b = frame(name, parent, x, y, w, h, color || "#2196f3");
  opts = opts || {};
  if (opts.radius !== undefined) b.cornerRadius = opts.radius;
  if (opts.stroke) { b.strokes = solid(opts.stroke); b.strokeWeight = opts.strokeWeight || 2; b.strokeAlign = "INSIDE"; }
  if (label) {
    const t = text("label", b, 0, 0, label, opts.size || 14, "#ffffff");
    t.x = Math.round((w - t.width) / 2); t.y = Math.round((h - t.height) / 2);
  }
  return b;
}

// Icon components: vector shapes only, so their instances become images.
function icon(name, x, path, color) {
  const c = figma.createComponent();
  c.name = name; c.resize(24, 24); c.fills = [];
  figma.currentPage.appendChild(c); c.x = x; c.y = -200;
  const v = figma.createVector();
  v.vectorPaths = [{ windingRule: "NONZERO", data: path }];
  v.fills = solid(color); v.name = "shape";
  c.appendChild(v); v.x = 2; v.y = 2;
  return c;
}

const FIXTURES = [];   // frames, in page order

// Each fixture: function(frame) building its content, and its size.
const BUILDERS = {
  // Button with stroke and radius.
  f_button_style(f) {
    button("btn_square", f, 10, 10, 90, 40, "Square");
    button("btn_round", f, 110, 10, 90, 40, "Round", "#43a047", { radius: 8 });
    button("btn_pill", f, 210, 10, 100, 40, "Pill", "#8e24aa", { radius: 20 });
    button("btn_stroke", f, 10, 70, 90, 40, "Stroke", "#ffffff", { stroke: "#1e88e5", strokeWeight: 2 });
    button("btn_both", f, 110, 70, 90, 40, "Both", "#fb8c00", { radius: 10, stroke: "#5d4037", strokeWeight: 3 });
    button("btn_thick", f, 210, 70, 100, 40, "Thick", "#00897b", { radius: 4, stroke: "#004d40", strokeWeight: 6 });
  },
  // Button with each event suffix.
  f_button_events(f) {
    button("btn_tap", f, 10, 10, 140, 40, "Tap");
    button("btn_hold_lp", f, 170, 10, 140, 40, "Hold");
    button("btn_repeat_lpr", f, 10, 70, 140, 40, "Repeat");
    button("btn_down_press", f, 170, 70, 140, 40, "Down");
    button("btn_up_release", f, 10, 130, 140, 40, "Up");
  },
  // Bar with range, reversed range, negative range.
  f_bars(f) {
    rect("level_bar", f, 10, 20, 300, 20, "#4caf50");
    rect("volume_bar_0_255", f, 10, 60, 300, 20, "#2196f3");
    rect("drain_bar_100_0", f, 10, 100, 300, 20, "#ff9800");
    rect("temp_bar_n20_50", f, 10, 140, 300, 20, "#e91e63");
    rect("tall_bar", f, 10, 180, 20, 50, "#795548");   // height > width: vertical
  },
  // Slider horizontal and vertical.
  f_sliders(f) {
    rect("slider_h", f, 20, 30, 220, 16, "#9e9e9e");
    rect("slider_h_0_10", f, 20, 80, 220, 16, "#9e9e9e");
    rect("slider_v", f, 270, 20, 16, 200, "#9e9e9e");
  },
  // Nested panels three deep, clipping on and off.
  f_nesting(f) {
    const a = frame("panel_outer", f, 10, 10, 140, 140, "#e3f2fd");
    a.clipsContent = true;
    const b = frame("panel_middle", a, 20, 20, 120, 100, "#90caf9");
    b.clipsContent = true;
    const c = frame("panel_inner", b, 30, 30, 110, 60, "#1e88e5");
    c.clipsContent = true;
    rect("box_clipped", c, 40, 10, 60, 30, "#0d47a1");   // overflows panel_middle: clipped
    const d = frame("panel_open", f, 170, 10, 140, 140, "#fff3e0");
    d.clipsContent = false;
    const e = frame("panel_open_mid", d, 20, 20, 120, 100, "#ffcc80");
    e.clipsContent = false;
    const g = frame("panel_open_inner", e, 30, 30, 110, 60, "#fb8c00");
    g.clipsContent = false;
    rect("box_overflow", g, 40, 10, 60, 30, "#e65100");   // drawn outside: no clipping
  },
  // A panel over a button (pressable) and a button over a button (covered).
  f_covering(f) {
    button("btn_under_panel", f, 20, 30, 120, 50, "Under");
    frame("panel_cover", f, 10, 20, 100, 70, "#bdbdbd");
    button("btn_under_button", f, 180, 30, 120, 50, "Under");
    button("btn_on_top", f, 170, 20, 80, 70, "Top", "#e53935");
  },
  // Image instances: two of one component, one of another.
  f_images(f, comps) {
    place(comps.star.createInstance(), f, 20, 20).name = "icon_star";
    place(comps.star.createInstance(), f, 60, 20).name = "icon_star";
    place(comps.heart.createInstance(), f, 100, 20).name = "icon_heart";
    // Inside a panel, to check the path and position.
    const p = frame("panel_icons", f, 20, 80, 120, 50, "#f5f5f5");
    place(comps.heart.createInstance(), p, 10, 13).name = "icon_heart";
  },
  // Text at every Montserrat size LVGL is built with (8 to 48).
  f_text_sizes(f) {
    let y = 4, col = 0;
    for (let s = 8; s <= 48; s += 2) {
      if (s === 32) { y = 4; col = 1; }
      const t = text("size_" + s, f, 6 + col * 160, y, "Ag " + s, s);
      y += Math.ceil(t.height) + 2;
    }
  },
  // Multi-line text: explicit line breaks, and wrapping at a fixed width.
  f_text_multiline(f) {
    text("lines", f, 10, 10, "First line\nSecond line\nThird", 16);
    const w = text("wrapped", f, 10, 100, "This sentence is long enough to wrap inside a fixed width box.", 14);
    w.textAutoResize = "HEIGHT"; w.resize(180, w.height);
    const c = text("centered", f, 10, 190, "Centered\ntext", 18);
    c.textAlignHorizontal = "CENTER"; c.textAutoResize = "NONE"; c.resize(300, 50);
  },
  // Opacity 0, 50 % and 100 %, on panels, buttons and text.
  f_opacity(f) {
    frame("panel_opa_0", f, 10, 10, 90, 60, "#3f51b5").opacity = 0;
    frame("panel_opa_50", f, 115, 10, 90, 60, "#3f51b5").opacity = 0.5;
    frame("panel_opa_100", f, 220, 10, 90, 60, "#3f51b5").opacity = 1;
    button("btn_opa_50", f, 10, 90, 140, 40, "Half").opacity = 0.5;
    text("text_opa_50", f, 170, 100, "Half text", 16).opacity = 0.5;
    frame("panel_fill_50", f, 10, 150, 140, 50, null).fills = solid("#e91e63", 0.5);
  },
  // A hidden subtree: nothing of it is generated or drawn.
  f_hidden(f) {
    const h = frame("panel_hidden", f, 10, 10, 200, 120, "#ffcdd2");
    button("btn_inside_hidden", h, 10, 10, 120, 40, "Hidden");
    text("text_inside_hidden", h, 10, 70, "Not drawn", 14);
    h.visible = false;
    rect("box_hidden", f, 220, 10, 60, 60, "#d32f2f").visible = false;
    button("btn_visible", f, 10, 150, 120, 40, "Visible");
  },
  // A rotated layer (drawn unrotated, with a warning).
  f_rotated(f) {
    rect("box_rotated", f, 60, 40, 120, 40, "#c0ca33").rotation = 30;
    button("btn_rotated", f, 160, 140, 120, 40, "Tilted").rotation = -15;
  },
  // A list with a design-time child (children aren't generated).
  f_list(f) {
    const l = frame("list_items", f, 10, 10, 200, 200, "#eceff1");
    button("btn_row_example", l, 10, 10, 180, 40, "Row");
    text("row_hint", l, 10, 60, "design-time only", 12);
    frame("grid_cells", f, 220, 10, 90, 200, "#e8eaf6");
  },
  // The smallest buttons: rendering and input at 4x4 and 1x1.
  f_tiny(f) {
    rect("btn_4x4", f, 20, 20, 4, 4, "#d32f2f");
    rect("btn_1x1", f, 60, 20, 1, 1, "#d32f2f");
    rect("slider_tiny", f, 20, 60, 40, 4, "#9e9e9e");
  },
};

const SIZES = { f_text_sizes: [320, 500], f_text_multiline: [320, 240] };

// ---- Showcase: everything together, plus what isn't supported yet ------
function showcase(s, comps) {
  // Supported
  const top = frame("panel_top", s, 0, 0, 480, 40, "#263238");
  text("title", top, 12, 10, "Showcase", 16, "#ffffff");
  place(comps.star.createInstance(), top, 444, 8).name = "icon_star";
  button("btn_ok", s, 12, 52, 100, 40, "OK", "#43a047", { radius: 8 });
  button("btn_cancel_lp", s, 122, 52, 100, 40, "Cancel", "#ffffff", { radius: 8, stroke: "#e53935", strokeWeight: 2 });
  rect("battery_bar_0_100", s, 12, 104, 210, 16, "#4caf50");
  rect("slider_volume", s, 12, 132, 210, 16, "#9e9e9e");
  rect("slider_level", s, 236, 52, 16, 120, "#9e9e9e");
  const card = frame("panel_card", s, 12, 160, 210, 100, "#eceff1");
  card.cornerRadius = 8; card.clipsContent = true;
  text("card_title", card, 10, 8, "Card", 18);
  text("card_body", card, 10, 36, "Two lines of\nbody text", 12);
  place(comps.heart.createInstance(), card, 176, 8).name = "icon_heart";
  frame("list_devices", s, 12, 268, 210, 44, "#e0e0e0");
  frame("panel_dim", s, 236, 180, 40, 40, "#3f51b5").opacity = 0.5;
  // Not supported yet
  const grad = frame("panel_gradient", s, 270, 52, 200, 40, null);
  grad.fills = [{ type: "GRADIENT_LINEAR",
    gradientTransform: [[1, 0, 0], [0, 1, 0]],
    gradientStops: [{ position: 0, color: { r: 0.13, g: 0.59, b: 0.95, a: 1 } },
                    { position: 1, color: { r: 0.91, g: 0.12, b: 0.39, a: 1 } }] }];
  const sh = button("btn_shadow", s, 270, 100, 200, 40, "Shadow", "#ffffff", { radius: 8 });
  sh.children[0].fills = solid("#212121");
  sh.effects = [{ type: "DROP_SHADOW", color: { r: 0, g: 0, b: 0, a: 0.35 }, offset: { x: 0, y: 3 },
    radius: 6, spread: 0, visible: true, blendMode: "NORMAL" }];
  const row = frame("panel_row", s, 270, 150, 200, 44, "#fff8e1");
  row.layoutMode = "HORIZONTAL"; row.primaryAxisSizingMode = "FIXED"; row.counterAxisSizingMode = "FIXED";
  row.paddingLeft = row.paddingRight = row.paddingTop = row.paddingBottom = 6; row.itemSpacing = 6;
  for (const n of ["btn_fill_a", "btn_fill_b"]) {
    const b = button(n, row, 0, 0, 60, 32, n.slice(-1).toUpperCase(), "#ffb300");
    b.layoutSizingHorizontal = "FILL";
  }
  text("custom_font", s, 270, 204, "Inter font", 18, "#212121", CUSTOM_FONT);
  // Variant component: a component set with two states, one instance.
  const on = figma.createComponent(); on.name = "state=on"; on.resize(48, 24); on.fills = solid("#43a047"); on.cornerRadius = 12;
  const off = figma.createComponent(); off.name = "state=off"; off.resize(48, 24); off.fills = solid("#bdbdbd"); off.cornerRadius = 12;
  figma.currentPage.appendChild(on); figma.currentPage.appendChild(off);
  const set = figma.combineAsVariants([on, off], figma.currentPage);
  set.name = "toggle"; set.x = 200; set.y = -200;
  place(on.createInstance(), s, 270, 240).name = "toggle_wifi";
  // Icon button: a button whose content is an icon instance and a label.
  const ib = button("btn_icon", s, 330, 236, 140, 36, null, "#1e88e5", { radius: 6 });
  place(comps.star.createInstance(), ib, 8, 6).name = "icon_star";
  text("label", ib, 40, 9, "Favourite", 14, "#ffffff");
}

// ---- Report ---------------------------------------------------------------
const FRAMEY = new Set(["FRAME", "COMPONENT", "INSTANCE"]);
function absXY(n) { return [n.absoluteTransform[0][2], n.absoluteTransform[1][2]]; }
function r2(v) { return Math.round(v * 100) / 100; }
function report(screen) {
  const [sx, sy] = absXY(screen);
  const rows = [];
  function walk(node) {
    let anc = node.parent;
    while (anc && !FRAMEY.has(anc.type)) anc = anc.parent;
    const [ax, ay] = absXY(node), [px, py] = absXY(anc);
    rows.push({
      name: node.name, type: node.type, parent: node.parent.name,
      xml_x: r2(ax - px), xml_y: r2(ay - py), screen_x: r2(ax - sx), screen_y: r2(ay - sy),
      w: r2(node.width), h: r2(node.height), visible: node.visible,
      opacity: r2("opacity" in node ? node.opacity : 1), rotation: r2(node.rotation || 0),
    });
    if ("children" in node && node.type !== "INSTANCE") for (const c of node.children) walk(c);
  }
  for (const c of screen.children) walk(c);
  return { screen: screen.name, size: [screen.width, screen.height], nodes: rows };
}

async function main() {
  await figma.loadFontAsync(FONT);
  await figma.loadFontAsync(CUSTOM_FONT);
  const comps = {
    star: icon("icon_star", 0, "M 10 0 L 13 7 L 20 7 L 14 12 L 16 20 L 10 15 L 4 20 L 6 12 L 0 7 L 7 7 Z", "#fbc02d"),
    heart: icon("icon_heart", 60, "M 10 18 L 1 9 C -2 5 2 -1 6 1 L 10 4 L 14 1 C 18 -1 22 5 19 9 Z", "#e53935"),
  };
  let col = 0, row = 0;
  for (const name of Object.keys(BUILDERS)) {
    const [w, h] = SIZES[name] || [320, 240];
    const f = frame(name, figma.currentPage, col * 360, row * 520, w, h, "#ffffff");
    f.clipsContent = true;
    BUILDERS[name](f, comps);
    FIXTURES.push(f);
    if (++col === 5) { col = 0; row++; }
  }
  const s = frame("showcase", figma.currentPage, 0, (row + 1) * 520 + 40, 480, 320, "#fafafa");
  s.clipsContent = true;
  showcase(s, comps);

  const out = JSON.stringify({
    plugin: "figma2lvgl fixture round 3",
    frames: FIXTURES.concat([s]).map(report),
  }, null, 1);
  figma.viewport.scrollAndZoomIntoView(FIXTURES.concat([s]));
  figma.showUI(
    "<p style='font:12px sans-serif'>Copy this report and send it with the exports (save it as figma_positions.json):</p>" +
    "<textarea style='width:100%;height:85%;font:11px monospace'>" +
    out.replace(/&/g, "&amp;").replace(/</g, "&lt;") + "</textarea>",
    { width: 480, height: 600 }
  );
}

main().catch(e => figma.closePlugin("round 3 failed: " + e.message));
