#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "lvgl.h"

class DisplayManager {
public:
    bool init();
    void clear();
    void update();
    void fadeIn();
    void fadeOut();

    lv_disp_t* getDisplay() const { return disp; }

private:
    lv_disp_t* disp;
    lv_obj_t* fade_overlay;  // Black overlay for fade effect

    static void fadeAnimCallback(void* var, int32_t v);
};

#endif