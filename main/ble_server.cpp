#include "ble_server.h"
#include "secure_storage.h"
#include <esp_bt.h>
#include <esp_gap_bt_api.h>

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHAR_RX_UUID "6d63b001-1f0a-459a-8e24-b73bb92a8f10"
#define CHAR_TX_UUID "6d63b002-1f0a-459a-8e24-b73bb92a8f10"

static const uint8_t PSK[32] = {
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f
};

SecureBLEServer::SecureBLEServer() 
    : server(nullptr), service(nullptr), char_tx(nullptr), char_rx(nullptr),
      connected(false), dataCallback(nullptr) {}

void SecureBLEServer::begin() {
    if (!SecureStorage::init()) while(true);
    setupSecurity();
    setupServer();
}

void SecureBLEServer::setupSecurity() {
    esp_ble_io_cap_t iocap = ESP_IO_CAP_KEYBOARD_ONLY;
    esp_ble_gap_set_security_param(ESP_BLE_SEC_PARAM_IO_CAPABILITY, &iocap, sizeof(uint8_t));
    uint8_t auth = ESP_LE_AUTH_REQ_SC_BOND;
    esp_ble_gap_set_security_param(ESP_BLE_SEC_PARAM_AUTH_REQ, &auth, sizeof(uint8_t));
    uint8_t key_size = 16;
    esp_ble_gap_set_security_param(ESP_BLE_SEC_PARAM_KEY_SIZE, &key_size, sizeof(uint8_t));
}

void SecureBLEServer::setupServer() {
    BLEDevice::init("DriveCompanion.id");
    server = BLEDevice::createServer();
    server->setCallbacks(new BLEServerHandler());
    service = server->createService(BLEUUID(SERVICE_UUID));
    char_rx = service->createCharacteristic(BLEUUID(CHAR_RX_UUID), BLECharacteristic::PROPERTY_WRITE);
    char_rx->setCallbacks(new BLECharacteristicHandler());
    char_rx->addDescriptor(new BLE2902());
    char_tx = service->createCharacteristic(BLEUUID(CHAR_TX_UUID), BLECharacteristic::PROPERTY_NOTIFY);
    char_tx->addDescriptor(new BLE2902());
    service->start();
    server->getAdvertising()->start();
}

void SecureBLEServer::onConnect(BLEServer* pServer) {
    connected = true;
}

void SecureBLEServer::onDisconnect(BLEServer* pServer) {
    connected = false;
    pServer->getAdvertising()->start();
}

void SecureBLEServer::onWrite(BLECharacteristic* pChar) {
    std::string value = pChar->getValue();
    if (value.length() == 0) return;
    if (dataCallback) {
        dataCallback(value.c_str(), value.length());
    }
}

void SecureBLEServer::setDataCallback(DataCallback cb) {
    this->dataCallback = cb;
}

bool SecureBLEServer::isConnected() const {
    return connected;
}