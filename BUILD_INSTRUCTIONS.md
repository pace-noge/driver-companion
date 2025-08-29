# DriveCompanion Build Instructions

## Prerequisites
- ESP-IDF v5.1 or later
- Git
- CMake
- Ninja build system

## Setup Environment
1. Make sure ESP-IDF is properly installed and the environment variables are set:
   ```bash
   . $HOME/esp/esp-idf/export.sh
   ```

2. Update ESP-IDF git submodules:
   ```bash
   cd $HOME/esp-idf
   git submodule update --init --recursive
   ```

## Project Setup
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/drive_companion.git
   cd drive_companion
   ```

2. Set the target to ESP32-C3:
   ```bash
   idf.py set-target esp32c3
   ```

3. Configure the project (optional):
   ```bash
   idf.py menuconfig
   ```

## Build and Flash
1. Build the project:
   ```bash
   idf.py build
   ```

2. Flash to your ESP32-C3:
   ```bash
   idf.py -p [PORT] flash
   ```
   Replace [PORT] with your device port (e.g., /dev/ttyUSB0 on Linux or /dev/cu.usbserial-X on macOS)

3. Monitor the device output:
   ```bash
   idf.py -p [PORT] monitor
   ```

## Troubleshooting
- If build fails with missing components, try:
  ```bash
  idf.py reconfigure
  ```

- If you encounter "Failed to resolve component" errors, make sure the component is correctly added to `main/idf_component.yml`

- For ESP-IDF git submodule issues, run:
  ```bash
  cd $HOME/esp-idf
  git submodule update --init --recursive
  ```
