#include "ota_updater.h"
#include "wifi_manager.h"
#include "display_manager.h"
#include "face_renderer.h"
#include <esp_crt_bundle.h>

extern DisplayManager display;
extern FaceRenderer face;

bool OTAUpdater::performUpdate(const char* firmwareUrl) {
    if (!WiFiManager::isConnected()) {
        return false;
    }

    display.clear();
    
    // Create labels for update messages
    lv_obj_t* updating_label = lv_label_create(display.getScreen());
    lv_label_set_text(updating_label, "Updating...");
    lv_obj_align(updating_label, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 20);
    
    lv_obj_t* warning_label = lv_label_create(display.getScreen());
    lv_label_set_text(warning_label, "Do not power off");
    lv_obj_align(warning_label, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 40);
    
    // Set label style
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_color(&style, lv_color_white());
    
    lv_obj_add_style(updating_label, &style, 0);
    lv_obj_add_style(warning_label, &style, 0);
    
    display.update();

    esp_http_client_config_t config = {};
    config.url = firmwareUrl;
    config.cert_pem = NULL;
    config.crt_bundle_attach = esp_crt_bundle_attach;
    config.timeout_ms = 30000;

    esp_err_t ret = esp_https_ota(&config);
    if (ret == ESP_OK) {
        display.clear();
        
        // Create success message
        lv_obj_t* success_label = lv_label_create(display.getScreen());
        lv_label_set_text(success_label, "Update Success!");
        lv_obj_align(success_label, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 30);
        lv_obj_add_style(success_label, &style, 0);
        
        display.update();
    vTaskDelay(pdMS_TO_TICKS(2000));
        esp_restart();
    } else {
        display.clear();
        
        // Create failure message
        lv_obj_t* fail_label = lv_label_create(display.getScreen());
        lv_label_set_text(fail_label, "Update Failed");
        lv_obj_align(fail_label, NULL, LV_ALIGN_IN_TOP_LEFT, 10, 30);
        lv_obj_add_style(fail_label, &style, 0);
        
        display.update();
    vTaskDelay(pdMS_TO_TICKS(3000));
    }
    return false;
}