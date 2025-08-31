idf_component_register(
    SRCS "main.cpp"
    "i2c_scanner.cpp"
    "hardware_config.cpp"
    "display_manager.cpp"
    "secure_storage.cpp"
    "ble_server.cpp"
    "state_manager.cpp"
    "mpu6050_sensor.cpp"
    "emotion_engine.cpp"
    "face_renderer.cpp"
    "arrow_renderer.cpp"
    "ota_updater.cpp"
    "wifi_manager.cpp"
    "ssd1306_idf.c"
    INCLUDE_DIRS "."
    REQUIRES bt nvs_flash json esp_https_ota
)
