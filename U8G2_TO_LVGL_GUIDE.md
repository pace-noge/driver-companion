# Converting from U8G2 to LVGL

This guide outlines the steps needed to convert your ESP32 display project from U8G2 to LVGL.

## 1. Update Dependencies

In your `main/idf_component.yml` file, remove the U8G2 dependency and ensure you have LVGL and the ESP LVGL port:

```yaml
dependencies:
  # ...existing dependencies...
  lvgl/lvgl: ^9.3.0
  espressif/esp_lvgl_port: ^1.1.0
```

## 2. Update Display Manager

The `DisplayManager` class should be updated to use LVGL and provide access to LVGL objects:

```cpp
// display_manager.h
#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "lvgl.h"

class DisplayManager {
public:
    bool init();
    void clear();
    void update();

    lv_disp_t* getDisplay() const { return disp; }
    lv_obj_t* getScreen() const { return lv_scr_act(); }

private:
    lv_disp_t* disp;
};
#endif
```

## 3. Update Renderer Classes

### Face Renderer

Update the header file:
```cpp
// face_renderer.h
private:
    DisplayManager* display;
    lv_obj_t* canvas;  // instead of u8g2_t* u8g2
```

Update the implementation:
```cpp
// face_renderer.cpp
FaceRenderer::FaceRenderer(DisplayManager* display) : display(display) {
    // Create a canvas for drawing
    canvas = lv_canvas_create(display->getScreen());
    lv_canvas_set_buffer(canvas, canvas_buf, DISPLAY_WIDTH, DISPLAY_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_center(canvas);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
}

// Replace U8G2 drawing functions with LVGL equivalents:
// u8g2_DrawLine -> lv_canvas_draw_line
// u8g2_DrawArc -> lv_canvas_draw_arc
// u8g2_DrawFrame -> lv_canvas_draw_rect
// etc.
```

### Arrow Renderer

Follow the same pattern as Face Renderer.

## 4. Update OTA Updater

The OTA updater should use LVGL drawing functions:

```cpp
// Instead of:
u8g2_SetFont(display.getU8g2(), u8g2_font_6x10_t_mf);
u8g2_DrawStr(display.getU8g2(), 10, 20, "Updating...");

// Use:
lv_obj_t* label = lv_label_create(display->getScreen());
lv_label_set_text(label, "Updating...");
lv_obj_align(label, LV_ALIGN_TOP_LEFT, 10, 20);
```

## 5. LVGL Drawing Reference

Here's a cheat sheet for converting U8G2 drawing calls to LVGL:

| U8G2 | LVGL |
|------|------|
| u8g2_DrawLine | lv_canvas_draw_line |
| u8g2_DrawArc | lv_canvas_draw_arc |
| u8g2_DrawFrame | lv_canvas_draw_rect (with border) |
| u8g2_DrawStr | lv_canvas_draw_text or lv_label_create |
| u8g2_DrawBitmap | lv_canvas_draw_img |

## 6. Additional LVGL Configuration

You may need to add or modify LVGL configuration in your project. The ESP LVGL port component should handle most of the integration, but you might need to:

1. Ensure you're calling `lv_timer_handler()` in your main loop
2. Set up proper memory allocation for LVGL
3. Configure input devices if needed

## 7. Testing

After converting your code:

1. Build the project: `idf.py build`
2. Flash to your device: `idf.py -p [PORT] flash`
3. Monitor output: `idf.py -p [PORT] monitor`
