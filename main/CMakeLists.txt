set(COMPONENT_SRCS
    "main.cpp"
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
)

set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES bt)

# LVGL is added via component manager, so no need to declare here
# It will be found via idf_component.yml

register_component()
