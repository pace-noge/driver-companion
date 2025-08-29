#include "wifi_manager.h"
#include "secure_storage.h"
#include <esp_log.h>

static const char* TAG = "WiFiManager";

bool WiFiManager::connect(const char* ssid, const char* password) {
    char savedSsid[32] = {0};
    char savedPswd[64] = {0};

    if (SecureStorage::readString("wifi_ssid", savedSsid, sizeof(savedSsid)) &&
        SecureStorage::readString("wifi_pswd", savedPswd, sizeof(savedPswd))) {
        strcpy((char*)ssid, savedSsid);
        strcpy((char*)password, savedPswd);
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    if (waitForConnection(10000)) {
        ESP_LOGI(TAG, "Connected: %s", WiFi.localIP().toString().c_str());
        return true;
    } else {
        ESP_LOGE(TAG, "WiFi connect failed");
        return false;
    }
}

bool WiFiManager::isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiManager::disconnect() {
    WiFi.disconnect();
}

bool WiFiManager::waitForConnection(int timeoutMs) {
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
        delay(200);
    }
    return WiFi.status() == WL_CONNECTED;
}