#include "arrow_renderer.h"
#include "hardware_config.h"
#include <string.h>
#include <math.h>
#include <cstdio>

ArrowRenderer::ArrowRenderer(DisplayManager* display) : display(display) {
    // Create canvas for drawing
    static lv_color_t cbuf[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    canvas = lv_canvas_create(lv_scr_act());
    lv_canvas_set_buffer(canvas, cbuf, DISPLAY_WIDTH, DISPLAY_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(canvas, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
}

void ArrowRenderer::drawStreetName(const char* name) {
    // Create label for street name
    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, name);
    
    // Set label position
    int len = strlen(name);
    if (len > 12) {
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, 5, STREET_Y);
    } else {
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, STREET_Y);
    }
    
    // Set label style
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_color_white());
    lv_obj_add_style(label, &style, 0);
}

void ArrowRenderer::drawArrow(TurnType type) {
    display->clear();
    
    // Clear canvas with black
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    
    // Initialize drawing descriptors
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_white();
    line_dsc.width = 2;
    
    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.color = lv_color_white();
    arc_dsc.width = 2;

    switch (type) {
        case TURN_STRAIGHT: {
            lv_point_t points1[2] = {{64, 10}, {64, 20}};
            lv_point_t points2[2] = {{59, 15}, {64, 10}};
            lv_point_t points3[2] = {{69, 15}, {64, 10}};
            lv_canvas_draw_line(canvas, points1, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points2, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points3, 2, &line_dsc);
            break;
        }
        case TURN_RIGHT_90: {
            lv_point_t points1[2] = {{50, 20}, {50, 40}};
            lv_point_t points2[2] = {{50, 40}, {70, 40}};
            lv_point_t points3[2] = {{65, 35}, {70, 40}};
            lv_point_t points4[2] = {{65, 45}, {70, 40}};
            lv_canvas_draw_line(canvas, points1, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points2, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points3, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points4, 2, &line_dsc);
            break;
        }
        case KEEP_RIGHT: {
            lv_point_t points1[2] = {{50, 30}, {60, 30}};
            lv_point_t points2[2] = {{60, 30}, {70, 35}};
            lv_point_t points3[2] = {{65, 30}, {70, 35}};
            lv_point_t points4[2] = {{65, 40}, {70, 35}};
            lv_canvas_draw_line(canvas, points1, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points2, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points3, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points4, 2, &line_dsc);
            break;
        }
        case UTURN_MEDIAN: {
            for (int angle = 0; angle < 180; angle += 5) {
                lv_canvas_draw_arc(canvas, 64, 32, 15, angle, angle + 5, &arc_dsc);
            }
            lv_point_t points1[2] = {{49, 32}, {49, 40}};
            lv_point_t points2[2] = {{79, 32}, {79, 40}};
            lv_canvas_draw_line(canvas, points1, 2, &line_dsc);
            lv_canvas_draw_line(canvas, points2, 2, &line_dsc);
            break;
        }
        case ROUNDABOUT_EXIT: {
            for (int angle = 0; angle < 360; angle += 10) {
                lv_canvas_draw_arc(canvas, 64, 32, 12, angle, angle + 10, &arc_dsc);
            }
            lv_point_t points[2] = {{64, 20}, {64, 10}};
            lv_canvas_draw_line(canvas, points, 2, &line_dsc);
            break;
        }
    }
}

void ArrowRenderer::drawDistance(int distance) {
    char buf[16];
    sprintf(buf, "in %dm", distance);
    
    // Create label for distance
    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, buf);
    
    // Set label position
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, DISTANCE_Y);
    
    // Set label style
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_text_font(&style, &lv_font_montserrat_14);
    lv_obj_add_style(label, &style, 0);
}

void ArrowRenderer::drawLaneIndicator(bool activeLanes[4], int count) {
    int radius = 4;
    int spacing = 12;
    int startX = (128 - (count - 1) * spacing) / 2;
    int y = LANE_Y;

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.bg_color = lv_color_white();
    
    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.color = lv_color_white();
    arc_dsc.width = 2;

    for (int i = 0; i < count; i++) {
        int x = startX + i * spacing;
        if (activeLanes[i]) {
            // Draw filled circle
            rect_dsc.radius = radius;
            lv_canvas_draw_rect(canvas, x - radius, y - radius, radius * 2, radius * 2, &rect_dsc);
        } else {
            // Draw empty circle
            for (int angle = 0; angle < 360; angle += 20) {
                lv_canvas_draw_arc(canvas, x, y, radius, angle, angle + 20, &arc_dsc);
            }
        }
    }
}

// ...existing code...