// Builds the figma2lvgl N16/N18 test screen and reports where Figma places
// every node. Run once on an empty page; export the frame "coords_test".

function solid(hex) {
  const n = parseInt(hex.slice(1), 16);
  return [{ type: "SOLID", color: { r: (n >> 16 & 255) / 255, g: (n >> 8 & 255) / 255, b: (n & 255) / 255 } }];
}

function btn(name, x, y, parent, color) {
  const r = figma.createRectangle();
  r.name = name;
  r.resize(60, 30);
  r.fills = solid(color);
  parent.appendChild(r);
  r.x = x;
  r.y = y;
  return r;
}

function frame(name, x, y, w, h, parent, fill) {
  const f = figma.createFrame();
  f.name = name;
  f.resize(w, h);
  f.fills = fill ? solid(fill) : [];
  parent.appendChild(f);
  f.x = x;
  f.y = y;
  return f;
}

const screen = figma.createFrame();
screen.name = "coords_test";
screen.resize(320, 480);
screen.fills = solid("#ffffff");
screen.x = 0;
screen.y = 0;
figma.currentPage.appendChild(screen);

// a: Group -> btn_a
const a = btn("btn_a", 20, 30, screen, "#e53935");
const gA = figma.group([a], screen);

// b: Frame 12 (no fill) at (160, 20) -> btn_b at (15, 25)
const f12 = frame("Frame 12", 160, 20, 120, 80, screen, null);
btn("btn_b", 15, 25, f12, "#43a047");

// c: Group -> Frame 13 (no fill) at (30, 140) -> btn_c at (10, 20)
const f13 = frame("Frame 13", 30, 140, 110, 80, screen, null);
btn("btn_c", 10, 20, f13, "#1e88e5");
const gC = figma.group([f13], screen);

// d: Frame 14 (no fill) at (170, 140) -> Group -> btn_d at (25, 35)
const f14 = frame("Frame 14", 170, 140, 120, 90, screen, null);
const d = btn("btn_d", 25, 35, f14, "#fb8c00");
const gD = figma.group([d], f14);

// e: hidden_box (fill, hidden) at (30, 270) -> btn_e (visible) at (20, 15)
const hb = frame("hidden_box", 30, 270, 120, 70, screen, "#cfd8dc");
btn("btn_e", 20, 15, hb, "#8e24aa");
hb.visible = false;

// hidden leaf and transparent leaf
const bh = btn("btn_hidden", 180, 280, screen, "#6d4c41");
bh.visible = false;
const bg = btn("btn_ghost", 180, 360, screen, "#00897b");
bg.opacity = 0;

// Report: for every node, its parent, Figma's own x/y (relative, as the API
// and the position panel give it) and its position on the screen.
const sx = screen.absoluteTransform[0][2], sy = screen.absoluteTransform[1][2];
const rows = [];
function walk(node, depth) {
  rows.push({
    name: node.name,
    type: node.type,
    parent: node.parent.name,
    x: Math.round(node.x * 100) / 100,
    y: Math.round(node.y * 100) / 100,
    screen_x: Math.round((node.absoluteTransform[0][2] - sx) * 100) / 100,
    screen_y: Math.round((node.absoluteTransform[1][2] - sy) * 100) / 100,
    visible: node.visible,
    opacity: node.opacity,
  });
  if ("children" in node) for (const c of node.children) walk(c, depth + 1);
}
for (const c of screen.children) walk(c, 1);

const report = JSON.stringify({ screen: screen.name, nodes: rows }, null, 2);
console.log(report);
figma.viewport.scrollAndZoomIntoView([screen]);
figma.showUI(
  "<p style='font:12px sans-serif'>Copy this report and send it with the XML:</p>" +
  "<textarea style='width:100%;height:85%;font:11px monospace'>" +
  report.replace(/&/g, "&amp;").replace(/</g, "&lt;") +
  "</textarea>",
  { width: 420, height: 520 }
);
