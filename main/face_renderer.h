#ifndef FACE_RENDERER_H
#define FACE_RENDERER_H

#include "display_manager.h"

enum EyeState {
    EYE_OPEN,
    EYE_CLOSED
};

enum MouthShape {
    MOUTH_SMILE,
    MOUTH_NEUTRAL,
    MOUTH_FROWN,
    MOUTH_WIDE
};

enum FaceEmotion {
    EMOTION_NEUTRAL,
    EMOTION_HAPPY,
    EMOTION_SAD,
    EMOTION_ANGRY,
    EMOTION_SURPRISED,
    EMOTION_SLEEPY
};

class FaceRenderer {
public:
    FaceRenderer(DisplayManager* display);

    void drawFace(FaceEmotion emotion);
    void drawEyes(EyeState left, EyeState right);
    void drawMouth(MouthShape shape);
    
    DisplayManager* getDisplayManager() const { return display; }

    static constexpr int LEFT_EYE_X = 30;
    static constexpr int RIGHT_EYE_X = 90;
    static constexpr int EYE_Y = 20;
    static constexpr int EYE_WIDTH = 12;
    static constexpr int EYE_HEIGHT = 8;

    static constexpr int MOUTH_X = 64;
    static constexpr int MOUTH_Y = 40;
    static constexpr int MOUTH_WIDTH = 8;
    static constexpr int MOUTH_HEIGHT = 4;

private:
    DisplayManager* display;
    lv_obj_t* canvas;

    void drawEye(int x, int y, EyeState state);
};

#endif