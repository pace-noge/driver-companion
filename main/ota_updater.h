#ifndef OTA_UPDATER_H
#define OTA_UPDATER_H

#include <esp_https_ota.h>

class OTAUpdater {
public:
    static bool performUpdate(const char* firmwareUrl);
};

#endif