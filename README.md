# DriveCompanion.id

Emotionally intelligent driver companion for ESP32-C3.

## Features
- Emotion-reactive robo face
- Full-screen navigation with custom arrows
- MPU6050 motion sensing
- Secure BLE pairing with dynamic PIN
- Indonesia-optimized navigation

## Hardware
- ESP32-C3 SuperMini
- SSD1306 128x64 OLED
- MPU6050 (GY-521)
- I²C wiring: GPIO0=SCL, GPIO1=SDA

## Build
```bash
idf.py set-target esp32c3
idf.py build flash monitor