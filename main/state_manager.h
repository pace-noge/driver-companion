#ifndef STATE_MANAGER_H
#define STATE_MANAGER_H

#include "display_manager.h"
#include "face_renderer.h"
#include "arrow_renderer.h"
#include "ble_server.h"
#include "hardware_config.h"

enum DisplayMode {
    DISPLAY_NAVIGATION,
    DISPLAY_GYRO_REACTION,
    DISPLAY_EMOTION,
    DISPLAY_IDLE
};

class StateManager {
public:
    StateManager(
        DisplayManager* display,
        FaceRenderer* face,
        ArrowRenderer* arrow,
        SecureBLEServer* bleServer
    );

    void begin();
    void update();

    void triggerNavigation(
        const char* turnType,
        int distance,
        const char* streetName,
        bool activeLanes[4],
        int laneCount
    );
    void triggerGyroReaction();
    void clearNavigation();

    DisplayMode getCurrentMode() const;
    bool isNavigationActive() const;

private:
    DisplayManager* display;
    FaceRenderer* face;
    ArrowRenderer* arrow;
    SecureBLEServer* bleServer;

    DisplayMode currentMode;
    bool navigationActive;
    unsigned long navigationStartTime;
    unsigned long gyroStartTime;

    static constexpr unsigned long NAVIGATION_DURATION_MS = 3000;
    static constexpr unsigned long GYRO_REACTION_DURATION_MS = 200;

    void switchToMode(DisplayMode mode);
    void updateNavigation();
    void updateGyroReaction();
    void updateEmotion();
    void updateIdle();
};

#endif