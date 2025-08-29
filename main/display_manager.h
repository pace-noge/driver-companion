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