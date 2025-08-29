#ifndef ARROW_RENDERER_H
#define ARROW_RENDERER_H

#include "display_manager.h"

enum TurnType {
    TURN_STRAIGHT,
    TURN_RIGHT_90,
    KEEP_RIGHT,
    UTURN_MEDIAN,
    ROUNDABOUT_EXIT
};

class ArrowRenderer {
public:
    ArrowRenderer(DisplayManager* display);

    void drawArrow(TurnType type);
    void drawDistance(int distance);
    void drawLaneIndicator(bool activeLanes[4], int count);
    void drawStreetName(const char* name);

    static constexpr int DISTANCE_Y = 44;
    static constexpr int LANE_Y = 52;
    static constexpr int STREET_Y = 62;

private:
    DisplayManager* display;
    lv_obj_t* canvas;
};
#endif

