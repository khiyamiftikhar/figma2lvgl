// Round 2 of the figma2lvgl coordinate fixture. Builds cases round 1 didn't
// cover and writes, BEFORE export, the XML x/y the rule predicts:
//   a node's x/y is relative to its nearest Frame-like ancestor
//   (FRAME, COMPONENT, INSTANCE); Groups are transparent.
// Run once on an empty page; export the frame "coords_test_2".

function solid(hex) {
  const n = parseInt(hex.slice(1), 16);
  return [{ type: "SOLID", color: { r: (n >> 16 & 255) / 255, g: (n >> 8 & 255) / 255, b: (n & 255) / 255 } }];
}
function rect(name, x, y, parent, color, w, h) {
  const r = figma.createRectangle();
  r.name = name; r.resize(w || 50, h || 24); r.fills = solid(color);
  parent.appendChild(r); r.x = x; r.y = y; return r;
}
function frame(name, x, y, w, h, parent, fill) {
  const f = figma.createFrame();
  f.name = name; f.resize(w, h); f.fills = fill ? solid(fill) : [];
  parent.appendChild(f); f.x = x; f.y = y; return f;
}

// Screen deliberately NOT at the page origin.
const screen = figma.createFrame();
screen.name = "coords_test_2";
screen.resize(320, 480);
screen.fills = solid("#ffffff");
figma.currentPage.appendChild(screen);
screen.x = 400; screen.y = 100;

// 1. Group -> Group -> btn_g2
const g2b = rect("btn_g2", 20, 20, screen, "#e53935");
const g2in = figma.group([g2b], screen);
const g2out = figma.group([g2in], screen);

// 2. Frame -> Frame -> btn_f2 (no fills)
const fa = frame("Frame 20", 100, 10, 110, 70, screen, null);
const fb = frame("Frame 21", 12, 14, 80, 45, fa, null);
rect("btn_f2", 8, 9, fb, "#43a047");

// 3. Frame -> Group -> Frame -> btn_mix, offsets everywhere
const fm = frame("Frame 22", 220, 10, 95, 90, screen, null);
const fmi = frame("Frame 23", 10, 30, 70, 50, fm, null);
rect("btn_mix", 6, 7, fmi, "#1e88e5");
figma.group([fmi], fm);

// 4. Auto-layout frame, vertical, padding 10, spacing 6, three buttons
const al = frame("list_auto", 10, 110, 130, 120, screen, "#eceff1");
al.layoutMode = "VERTICAL";
al.primaryAxisSizingMode = "FIXED";
al.counterAxisSizingMode = "FIXED";
al.paddingLeft = 10; al.paddingTop = 10; al.paddingRight = 10; al.paddingBottom = 10;
al.itemSpacing = 6;
rect("btn_al1", 0, 0, al, "#8e24aa");
rect("btn_al2", 0, 0, al, "#8e24aa");
rect("btn_al3", 0, 0, al, "#8e24aa");
al.name = "auto_box";   // not a list_ keyword: plain container with fill

// 5. Frame with fill (a PANEL in v0.5.0)
const card = frame("card", 160, 120, 140, 70, screen, "#fff3e0");
rect("btn_card", 30, 25, card, "#fb8c00");

// 6. Group with the panel marker
const pg1 = rect("btn_pg1", 30, 250, screen, "#00897b");
const pg2 = rect("btn_pg2", 90, 280, screen, "#00897b");
const pgrp = figma.group([pg1, pg2], screen);
pgrp.name = "panel_grp";

// 7. Component instance with a child (component kept off-screen)
const comp = figma.createComponent();
comp.name = "chip_component"; comp.resize(70, 34); comp.fills = solid("#d1c4e9");
figma.currentPage.appendChild(comp); comp.x = 800; comp.y = 100;
rect("btn_chip", 10, 5, comp, "#5e35b1");
const inst = comp.createInstance();
screen.appendChild(inst); inst.x = 180; inst.y = 220; inst.name = "chip_inst";

// 8. Frame that doesn't clip
const nc = frame("Frame 24", 180, 290, 60, 40, screen, null);
nc.clipsContent = false;
rect("btn_overflow", 30, 25, nc, "#6d4c41");

// 9. Rotated button (out of scope for positions; recorded to learn the format)
const rot = rect("btn_rot", 60, 380, screen, "#c0ca33");
rot.rotation = 30;

// ---- Report -------------------------------------------------------------
const FRAMEY = new Set(["FRAME", "COMPONENT", "INSTANCE"]);
function absXY(n) { return [n.absoluteTransform[0][2], n.absoluteTransform[1][2]]; }
function r2(v) { return Math.round(v * 100) / 100; }
const [sx, sy] = absXY(screen);
const rows = [];
function walk(node) {
  let anc = node.parent;
  while (anc && !FRAMEY.has(anc.type)) anc = anc.parent;
  const [ax, ay] = absXY(node), [px, py] = absXY(anc);
  rows.push({
    name: node.name, type: node.type, parent: node.parent.name,
    nearest_frame: anc.name,
    predicted_xml_x: r2(ax - px), predicted_xml_y: r2(ay - py),
    screen_x: r2(ax - sx), screen_y: r2(ay - sy),
    rotation: r2(node.rotation || 0),
    layout_parent: node.parent.layoutMode || "NONE",
    clips: "clipsContent" in node ? node.clipsContent : null,
  });
  if ("children" in node) for (const c of node.children) walk(c);
}
for (const c of screen.children) walk(c);

const report = JSON.stringify({ screen: screen.name, screen_on_page: [screen.x, screen.y], nodes: rows }, null, 2);
console.log(report);
figma.viewport.scrollAndZoomIntoView([screen]);
figma.showUI(
  "<p style='font:12px sans-serif'>Copy this report and send it with the XML of coords_test_2:</p>" +
  "<textarea style='width:100%;height:85%;font:11px monospace'>" +
  report.replace(/&/g, "&amp;").replace(/</g, "&lt;") + "</textarea>",
  { width: 440, height: 560 }
);
