# figma2lvgl — Figma Design Guide

## Who This Is For

This guide is for UI designers using Figma to design screens that will be
rendered on an embedded LVGL display. figma2lvgl reads your Figma XML export
and generates the C code — you design normally in Figma, the tool handles
the translation.

---

## The Embedded Lens

Figma is designed for apps and web. Embedded displays are different:

| Figma / web | Embedded LVGL |
|-------------|--------------|
| Responsive layout | Fixed pixel size |
| Any font | Montserrat (10–24pt) or custom |
| Gradients, shadows | Flat color only |
| Hover states | No cursor |
| Scroll physics | Simple scroll or none |
| Dynamic content | Fixed structure + runtime updates |

figma2lvgl extracts what LVGL can represent and ignores the rest. **Design
naturally in Figma — the tool applies the embedded lens automatically.**

---

## Screen Setup

Each top-level **Frame** in your Figma page becomes one screen in LVGL.

- Set the frame size to match your display resolution (e.g. 320×480)
- Name the frame after the screen: `home`, `settings`, `splash`
- Frame name → screen name → `ui_home_init()`, `ui_home_load()`

---

## Naming Conventions

The tool reads your **layer names** to determine what LVGL widget to generate.
These are the only naming rules you need to follow.

### Required prefixes for interactive widgets

| What you're designing | Name it | Example |
|----------------------|---------|---------|
| Button | starts with `btn_` | `btn_ok`, `btn_settings`, `btn_cancel` |
| Slider | starts with `slider_` or ends with `_slider` | `brightness_slider`, `slider_volume` |
| Dynamic list/grid | starts with `list_` or `grid_` | `list_devices`, `grid_icons` |

### Automatic detection (no prefix needed)

| What you're designing | Figma element | Detected by |
|----------------------|---------------|-------------|
| Label / text | Text layer | Figma node type (`Text`) |
| Bar / progress | Rectangle | name contains `bar` |
| Image / icon | Any frame | name contains `icon` or `image` |
| Container / panel | Frame with fill | has visual style + meaningful name |

### Names are case-insensitive

`btn_OK`, `Btn_Ok`, `BTN_OK` all work. The generated C identifier is
always normalized to `snake_case`.

---

## Designing Each Widget

### Text / Label

- Use Figma's **Text tool** (T)
- Set font size, color, alignment in the right panel
- Name it anything: `time`, `temperature`, `status_message`
- The text you type in Figma becomes the **initial displayed value** in the
  generated code. Your firmware can update it at runtime.

```
Figma:   Text layer "time" with content "16:30"
LVGL:    lv_label_create() showing "16:30" on first render
Firmware: ui_home_panel_top_time_set_text("17:45");
```

### Bar / Progress

- Use a **Rectangle** (R)
- Name it with `bar` anywhere: `battery_bar`, `signal_bar`, `progress_bar`
- Set fill color — this becomes the filled (indicator) color in LVGL
- Set corner radius for rounded bars
- Default range: 0–100. To set custom range, append it to the name:
  `temperature_bar_n20_50` → min=-20, max=50 (prefix `n` = negative)

```
Figma:   Rectangle "battery_bar" with green fill, radius 4
LVGL:    lv_bar_create(), range 0-100, green indicator
Firmware: ui_home_battery_bar_set_value(75, 300); /* animated */
```

### Image / Icon

- Use any frame or component instance
- Name it with `icon` or `image`: `icon_wifi`, `image_logo`
- Place a matching PNG in your images folder: `icon_wifi.png`
- The PNG filename must match the Figma layer name exactly

```
Figma:   Frame "icon_wifi" (48×48)
PNG:     icon_wifi.png in your images folder
LVGL:    lv_image_create()
Firmware: ui_home_panel_top_display_icon_wifi();
```

### Button

- Use a **Frame** (F) — not a rectangle
- Name it `btn_something`: `btn_ok`, `btn_back`, `btn_confirm`
- Set fill color, corner radius, border on the frame — these style the button
- **Add a Text layer inside the frame** for the button label
  - The text content becomes the button label in LVGL automatically
  - Name the text layer anything (e.g. `label`)
  - If no text layer: the tool derives label from the button name
    (`btn_ok` → "Ok")
- The button generates an event callback you override in firmware

```
Figma:   Frame "btn_ok" (120×44, blue fill, radius 8)
           └── Text "label" "Ok" (white, centered)
LVGL:    lv_button_create() + lv_label_create("Ok") inside
Firmware: void ui_home_on_btn_ok(lv_event_t *e) { /* your code */ }
```

### Slider

- Use a **Rectangle** or thin **Frame**
- Name it `slider_something` or `something_slider`
- Make it wide and short (horizontal slider) or tall and narrow (vertical)
- Set fill color — styles the track
- Custom range in name: `brightness_slider_0_255` → min=0, max=255

```
Figma:   Rectangle "brightness_slider" (240×20, gray fill)
LVGL:    lv_slider_create(), range 0-100 default
Firmware: void ui_home_on_brightness_slider(lv_event_t *e) {
              int32_t val = lv_slider_get_value(lv_event_get_target(e));
          }
```

### Container / Panel

- Use a **Frame** for grouping related elements
- Give it a meaningful name: `panel_top`, `panel_controls`, `header`
- Set fill color and/or border if you want the panel to be visible
- Nest your widgets inside — the tool recurses into them
- No special prefix needed — if the frame has visual properties or a
  meaningful name, it becomes an `lv_obj` container in LVGL

```
Figma:   Frame "panel_top" (320×80, dark fill)
           ├── Text "time" "16:30"
           └── Frame "icon_wifi"
LVGL:    lv_obj_create() panel, time label inside, wifi image inside
Firmware: ui_home_panel_top_time_set_text("17:45");
```

### Dynamic List / Grid

- Use a **Frame** for the container
- Name it `list_something` or `grid_something`
- The tool generates only the container — your firmware fills it at runtime
- Design one list item in Figma for reference (the tool ignores it)

```
Figma:   Frame "list_devices" (320×300)
           └── [item frames — ignored by tool, for design reference only]
LVGL:    lv_obj_create() — container only
Firmware: lv_obj_t *list = ui_home_get_list_devices();
          for (int i = 0; i < device_count; i++) {
              lv_obj_t *item = lv_obj_create(list);
              /* populate item */
          }
```

---

## What Gets Ignored (By Design)

The tool silently skips these — no error, no warning:

| Figma feature | Reason ignored |
|--------------|----------------|
| Gradients | No LVGL equivalent for embedded |
| Drop shadows | No LVGL equivalent |
| Blur effects | No LVGL equivalent |
| Hover / pressed states | LVGL handles these internally via themes |
| Prototype interactions (arrows) | Not in FigML XML export |
| Responsive constraints | All positioning is absolute pixel coordinates |
| Text alignment (horizontal) | FigML doesn't export this attribute |

---

## Structural Frames (Invisible Grouping)

If you use a Frame purely for grouping in Figma with **no fill, no border,
and no specific name**, the tool drops it from the output and promotes its
children up to the parent level. This is transparent to you — the generated
code behaves as if the grouping frame wasn't there.

If you want the frame to become an LVGL container, give it either a fill
color or a meaningful name.

---

## Nesting Depth

The tool supports nesting up to 5 levels deep. Beyond that, a warning is
emitted in the build log. Screens deeper than 7 levels are skipped with an
error.

For embedded UIs, aim for 2–3 levels. Deeper nesting:
- Generates longer function names (`ui_home_a_b_c_d_set_text`)
- Increases LVGL layout recalculation overhead
- Usually signals that the design should be split into multiple screens

---

## Exporting from Figma

1. Install the **FigML — Figma XML Exporter** plugin
2. Right-click your Frame in Figma
3. Plugins → FigML → Export
4. Save the `.xml` file
5. Run: `figma2lvgl -x your_layout.xml`

---

## Summary — Quick Reference

| What you want | Do this in Figma |
|---------------|-----------------|
| A screen | Top-level Frame, named after the screen |
| A text label | Text layer, any name |
| A progress bar | Rectangle, name contains `bar` |
| An icon | Frame/Instance, name contains `icon` or `image` |
| A button | Frame named `btn_*`, Text child inside for label |
| A slider | Rectangle named `slider_*` or `*_slider` |
| A grouping panel | Frame with fill, meaningful name |
| A runtime-filled list | Frame named `list_*` or `grid_*` |
| Pure visual grouping | Unnamed Frame, no fill — becomes transparent |
