#include "ssd1306_idf.h"
#include <string.h>
#include <driver/i2c.h>
#include <esp_log.h>
#include "hardware_config.h"
#include "lvgl.h"

#define TAG "SSD1306_IDF"

static uint8_t ssd1306_addr = 0x3C;
static i2c_port_t ssd1306_port = I2C_NUM_0;
static uint8_t ssd1306_buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

static void ssd1306_send_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_master_write_to_device(ssd1306_port, ssd1306_addr, buf, 2, 100 / portTICK_PERIOD_MS);
}

static void ssd1306_send_data(const uint8_t *data, size_t len) {
    uint8_t prefix = 0x40;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ssd1306_addr << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, prefix, true);
    i2c_master_write(cmd, data, len, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(ssd1306_port, cmd, 100 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);
}

bool ssd1306_init(i2c_port_t port, uint8_t addr) {
    ssd1306_port = port;
    ssd1306_addr = addr;
    ESP_LOGI(TAG, "SSD1306 init: port=%d addr=0x%02X", port, addr);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    ssd1306_send_cmd(0xAE); // Display off
    ssd1306_send_cmd(0x20); // Set Memory Addressing Mode
    ssd1306_send_cmd(0x00); // Horizontal Addressing Mode
    ssd1306_send_cmd(0xB0); // Set Page Start Address for Page Addressing Mode
    ssd1306_send_cmd(0xC8); // COM Output Scan Direction
    ssd1306_send_cmd(0x00); // Low column address
    ssd1306_send_cmd(0x10); // High column address
    ssd1306_send_cmd(0x40); // Start line address
    ssd1306_send_cmd(0x81); // Set contrast control
    ssd1306_send_cmd(0x7F);
    ssd1306_send_cmd(0xA1); // Segment re-map
    ssd1306_send_cmd(0xA6); // Normal display
    ssd1306_send_cmd(0xA8); // Set multiplex ratio
    ssd1306_send_cmd(0x3F);
    ssd1306_send_cmd(0xA4); // Output follows RAM content
    ssd1306_send_cmd(0xD3); // Set display offset
    ssd1306_send_cmd(0x00);
    ssd1306_send_cmd(0xD5); // Set display clock divide ratio/oscillator freq
    ssd1306_send_cmd(0x80);
    ssd1306_send_cmd(0xD9); // Set pre-charge period
    ssd1306_send_cmd(0xF1);
    ssd1306_send_cmd(0xDA); // Set com pins hardware config
    ssd1306_send_cmd(0x12);
    ssd1306_send_cmd(0xDB); // Set vcomh
    ssd1306_send_cmd(0x40);
    ssd1306_send_cmd(0x8D); // Enable charge pump regulator
    ssd1306_send_cmd(0x14);
    ssd1306_send_cmd(0xAF); // Display ON
    memset(ssd1306_buffer, 0, sizeof(ssd1306_buffer));
    return true;
}

void ssd1306_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map) {
    int w = (area->x2 - area->x1 + 1);
    int h = (area->y2 - area->y1 + 1);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int px = area->x1 + x;
            int py = area->y1 + y;
            if (px >= SSD1306_WIDTH || py >= SSD1306_HEIGHT) continue;
            int byte_idx = px + (py / 8) * SSD1306_WIDTH;
            int bit = py % 8;
            if (color_map[y * w + x].full) {
                ssd1306_buffer[byte_idx] |= (1 << bit);
            } else {
                ssd1306_buffer[byte_idx] &= ~(1 << bit);
            }
        }
    }
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_send_cmd(0xB0 | page);
        ssd1306_send_cmd(0x00);
        ssd1306_send_cmd(0x10);
        ssd1306_send_data(&ssd1306_buffer[SSD1306_WIDTH * page], SSD1306_WIDTH);
    }
    lv_disp_flush_ready(drv);
}
