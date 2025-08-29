#include "emotion_engine.h"
#include <math.h>

EmotionEngine::EmotionEngine(
    FaceRenderer* face,
    MPU6050Sensor* sensor,
    StateManager* state
) :
    face(face),
    sensor(sensor),
    state(state),
    display(face->getDisplayManager()),
    currentState(DRIVING_IDLE),
    stateEnterTime(0),
    lastBlinkTime(0),
    leftBlink(true)
{}

void EmotionEngine::begin() {
    currentState = DRIVING_IDLE;
    stateEnterTime = millis();
    lastBlinkTime = millis() + random(BLINK_INTERVAL_MIN, BLINK_INTERVAL_MAX);
    leftBlink = true;
}

void EmotionEngine::update() {
    sensor->update();
    updateDrivingState();
    updateFacialExpression();
    updateMicroExpressions();
}

void EmotionEngine::updateDrivingState() {
    float ax = sensor->getAccelX();
    float gz = sensor->getGyroZ();
    float speed = sensor->getSpeedEstimate();
    unsigned long now = millis();
    unsigned long stateDuration = now - stateEnterTime;

    if (ax < BRAKE_THRESHOLD && currentState != DRIVING_HARD_BRAKE) {
        if (stateDuration > 200) {
            currentState = DRIVING_HARD_BRAKE;
            stateEnterTime = now;
            state->triggerGyroReaction();
        }
        return;
    }
    if (currentState == DRIVING_HARD_BRAKE && stateDuration > 500) {
        currentState = DRIVING_NORMAL;
        stateEnterTime = now;
        return;
    }
    if (abs(gz) > DRIFT_THRESHOLD && currentState != DRIVING_DRIFTING && stateDuration > 300) {
        currentState = DRIVING_DRIFTING;
        stateEnterTime = now;
        return;
    }
    if (currentState == DRIVING_DRIFTING && abs(gz) < 0.3f) {
        currentState = DRIVING_NORMAL;
        stateEnterTime = now;
        return;
    }
    if (speed > SPEED_THRESHOLD && currentState != DRIVING_SPEEDING && stateDuration > 1000) {
        currentState = DRIVING_SPEEDING;
        stateEnterTime = now;
        return;
    }
    if (speed < SPEED_THRESHOLD - 0.2f && currentState == DRIVING_SPEEDING) {
        currentState = DRIVING_NORMAL;
        stateEnterTime = now;
        return;
    }
    if (speed < 0.2f && currentState != DRIVING_IDLE) {
        currentState = DRIVING_IDLE;
        stateEnterTime = now;
        return;
    }
    if (speed > 0.3f && currentState == DRIVING_IDLE) {
        currentState = DRIVING_NORMAL;
        stateEnterTime = now;
        return;
    }
    if (currentState != DRIVING_NORMAL) {
        currentState = DRIVING_NORMAL;
        stateEnterTime = now;
    }
}

void EmotionEngine::updateFacialExpression() {
    FaceEmotion emotion = EMOTION_NEUTRAL;
    switch (currentState) {
        case DRIVING_SPEEDING: emotion = EMOTION_ANGRY; break;
        case DRIVING_SLOW: emotion = EMOTION_SLEEPY; break;
        case DRIVING_IDLE: emotion = EMOTION_SLEEPY; break;
        case DRIVING_HARD_BRAKE: emotion = EMOTION_SURPRISED; break;
        case DRIVING_DRIFTING: emotion = EMOTION_HAPPY; break;
        case DRIVING_HIGH_SPIRIT: emotion = EMOTION_HAPPY; break;
        case DRIVING_NORMAL: default: emotion = EMOTION_NEUTRAL; break;
    }
    face->drawFace(emotion);
}

void EmotionEngine::updateMicroExpressions() {
    unsigned long now = millis();
    if (state->getCurrentMode() != DISPLAY_EMOTION && state->getCurrentMode() != DISPLAY_IDLE) return;
    if (now - lastBlinkTime > random(BLINK_INTERVAL_MIN, BLINK_INTERVAL_MAX)) {
        triggerBlink();
        lastBlinkTime = now;
    }
}

void EmotionEngine::triggerBlink() {
    leftBlink = !leftBlink;
    EyeState left = leftBlink ? EYE_OPEN : EYE_CLOSED;
    EyeState right = leftBlink ? EYE_CLOSED : EYE_OPEN;
    face->drawEyes(left, right);
    face->drawMouth(MOUTH_NEUTRAL);
    display->update();
    delay(150);
    face->drawEyes(EYE_OPEN, EYE_OPEN);
    face->drawMouth(MOUTH_NEUTRAL);
    display->update();
}
}