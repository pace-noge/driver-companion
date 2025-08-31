#include "ssd1306_idf.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "display_manager.h"
#include "hardware_config.h"
#include <driver/i2c.h>
#include <esp_log.h>
#include "esp_lvgl_port.h"


static const char* TAG = "DisplayManager";

bool DisplayManager::init() {

    // Initialize LVGL core
    lv_init();

    // Initialize I2C driver (if not already installed)
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_SDA_PIN;
    conf.scl_io_num = I2C_SCL_PIN;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_FREQ_HZ;
    esp_err_t err = i2c_param_config(I2C_NUM_0, &conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "I2C param config failed: %s", esp_err_to_name(err));
        return false;
    }
    err = i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) { // Already installed is not fatal
        ESP_LOGE(TAG, "I2C driver install failed: %s", esp_err_to_name(err));
        return false;
    }

    // Initialize SSD1306
    if (!ssd1306_init(I2C_NUM_0, DISPLAY_I2C_ADDR)) {
        ESP_LOGE(TAG, "SSD1306 init failed");
        return false;
    }

    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf1[DISPLAY_WIDTH * DISPLAY_HEIGHT];
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, DISPLAY_WIDTH * DISPLAY_HEIGHT);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = DISPLAY_WIDTH;
    disp_drv.ver_res = DISPLAY_HEIGHT;
    disp_drv.flush_cb = ssd1306_flush;
    disp_drv.draw_buf = &draw_buf;
    disp = lv_disp_drv_register(&disp_drv);
    if (!disp) {
        ESP_LOGE(TAG, "LVGL display register failed");
        return false;
    }
    // (esp_lcd and panel_handle/io_handle code removed, only local SSD1306 driver and LVGL flush used)

    // Create fade overlay (invisible at first)
    fade_overlay = lv_obj_create(lv_disp_get_scr_act(disp));
    lv_obj_set_size(fade_overlay, DISPLAY_WIDTH, DISPLAY_HEIGHT);
    // In 1-bit mode, overlay is always black; explicit color setting is unnecessary
    lv_obj_set_style_border_width(fade_overlay, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(fade_overlay, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(fade_overlay, 0, LV_PART_MAIN);
    lv_obj_move_to_index(fade_overlay, 9999);  // Bring to front

    // Set default font
    // lv_disp_set_default_font(disp, &lv_font_montserrat_14);
    lv_disp_set_default(disp);


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

void DisplayManager::fadeOut() {
    if (!fade_overlay) return;
    lv_obj_clear_flag(fade_overlay, LV_OBJ_FLAG_HIDDEN); // Show overlay (instant)
}

void DisplayManager::fadeIn() {
    if (!fade_overlay) return;
    lv_obj_add_flag(fade_overlay, LV_OBJ_FLAG_HIDDEN); // Hide overlay (instant)
}

void DisplayManager::fadeAnimCallback(void* var, int32_t v) {
    // No-op for 1-bit mode
}