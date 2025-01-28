# Flashing ESP32-C3 Dev Kit v2 with AT Firmware

This guide provides step-by-step instructions to flash the ESP32-C3 Dev Kit v2 with Espressif's official AT firmware using the provided flashing script.

## 📦 Prerequisites

- **ESP32-C3 Dev Kit v2**
- **USB Cable:** To connect the ESP32 to your computer.
- **esptool.py:** Python utility to communicate with the ESP32.
  
  Install via pip:
  ```bash
  pip install esptool
AT Firmware Binary: Download the latest AT firmware from Espressif's GitHub Repository.
🔧 Flashing Steps
1. Download the AT Firmware
Visit the Espressif AT Firmware Releases page.
Download the latest firmware compatible with ESP32-C3.
Extract the downloaded archive and locate the AT-firmware.bin file.
Place AT-firmware.bin in the docs/ directory of this repository.
2. Connect the ESP32 to Your Computer
Use a USB cable to connect the ESP32-C3 Dev Kit v2 to your computer.
Identify the serial port assigned to the ESP32:
Windows: COM3, COM4, etc.
Linux/macOS: /dev/ttyUSB0, /dev/ttyUSB1, etc.
3. Prepare the Flashing Script
Ensure the flashing script scripts/flash_esp32.sh has the correct path to the firmware and the correct serial port.

Edit the Flashing Script:

bash
Copy
nano scripts/flash_esp32.sh
Modify the Following Variables as Needed:

bash
Copy
#!/bin/bash

# flash_esp32.sh
# Script to flash ESP32-C3 with AT firmware

# Path to the AT firmware binary
FIRMWARE_PATH="../docs/AT-firmware.bin"

# Serial port (modify as needed)
PORT="/dev/ttyUSB0"  # Replace with your ESP32 serial port

# Baud rate
BAUD=115200

# Erase the flash
esptool.py --chip esp32c3 --port $PORT erase_flash

# Write the firmware
esptool.py --chip esp32c3 --port $PORT --baud $BAUD write_flash -z 0x0 $FIRMWARE_PATH

echo "🔄 Flashing completed."
Save and Close the File:

If using nano, press CTRL + O to write the changes and CTRL + X to exit.
Make the Flashing Script Executable

bash
Copy
chmod +x scripts/flash_esp32.sh
Run the Flashing Script

bash
Copy
cd scripts
./flash_esp32.sh
Note: Ensure that the ESP32 is not in flashing mode unless required. If the ESP32 doesn't automatically enter flashing mode, you might need to press the BOOT button while pressing the RESET button.
Verify the Flashing Process

After flashing, open a serial terminal (e.g., screen, minicom, or PlatformIO's Serial Monitor) to verify the ESP32 responds to AT commands.

Using PlatformIO Serial Monitor:

bash
Copy
pio device monitor -p /dev/ttyUSB0 -b 115200
Send the AT Command:

plaintext
Copy
AT
Expected Response:

plaintext
Copy
OK
Note: If you receive an OK response, the flashing was successful.

Troubleshooting

No Response:

Ensure the correct serial port is specified in the flashing script.
Verify USB connections and try using a different USB cable.
Confirm that the ESP32 is not in flashing mode during normal operation.
Permission Issues (Linux/macOS):

You might need to add your user to the dialout group or run the script with sudo.
bash
Copy
sudo usermod -a -G dialout $USER
Log out and log back in for the changes to take effect.
Incorrect Firmware:

Ensure that the downloaded AT firmware is compatible with the ESP32-C3.
Baud Rate Mismatch:

Confirm that both the flashing script and the serial monitor use the same baud rate (115200 by default).