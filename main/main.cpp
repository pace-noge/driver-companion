#include "display_manager.h"
#include "secure_storage.h"
#include "ble_server.h"
#include "state_manager.h"
#include "mpu6050_sensor.h"

#include "emotion_engine.h"
#include <freertos/FreeRTOS.h>
#include "cJSON.h"

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
        // Parse JSON and trigger navigation using cJSON
        cJSON* doc = cJSON_ParseWithLength(packet, len);
        if (!doc) return;
        const cJSON* turnItem = cJSON_GetObjectItemCaseSensitive(doc, "turn");
        const cJSON* distanceItem = cJSON_GetObjectItemCaseSensitive(doc, "distance");
        const cJSON* streetItem = cJSON_GetObjectItemCaseSensitive(doc, "street");
        const char* turn = turnItem && cJSON_IsString(turnItem) ? turnItem->valuestring : "";
        int distance = distanceItem && cJSON_IsNumber(distanceItem) ? distanceItem->valueint : 0;
        const char* street = streetItem && cJSON_IsString(streetItem) ? streetItem->valuestring : "Unknown";
        stateManager.triggerNavigation(turn, distance, street, nullptr, 0);
        cJSON_Delete(doc);
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