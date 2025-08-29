#ifndef SECURE_STORAGE_H
#define SECURE_STORAGE_H

#include <nvs.h>

class SecureStorage {
public:
    static bool init();
    static bool writeString(const char* key, const char* value);
    static bool readString(const char* key, char* value, size_t maxSize);
    static bool writeBlob(const char* key, const void* data, size_t len);
    static bool readBlob(const char* key, void* data, size_t len);

private:
    static nvs_handle_t handle;
    static bool initialized;
};

#endif