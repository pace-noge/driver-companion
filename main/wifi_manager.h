#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H


class WiFiManager {
public:
    static bool connect(const char* ssid, const char* password);
    static bool isConnected();
    static void disconnect();
    static bool waitForConnection(int timeoutMs = 10000);
};

#endif