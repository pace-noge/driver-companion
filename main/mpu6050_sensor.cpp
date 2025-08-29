#include "mpu6050_sensor.h"
#include "hardware_config.h"
#include <esp_log.h>

static const char* TAG = "MPU6050Sensor";

MPU6050Sensor::MPU6050Sensor()
    : mpu(Wire), lastUpdate(0), speedEstimate(0.0f),
      accelOffsetX(0), accelOffsetY(0), accelOffsetZ(0),
      gyroOffsetX(0), gyroOffsetY(0), gyroOffsetZ(0),
      orientation(ORIENTATION_HORIZONTAL)
{}

byte MPU6050Sensor::detectAddress() {
    Wire.beginTransmission(0x68);
    byte error = Wire.endTransmission();
    if (error == 0) return 0x68;
    Wire.beginTransmission(0x70);
    error = Wire.endTransmission();
    if (error == 0) return 0x70;
    return 0x00;
}

bool MPU6050Sensor::begin() {
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN, I2C_FREQ_HZ);
    byte addr = detectAddress();
    if (addr == 0x00) return false;
    mpu.setI2CAddress(addr);
    byte status = mpu.begin();
    if (status != 0) return false;
    if (SecureStorage::readString(NVS_ORIENTATION, (char*)&orientation, 1)) {}
    if (SecureStorage::readBlob(NVS_ACCEL_X, &accelOffsetX, sizeof(float))) {
        SecureStorage::readBlob(NVS_ACCEL_Y, &accelOffsetY, sizeof(float));
        SecureStorage::readBlob(NVS_ACCEL_Z, &accelOffsetZ, sizeof(float));
        SecureStorage::readBlob(NVS_GYRO_X, &gyroOffsetX, sizeof(float));
        SecureStorage::readBlob(NVS_GYRO_Y, &gyroOffsetY, sizeof(float));
        SecureStorage::readBlob(NVS_GYRO_Z, &gyroOffsetZ, sizeof(float));
    } else {
        if (!calibrate()) return false;
    }
    lastUpdate = millis();
    speedEstimate = 0.0f;
    return true;
}

bool MPU6050Sensor::calibrate() {
    float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
    const int samples = 100;
    for (int i = 0; i < samples; i++) {
        mpu.update();
        ax += mpu.getAccX(); ay += mpu.getAccY(); az += mpu.getAccZ();
        gx += mpu.getGyroX(); gy += mpu.getGyroY(); gz += mpu.getGyroZ();
        delay(10);
    }
    accelOffsetX = ax / samples;
    accelOffsetY = ay / samples;
    accelOffsetZ = az / samples - 1.0f;
    gyroOffsetX = gx / samples;
    gyroOffsetY = gy / samples;
    gyroOffsetZ = gz / samples;
    SecureStorage::writeBlob(NVS_ACCEL_X, &accelOffsetX, sizeof(float));
    SecureStorage::writeBlob(NVS_ACCEL_Y, &accelOffsetY, sizeof(float));
    SecureStorage::writeBlob(NVS_ACCEL_Z, &accelOffsetZ, sizeof(float));
    SecureStorage::writeBlob(NVS_GYRO_X, &gyroOffsetX, sizeof(float));
    SecureStorage::writeBlob(NVS_GYRO_Y, &gyroOffsetY, sizeof(float));
    SecureStorage::writeBlob(NVS_GYRO_Z, &gyroOffsetZ, sizeof(float));
    char orientChar = (char)orientation;
    SecureStorage::writeString(NVS_ORIENTATION, &orientChar);
    return true;
}

void MPU6050Sensor::setOrientation(MountOrientation orient) {
    if (orientation != orient) {
        orientation = orient;
        char orientChar = (char)orient;
        SecureStorage::writeString(NVS_ORIENTATION, &orientChar);
    }
}

float MPU6050Sensor::lpf(float current, float previous, float alpha) {
    return alpha * current + (1.0f - alpha) * previous;
}

void MPU6050Sensor::applyOrientation(float& ax, float& ay, float& az, float& gx, float& gy, float& gz) {
    if (orientation == ORIENTATION_VERTICAL) {
        float temp = ax;
        ax = -az;
        az = temp;
        temp = gx;
        gx = -gz;
        gz = temp;
    }
}

void MPU6050Sensor::update() {
    unsigned long now = millis();
    float dt = (now - lastUpdate) / 1000.0f;
    if (dt > 1.0f) dt = 0.016f;
    lastUpdate = now;
    mpu.update();
    float ax = lpf(mpu.getAccX() - accelOffsetX, filteredAccelX, LPF_ALPHA);
    float ay = lpf(mpu.getAccY() - accelOffsetY, filteredAccelY, LPF_ALPHA);
    float az = lpf(mpu.getAccZ() - accelOffsetZ, filteredAccelZ, LPF_ALPHA);
    float gx = lpf(mpu.getGyroX() - gyroOffsetX, filteredGyroX, LPF_ALPHA);
    float gy = lpf(mpu.getGyroY() - gyroOffsetY, filteredGyroY, LPF_ALPHA);
    float gz = lpf(mpu.getGyroZ() - gyroOffsetZ, filteredGyroZ, LPF_ALPHA);
    applyOrientation(ax, ay, az, gx, gy, gz);
    tiltX = atan2(ay, sqrt(ax*ax + az*az)) * 57.3f;
    tiltY = atan2(-ax, sqrt(ay*ay + az*az)) * 57.3f;
    speedEstimate += (ax * GRAVITY) * dt;
    if (speedEstimate < 0) speedEstimate = 0;
    speedEstimate = 0.9f * speedEstimate + 0.1f * (ax * 10);
}

float MPU6050Sensor::getAccelX() const { return mpu.getAccX() - accelOffsetX; }
float MPU6050Sensor::getAccelY() const { return mpu.getAccY() - accelOffsetY; }
float MPU6050Sensor::getAccelZ() const { return mpu.getAccZ() - accelOffsetZ; }
float MPU6050Sensor::getGyroX() const { return mpu.getGyroX() - gyroOffsetX; }
float MPU6050Sensor::getGyroY() const { return mpu.getGyroY() - gyroOffsetY; }
float MPU6050Sensor::getGyroZ() const { return mpu.getGyroZ() - gyroOffsetZ; }
float MPU6050Sensor::getSpeedEstimate() const { return speedEstimate; }
float MPU6050Sensor::getTiltX() const { return tiltX; }
float MPU6050Sensor::getTiltY() const { return tiltY; }