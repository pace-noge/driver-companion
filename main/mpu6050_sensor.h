#ifndef MPU6050_SENSOR_H
#define MPU6050_SENSOR_H

#include "driver/i2c.h"

// Prepare for ESP-IDF native MPU6050 support
// All MPU6050 register and I2C operations should use ESP-IDF I2C driver
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
    // Remove Arduino MPU6050 and Wire dependencies
    // Add ESP-IDF I2C handle and MPU6050 register values
    i2c_port_t i2c_port = I2C_NUM_0;
    uint8_t mpu_addr = 0x68;
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
    uint8_t detectAddress();
    void applyOrientation(float& ax, float& ay, float& az, float& gx, float& gy, float& gz);
    float lpf(float current, float previous, float alpha);
    static constexpr float LPF_ALPHA = 0.3f;
    static constexpr float GRAVITY = 9.81f;
};

#endif