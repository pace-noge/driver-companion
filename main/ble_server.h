#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include <nimble/nimble_port.h>
#include <esp_nimble_hci.h>
#include "ble/ble_nimble_callbacks.h"
#include <BLEService.h>
#include <BLECharacteristic.h>

class BLEServerHandler : public BLEServerCallbacks {
public:
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
};

class BLECharacteristicHandler : public BLECharacteristicCallbacks {
public:
    void onWrite(BLECharacteristic* pCharacteristic);
};

class SecureBLEServer {
public:
    SecureBLEServer();
    void begin();
    void stop();
    bool isConnected() const;

    using DataCallback = void (*)(const char* data, size_t len);
    void setDataCallback(DataCallback cb);

private:
    BLEServer* server;
    BLEService* service;
    BLECharacteristic* char_tx;
    BLECharacteristic* char_rx;

    bool connected;
    DataCallback dataCallback;

    void setupServer();
    void setupSecurity();
};

#endif