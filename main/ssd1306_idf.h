#ifndef SSD1306_IDF_H
#define SSD1306_IDF_H

#include <stdint.h>
#include <stdbool.h>
#include <driver/i2c.h>
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

bool ssd1306_init(i2c_port_t port, uint8_t addr);
void ssd1306_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_map);

#ifdef __cplusplus
}
#endif

#endif // SSD1306_IDF_H
