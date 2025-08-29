#include "display_manager.h"
#include "secure_storage.h"
#include "ble_server.h"
#include "state_manager.h"
#include "mpu6050_sensor.h"
#include "emotion_engine.h"
#include <freertos/FreeRTOS.h>

extern "C" void app_main() {
    // Initialize NVS
    if (!SecureStorage::init()) {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    // Initialize display
    DisplayManager display;
    if (!display.init()) {
        while (true) {
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }

    // Initialize MPU6050 sensor
    MPU6050Sensor mpuSensor;
    if (!mpuSensor.begin()) {
        lv_obj_t* label = lv_label_create(lv_scr_act());
        lv_label_set_text(label, "MPU6050 FAIL");
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
        while (true) {
            lv_timer_handler();
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    // Create face and arrow renderers
    FaceRenderer face(&display);
    ArrowRenderer arrow(&display);

    // Initialize state and emotion engine
    StateManager stateManager(&display, &face, &arrow, nullptr);
    EmotionEngine emotionEngine(&face, &mpuSensor, &stateManager);

    // Initialize BLE
    SecureBLEServer bleServer;
    bleServer.setDataCallback([&stateManager](const char* packet, size_t len) {
        StaticJsonDocument<256> doc;
        DeserializationError err = deserializeJson(doc, packet);
        if (err) return;

        const char* turn = doc["turn"];
        int distance = doc["distance"] | 0;
        const char* street = doc["street"] | "Unknown";

        bool lanes[4] = {false};
        int laneCount = 0;
        for (int i = 0; i < 4; i++) {
            char key[8];
            sprintf(key, "lane%d", i);
            lanes[i] = doc[key] | false;
            if (lanes[i]) laneCount = i + 1;
        }

        stateManager.triggerNavigation(turn, distance, street, lanes, laneCount);
    });
    bleServer.begin();

    // Start state and emotion engine
    stateManager.begin();
    emotionEngine.begin();

    // Main loop
    while (true) {
        static unsigned long lastUpdate = 0;
        unsigned long now = xTaskGetTickCount() * portTICK_PERIOD_MS;
        if (now - lastUpdate >= 16) {
            stateManager.update();
            emotionEngine.update();
            lastUpdate = now;
        }
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}