#include "ble_server.h"
#include <esp_log.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_gap_ble_api.h>
#include <esp_gatts_api.h>
#include <esp_bt_defs.h>
#include <string>

static const char* TAG = "SecureBLEServer";

// Forward declarations
static SecureBLEServer* g_instance = nullptr;

SecureBLEServer::SecureBLEServer()
    : connected(false)
    , dataCallback(nullptr)
    , gatt_interface(ESP_GATT_IF_NONE)
    , service_handle(0)
    , char_rx_handle(0)
    , char_tx_handle(0)
    , conn_id(0)
{
    g_instance = this;
}

void SecureBLEServer::gapEventHandler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
    switch (event) {
        case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
            esp_ble_gap_start_advertising(&g_instance->adv_params);
            break;
        default:
            break;
    }
}

void SecureBLEServer::gattsEventHandler(esp_gatts_cb_event_t event, esp_gatt_if_t gattc_if, esp_ble_gatts_cb_param_t* param) {
    switch (event) {
        case ESP_GATTS_REG_EVT:
            g_instance->gatt_interface = gattc_if;
            g_instance->setupService();
            break;

        case ESP_GATTS_CREATE_EVT:
            g_instance->service_handle = param->create.service_handle;
            esp_ble_gatts_start_service(g_instance->service_handle);
            break;

        case ESP_GATTS_START_EVT:
            esp_ble_gap_set_device_name("DriveCompanion.id");
            esp_ble_gap_config_adv_data(&g_instance->adv_data);
            break;

        case ESP_GATTS_CONNECT_EVT:
            g_instance->connected = true;
            g_instance->conn_id = param->connect.conn_id;
            esp_ble_gatts_send_indicate(gattc_if, g_instance->conn_id, g_instance->char_tx_handle, 0, nullptr, false);
            break;

        case ESP_GATTS_DISCONNECT_EVT:
            g_instance->connected = false;
            esp_ble_gap_start_advertising(&g_instance->adv_params);
            break;

        case ESP_GATTS_WRITE_EVT:
            if (param->write.handle == g_instance->char_rx_handle && g_instance->dataCallback) {
                std::string data((char*)param->write.value, param->write.len);
                g_instance->dataCallback(data.c_str(), data.length());
            }
            break;

        default:
            break;
    }
}

void SecureBLEServer::setupService() {
    // Create service
    char_uuid_rx.len = ESP_UUID_LEN_16;
    char_uuid_rx.uuid.uuid16 = CHAR_RX_UUID;

    esp_ble_gatts_add_char(
        service_handle,
        &char_uuid_rx,
        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
        ESP_GATT_CHAR_PROP_BIT_WRITE,
        nullptr, nullptr
    );

    // Add TX characteristic
    esp_bt_uuid_t char_uuid_tx = {
        .len = ESP_UUID_LEN_16,
        .uuid = {.uuid16 = CHAR_TX_UUID}
    };
    esp_ble_gatts_add_char(
        service_handle,
        &char_uuid_tx,
        ESP_GATT_PERM_READ,
        ESP_GATT_CHAR_PROP_BIT_NOTIFY,
        nullptr, nullptr
    );
}

void SecureBLEServer::begin() {
    esp_err_t ret = esp_ble_gatts_register_callback(gattsEventHandler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GATTS register failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gap_register_callback(gapEventHandler);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "GAP register failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_ble_gatts_app_register(0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "App register failed: %s", esp_err_to_name(ret));
        return;
    }

    // Set advertising data
    adv_data.set_scan_rsp = false;
    adv_data.include_name = true;
    adv_data.include_txpower = true;
    adv_data.min_interval = 0x20;
    adv_data.max_interval = 0x40;
    adv_data.appearance = 0x00;
    adv_data.manufacturer_len = 0;
    adv_data.p_manufacturer_data = nullptr;
    adv_data.service_data_len = 0;
    adv_data.p_service_data = nullptr;
    adv_data.service_uuid_len = 0;
    adv_data.p_service_uuid = nullptr;
    adv_data.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);

    // Set advertising parameters
    adv_params.adv_int_min = 0x20;
    adv_params.adv_int_max = 0x40;
    adv_params.adv_type = ADV_TYPE_IND;
    adv_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
    // adv_params.peer_addr = nullptr;
    adv_params.peer_addr_type = BLE_ADDR_TYPE_PUBLIC;
    adv_params.channel_map = ADV_CHNL_ALL;
    adv_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
}

void SecureBLEServer::stop() {
    if (connected) {
        esp_ble_gap_disconnect(nullptr);
    }
    esp_ble_gap_stop_advertising();
}

bool SecureBLEServer::isConnected() const {
    return connected;
}

void SecureBLEServer::setDataCallback(DataCallback cb) {
    this->dataCallback = cb;
}