#include "secure_storage.h"
#include "nvs_flash.h"
#include "esp_err.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include <string.h>

nvs_handle_t SecureStorage::handle = 0;
bool SecureStorage::initialized = false;

bool SecureStorage::init() {
    if (initialized) return true;
    esp_err_t err = nvs_flash_init();
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_INITIALIZED) return false;
    err = nvs_open("secure", NVS_READWRITE, &handle);
    if (err != ESP_OK) return false;
    initialized = true;
    return true;
}

bool SecureStorage::writeString(const char* key, const char* value) {
    if (!initialized) return false;
    esp_err_t err = nvs_set_str(handle, key, value);
    if (err != ESP_OK) return false;
    nvs_commit(handle);
    return true;
}

bool SecureStorage::readString(const char* key, char* value, size_t maxSize) {
    if (!initialized) return false;
    esp_err_t err = nvs_get_str(handle, key, value, &maxSize);
    return err == ESP_OK;
}

bool SecureStorage::writeBlob(const char* key, const void* data, size_t len) {
    if (!initialized) return false;
    esp_err_t err = nvs_set_blob(handle, key, data, len);
    if (err != ESP_OK) return false;
    nvs_commit(handle);
    return true;
}

bool SecureStorage::readBlob(const char* key, void* data, size_t len) {
    if (!initialized) return false;
    size_t size = len;
    esp_err_t err = nvs_get_blob(handle, key, data, &size);
    return err == ESP_OK && size == len;
}