#ifndef HARDWARE_CONFIG_H
#define HARDWARE_CONFIG_H


#include <stdint.h>
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5
#define I2C_FREQ_HZ 400000

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
extern uint8_t DISPLAY_I2C_ADDR;

#ifdef __cplusplus
extern "C" {
#endif
uint8_t i2c_scan_and_get_addr();
#ifdef __cplusplus
}
#endif

#endif