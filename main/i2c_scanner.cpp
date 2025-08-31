#include <stdio.h>
#include <driver/i2c.h>
#include "hardware_config.h"
#include "esp_log.h"

#define I2C_PORT I2C_NUM_0

#ifdef __cplusplus
extern "C" {
#endif
uint8_t i2c_scan_and_get_addr();
#ifdef __cplusplus
}
#endif

// Returns first found I2C address, or 0 if none found
uint8_t i2c_scan_and_get_addr() {
    printf("I2C Scanner. Scanning...\n");
    for (uint8_t addr = 1; addr < 127; addr++) {
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_stop(cmd);
        esp_err_t ret = i2c_master_cmd_begin(I2C_PORT, cmd, 100 / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        if (ret == ESP_OK) {
            printf("Found device at 0x%02X\n", addr);
            return addr;
        }
    }
    printf("Scan done. No device found.\n");
    return 0;
}

// Call i2c_scan() from your main() or setup function to print all detected I2C addresses.
