#include "face_renderer.h"
#include <math.h>

// Define LVGL image descriptors for eyes
static lv_img_dsc_t eye_open_img;
static lv_img_dsc_t eye_closed_img;

// Convert bitmap patterns to LVGL compatible format
static void init_eye_images() {
    static const uint8_t eye_open_data[] = {
        0x07, 0xFF, 0xFE,
        0x0F, 0xFF, 0xFC,
        0x1F, 0xFF, 0xF8,
        0x3F, 0xFF, 0xF0,
        0x3F, 0xFF, 0xF0,
        0x1F, 0xFF, 0xF8,
        0x0F, 0xFF, 0xFC,
        0x07, 0xFF, 0xFE
    };
    
    static const uint8_t eye_closed_data[] = {
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00,
        0x00, 0x00, 0x00
    };
    
    // Initialize LVGL image descriptors
    eye_open_img.data = eye_open_data;
    eye_open_img.data_size = sizeof(eye_open_data);
    eye_open_img.header.w = 24;
    eye_open_img.header.h = 8;
    eye_open_img.header.cf = LV_IMG_CF_RAW;
    
    eye_closed_img.data = eye_closed_data;
    eye_closed_img.data_size = sizeof(eye_closed_data);
    eye_closed_img.header.w = 24;
    eye_closed_img.header.h = 8;
    eye_closed_img.header.cf = LV_IMG_CF_RAW;
}

FaceRenderer::FaceRenderer(DisplayManager* display) : display(display) {
    // Create canvas for drawing
    static lv_color_t cbuf[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    
    canvas = lv_canvas_create(display->getScreen());
    lv_canvas_set_buffer(canvas, cbuf, DISPLAY_WIDTH, DISPLAY_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(canvas, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    
    // Initialize eye images
    init_eye_images();
}

void FaceRenderer::drawEye(int x, int y, EyeState state) {
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    
    if (state == EYE_OPEN) {
        // Draw open eye as a rounded rectangle
        rect_dsc.radius = EYE_HEIGHT / 2;
        rect_dsc.bg_color = lv_color_white();
        lv_canvas_draw_rect(canvas, x - EYE_WIDTH/2, y - EYE_HEIGHT/2, 
                           EYE_WIDTH, EYE_HEIGHT, &rect_dsc);
    } else {
        // Draw closed eye as a horizontal line
        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        line_dsc.color = lv_color_white();
        line_dsc.width = 2;
        
        lv_point_t points[2] = {
            {x - EYE_WIDTH/2, y},
            {x + EYE_WIDTH/2, y}
        };
        lv_canvas_draw_line(canvas, points, 2, &line_dsc);
    }
}

void FaceRenderer::drawEyes(EyeState left, EyeState right) {
    drawEye(LEFT_EYE_X, EYE_Y, left);
    drawEye(RIGHT_EYE_X, EYE_Y, right);
}

void FaceRenderer::drawMouth(MouthShape shape) {
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_white();
    line_dsc.width = 2;
    
    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.color = lv_color_white();
    arc_dsc.width = 2;
    
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.border_color = lv_color_white();
    rect_dsc.border_width = 2;
    rect_dsc.bg_opa = LV_OPA_TRANSP;
    
    switch (shape) {
        case MOUTH_SMILE: {
            // Draw smile arc (0-180 degrees)
            for (int angle = 0; angle < 180; angle += 10) {
                float rad = angle * M_PI / 180.0f;
                lv_point_t point = {
                    MOUTH_X + (int)(MOUTH_WIDTH * sin(rad)),
                    MOUTH_Y + (int)(MOUTH_HEIGHT * cos(rad))
                };
                lv_canvas_set_px(canvas, point.x, point.y, lv_color_white());
            }
            break;
        }
        case MOUTH_NEUTRAL: {
            // Draw neutral line
            lv_point_t points[2] = {
                {MOUTH_X - MOUTH_WIDTH/2, MOUTH_Y},
                {MOUTH_X + MOUTH_WIDTH/2, MOUTH_Y}
            };
            lv_canvas_draw_line(canvas, points, 2, &line_dsc);
            break;
        }
        case MOUTH_FROWN: {
            // Draw frown arc (180-360 degrees)
            for (int angle = 180; angle < 360; angle += 10) {
                float rad = angle * M_PI / 180.0f;
                lv_point_t point = {
                    MOUTH_X + (int)(MOUTH_WIDTH * sin(rad)),
                    MOUTH_Y + (int)(MOUTH_HEIGHT * cos(rad))
                };
                lv_canvas_set_px(canvas, point.x, point.y, lv_color_white());
            }
            break;
        }
        case MOUTH_WIDE: {
            // Draw wide mouth rectangle
            lv_canvas_draw_rect(canvas, MOUTH_X - 10, MOUTH_Y - 5, 20, 10, &rect_dsc);
            break;
        }
    }
}

void FaceRenderer::drawFace(FaceEmotion emotion) {
    display->clear();
    
    // Clear canvas with black
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);

    EyeState leftEye = EYE_OPEN;
    EyeState rightEye = EYE_OPEN;
    MouthShape mouth = MOUTH_NEUTRAL;

    switch (emotion) {
        case EMOTION_HAPPY:
            mouth = MOUTH_SMILE;
            break;
        case EMOTION_SAD:
            leftEye = EYE_CLOSED;
            rightEye = EYE_CLOSED;
            mouth = MOUTH_FROWN;
            break;
        case EMOTION_ANGRY:
            leftEye = EYE_CLOSED;
            rightEye = EYE_CLOSED;
            mouth = MOUTH_FROWN;
            break;
        case EMOTION_SURPRISED:
            mouth = MOUTH_WIDE;
            break;
        case EMOTION_SLEEPY:
            leftEye = EYE_CLOSED;
            mouth = MOUTH_FROWN;
            break;
        case EMOTION_NEUTRAL:
        default:
            mouth = MOUTH_SMILE;
            break;
    }

    drawEyes(leftEye, rightEye);
    drawMouth(mouth);
    display->update();
}

void FaceRenderer::drawEyes(EyeState left, EyeState right) {
    drawEye(LEFT_EYE_X, EYE_Y, left);
    drawEye(RIGHT_EYE_X, EYE_Y, right);
}

void FaceRenderer::drawMouth(MouthShape shape) {
    lv_draw_arc_dsc_t arc_dsc;
    lv_draw_arc_dsc_init(&arc_dsc);
    arc_dsc.color = lv_color_white();
    arc_dsc.width = 2;
    
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_white();
    line_dsc.width = 2;
    
    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.border_color = lv_color_white();
    rect_dsc.border_width = 2;
    rect_dsc.bg_opa = LV_OPA_TRANSP;
    
    switch (shape) {
        case MOUTH_SMILE:
            // Draw smile arc (0-180 degrees)
            lv_canvas_draw_arc(canvas, MOUTH_X, MOUTH_Y, MOUTH_WIDTH, 0, 180, &arc_dsc);
            break;
        case MOUTH_NEUTRAL:
            // Draw neutral line
            lv_canvas_draw_line(canvas, MOUTH_X - MOUTH_WIDTH/2, MOUTH_Y, 
                               MOUTH_X + MOUTH_WIDTH/2, MOUTH_Y, &line_dsc);
            break;
        case MOUTH_FROWN:
            // Draw frown arc (180-360 degrees)
            lv_canvas_draw_arc(canvas, MOUTH_X, MOUTH_Y, MOUTH_WIDTH, 180, 360, &arc_dsc);
            break;
        case MOUTH_WIDE:
            // Draw wide mouth rectangle
            lv_canvas_draw_rect(canvas, MOUTH_X - 10, MOUTH_Y - 5, 20, 10, &rect_dsc);
            break;
    }
}

void FaceRenderer::drawFace(FaceEmotion emotion) {
    display->clear();
    
    // Clear canvas with black
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);

    EyeState leftEye = EYE_OPEN;
    EyeState rightEye = EYE_OPEN;
    MouthShape mouth = MOUTH_NEUTRAL;

    switch (emotion) {
        case EMOTION_HAPPY:
            mouth = MOUTH_SMILE;
            break;
        case EMOTION_SAD:
            leftEye = EYE_CLOSED;
            rightEye = EYE_CLOSED;
            mouth = MOUTH_FROWN;
            break;
        case EMOTION_ANGRY:
            leftEye = EYE_CLOSED;
            rightEye = EYE_CLOSED;
            mouth = MOUTH_FROWN;
            break;
        case EMOTION_SURPRISED:
            mouth = MOUTH_WIDE;
            break;
        case EMOTION_SLEEPY:
            leftEye = EYE_CLOSED;
            mouth = MOUTH_FROWN;
            break;
        case EMOTION_NEUTRAL:
        default:
            mouth = MOUTH_SMILE;
            break;
    }

    drawEyes(leftEye, rightEye);
    drawMouth(mouth);
    display->update();
}