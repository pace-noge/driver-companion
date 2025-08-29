#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include <Wire.h>
#include <MPU6050_light.h>
#include "secure_storage.h"

enum MountOrientation {
    ORIENTATION_HORIZONTAL,
    ORIENTATION_VERTICAL
};

class MPU6050Sensor {
public:
    MPU6050Sensor();
    bool begin();
    void update();
    float getAccelX() const;
    float getAccelY() const;
    float getAccelZ() const;
    float getGyroX() const;
    float getGyroY() const;
    float getGyroZ() const;
    float getSpeedEstimate() const;
    float getTiltX() const;
    float getTiltY() const;
    bool calibrate();
    void setOrientation(MountOrientation orientation);

private:
    MPU6050 mpu;
    unsigned long lastUpdate;
    float speedEstimate;
    float tiltX;
    float tiltY;
    MountOrientation orientation;
    float accelOffsetX;
    float accelOffsetY;
    float accelOffsetZ;
    float gyroOffsetX;
    float gyroOffsetY;
    float gyroOffsetZ;
    static constexpr const char* NVS_ACCEL_X = "mpu_ax";
    static constexpr const char* NVS_ACCEL_Y = "mpu_ay";
    static constexpr const char* NVS_ACCEL_Z = "mpu_az";
    static constexpr const char* NVS_GYRO_X = "mpu_gx";
    static constexpr const char* NVS_GYRO_Y = "mpu_gy";
    static constexpr const char* NVS_GYRO_Z = "mpu_gz";
    static constexpr const char* NVS_ORIENTATION = "mpu_orient";
    byte detectAddress();
    void applyOrientation(float& ax, float& ay, float& az, float& gx, float& gy, float& gz);
    float lpf(float current, float previous, float alpha);
    static constexpr float LPF_ALPHA = 0.3f;
    static constexpr float GRAVITY = 9.81f;
};

#endif