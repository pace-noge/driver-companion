#include "display_manager.h"
#include "hardware_config.h"
#include <driver/i2c.h>
#include <esp_log.h>
#include "esp_lvgl_port.h"

static const char* TAG = "DisplayManager";

bool DisplayManager::init() {
    // Initialize I2C
    i2c_config_t i2c_cfg = {};
    i2c_cfg.mode = I2C_MODE_MASTER;
    i2c_cfg.sda_io_num = I2C_SDA_PIN;
    i2c_cfg.scl_io_num = I2C_SCL_PIN;
    i2c_cfg.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_cfg.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_cfg.master.clk_speed = I2C_FREQ_HZ;

    esp_err_t err = i2c_param_config(I2C_NUM_0, &i2c_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C config failed: %s", esp_err_to_name(err));
        return false;
    }

    err = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C driver install failed: %s", esp_err_to_name(err));
        return false;
    }

    // Initialize LVGL
    const lvgl_port_cfg_t port_cfg = {
        .task_priority = 2
    };
    lvgl_port_init(&port_cfg);

    // Add display using legacy LVGL port API for SSD1306 I2C
    const lvgl_port_display_cfg_t disp_cfg = {
        .buffer_size = DISPLAY_WIDTH * DISPLAY_HEIGHT / 8,
        .double_buffer = false,
        .hres = DISPLAY_WIDTH,
        .vres = DISPLAY_HEIGHT,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false
        },
        .flags = {
            .buff_dma = 0,
            .buff_spiram = 0
        }
    };

    disp = lvgl_port_add_disp(&disp_cfg);
    if (!disp) {
        ESP_LOGE(TAG, "LVGL display add failed");
        return false;
    }

    // Set default font for the screen
    lv_obj_set_style_text_font(lv_scr_act(), &lv_font_montserrat_14, LV_PART_MAIN | LV_STATE_DEFAULT);

    // Clear screen
    clear();

    ESP_LOGI(TAG, "LVGL display initialized");
    return true;
}

void DisplayManager::clear() {
    lv_obj_clean(lv_scr_act());
}

void DisplayManager::update() {
    // LVGL handles refresh via lv_timer_handler()
}