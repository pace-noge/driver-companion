#ifndef EMOTION_ENGINE_H
#define EMOTION_ENGINE_H

#include "face_renderer.h"
#include "mpu6050_sensor.h"
#include "state_manager.h"

enum DrivingState {
    DRIVING_NORMAL,
    DRIVING_SPEEDING,
    DRIVING_SLOW,
    DRIVING_IDLE,
    DRIVING_HARD_BRAKE,
    DRIVING_DRIFTING,
    DRIVING_HIGH_SPIRIT
};

class EmotionEngine {
public:
    EmotionEngine(
        FaceRenderer* face,
        MPU6050Sensor* sensor,
        StateManager* state
    );

    void begin();
    void update();

    DrivingState getCurrentState() const;

private:
    FaceRenderer* face;
    MPU6050Sensor* sensor;
    StateManager* state;
    DisplayManager* display;

    DrivingState currentState;
    unsigned long stateEnterTime;
    unsigned long lastBlinkTime;
    bool leftBlink;

    static constexpr float SPEED_THRESHOLD = 1.5f;
    static constexpr float BRAKE_THRESHOLD = -0.8f;
    static constexpr float DRIFT_THRESHOLD = 1.2f;
    static constexpr unsigned long BLINK_INTERVAL_MIN = 3000;
    static constexpr unsigned long BLINK_INTERVAL_MAX = 8000;

    void updateDrivingState();
    void updateFacialExpression();
    void updateMicroExpressions();
    void triggerBlink();
};

#endif