#ifndef SECURE_BLE_SERVER_H
#define SECURE_BLE_SERVER_H

#include <functional>
#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>
#include <esp_bt_defs.h>

class SecureBLEServer {
public:
    using DataCallback = std::function<void(const char* data, size_t len)>;

    SecureBLEServer();
    void begin();
    void stop();
    bool isConnected() const;
    void setDataCallback(DataCallback cb);

private:
    bool connected;
    DataCallback dataCallback;

    // GATT Server
    esp_gatt_if_t gatt_interface;
    uint16_t service_handle;
    uint16_t char_rx_handle;
    uint16_t char_tx_handle;
    uint16_t conn_id;
    esp_ble_adv_data_t adv_data;
    esp_ble_adv_params_t adv_params;
    esp_bt_uuid_t char_uuid_rx; // Add this if you use it

    // UUIDs
    static constexpr uint16_t SERVICE_UUID = 0x180F;
    static constexpr uint16_t CHAR_RX_UUID = 0x2A19;
    static constexpr uint16_t CHAR_TX_UUID = 0x2A1A;

    // Event handlers
    static void gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param);
    static void gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gatts_cb_param_t* param);

    void setupService();
    void sendResponse();
};

#endif // SECURE_BLE_SERVER_H