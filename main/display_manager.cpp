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
        // .task_priority = 2,
        // .task_stack_size = 8192,
        // .task_core_id = 0
    };
    lvgl_port_init(&port_cfg);

    // Add display (only supported fields)
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

    // Create fade overlay (invisible at first)
    fade_overlay = lv_obj_create(lv_scr_act());
    lv_obj_set_size(fade_overlay, LV_HOR_RES, LV_VER_RES);
    lv_obj_set_style_bg_color(fade_overlay, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(fade_overlay, LV_OPA_TRANSP, LV_PART_MAIN);
    lv_obj_set_style_border_width(fade_overlay, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(fade_overlay, 0, LV_PART_MAIN);
    lv_obj_set_style_shadow_width(fade_overlay, 0, LV_PART_MAIN);
    lv_obj_move_to_index(fade_overlay, 9999);  // Bring to front

    // Set default font
    // lv_disp_set_default_font(disp, &lv_font_montserrat_14);
    lv_disp_t* disp = lvgl_port_add_disp(&disp_cfg);
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

    // Ensure overlay is visible and black
    lv_obj_clear_flag(fade_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_obj_set_style_bg_opa(fade_overlay, LV_OPA_TRANSP, LV_PART_MAIN);

    // Create fade-out animation (0% → 100% opacity)
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, fade_overlay);
    lv_anim_set_values(&anim, 0, 255);  // Transparent → Opaque
    lv_anim_set_time(&anim, 300);
    lv_anim_set_exec_cb(&anim, fadeAnimCallback);
    lv_anim_set_path_cb(&anim, lv_anim_path_linear);
    lv_anim_start(&anim);
}

void DisplayManager::fadeIn() {
    if (!fade_overlay) return;

    // Ensure overlay is fully opaque
    lv_obj_set_style_bg_opa(fade_overlay, LV_OPA_COVER, LV_PART_MAIN);

    // Create fade-in animation (100% → 0% opacity)
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, fade_overlay);
    lv_anim_set_values(&anim, 255, 0);  // Opaque → Transparent
    lv_anim_set_time(&anim, 300);
    lv_anim_set_exec_cb(&anim, fadeAnimCallback);
    lv_anim_set_path_cb(&anim, lv_anim_path_linear);
    lv_anim_start(&anim);
}

void DisplayManager::fadeAnimCallback(void* var, int32_t v) {
    lv_obj_t* obj = (lv_obj_t*)var;
    lv_obj_set_style_bg_opa(obj, v, LV_PART_MAIN);
}