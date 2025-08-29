#include "state_manager.h"
#include <string.h>

StateManager::StateManager(
    DisplayManager* display,
    FaceRenderer* face,
    ArrowRenderer* arrow,
    SecureBLEServer* bleServer
) :
    display(display),
    face(face),
    arrow(arrow),
    bleServer(bleServer),
    currentMode(DISPLAY_IDLE),
    navigationActive(false),
    navigationStartTime(0),
    gyroStartTime(0)
{}

void StateManager::begin() {
    currentMode = DISPLAY_IDLE;
    navigationActive = false;
    navigationStartTime = 0;
    gyroStartTime = 0;
    switchToMode(DISPLAY_IDLE);
}

void StateManager::switchToMode(DisplayMode mode) {
    if (currentMode == mode) return;
    display->fadeOut();
    currentMode = mode;
    switch (currentMode) {
        case DISPLAY_NAVIGATION: updateNavigation(); break;
        case DISPLAY_GYRO_REACTION: updateGyroReaction(); break;
        case DISPLAY_EMOTION: updateEmotion(); break;
        case DISPLAY_IDLE: updateIdle(); break;
    }
    display->fadeIn();
}

void StateManager::update() {
    unsigned long now = millis();
    if (navigationActive && now - navigationStartTime < NAVIGATION_DURATION_MS) {
        if (currentMode != DISPLAY_NAVIGATION) switchToMode(DISPLAY_NAVIGATION);
        return;
    } else if (navigationActive) {
        navigationActive = false;
    }
    if (currentMode == DISPLAY_GYRO_REACTION && now - gyroStartTime < GYRO_REACTION_DURATION_MS) {
        return;
    } else if (currentMode == DISPLAY_GYRO_REACTION) {
        switchToMode(DISPLAY_EMOTION);
        return;
    }
    if (currentMode == DISPLAY_NAVIGATION && !navigationActive) {
        switchToMode(DISPLAY_EMOTION);
        return;
    }
    switchToMode(DISPLAY_EMOTION);
}

void StateManager::triggerNavigation(
    const char* turnType,
    int distance,
    const char* streetName,
    bool activeLanes[4],
    int laneCount
) {
    navigationActive = true;
    navigationStartTime = millis();
    if (currentMode != DISPLAY_NAVIGATION) switchToMode(DISPLAY_NAVIGATION);
}

void StateManager::triggerGyroReaction() {
    gyroStartTime = millis();
    if (currentMode != DISPLAY_GYRO_REACTION) switchToMode(DISPLAY_GYRO_REACTION);
}

void StateManager::clearNavigation() {
    navigationActive = false;
}

DisplayMode StateManager::getCurrentMode() const {
    return currentMode;
}

bool StateManager::isNavigationActive() const {
    return navigationActive;
}

void StateManager::updateNavigation() {
    display->clear();
    arrow->drawArrow(TURN_STRAIGHT);
    arrow->drawDistance(100);
    bool lanes[4] = {false, true, false, false};
    arrow->drawLaneIndicator(lanes, 4);
    arrow->drawStreetName("Jl. Sudirman");
    display->update();
}

void StateManager::updateGyroReaction() {
    display->clear();
    face->drawFace(EMOTION_SURPRISED);
    display->update();
}

void StateManager::updateEmotion() {
    static unsigned long lastUpdate = 0;
    static int emotionState = 0;
    if (millis() - lastUpdate > 3000) {
        FaceEmotion emotion = EMOTION_NEUTRAL;
        switch (emotionState % 4) {
            case 0: emotion = EMOTION_HAPPY; break;
            case 1: emotion = EMOTION_SAD; break;
            case 2: emotion = EMOTION_ANGRY; break;
            case 3: emotion = EMOTION_NEUTRAL; break;
        }
        emotionState++;
        lastUpdate = millis();
        display->clear();
        face->drawFace(emotion);
        display->update();
    }
}

void StateManager::updateIdle() {
    display->clear();
    face->drawFace(EMOTION_SLEEPY);
    display->update();
}