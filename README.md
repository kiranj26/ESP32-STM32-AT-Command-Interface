# ESP32-STM32 AT Command Interface

Welcome to the **ESP32-STM32 AT Command Interface** repository! This project demonstrates communication between the ESP32-C3 Dev Kit v2 and the STM32F030R8 Nucleo board using AT commands over UART. This playground serves as a foundational platform for learning, experimenting, and building projects involving Wi-Fi-enabled applications.

## Repository Structure

```
ESP32-STM32_AT_Interface/
├── src/                # Source code for STM32F030R8
│   ├── main.c          # Main application entry point
│   ├── uart.c          # UART driver for STM32
│   ├── esp32_at.c      # AT command handling for ESP32
│   └── esp32_at.h      # Header file for AT command handler
├── esp32_firmware/     # ESP32 AT firmware setup files
│   ├── README.md       # Steps to flash ESP32 with AT firmware
│   ├── AT-firmware.bin # ESP32 AT firmware binary
├── doc/                # Documentation
│   ├── AT_Command_Set.pdf  # ESP32 AT command reference
│   ├── wiring_diagram.png # Hardware wiring diagram
│   └── project_description.md # Detailed project description
├── README.md           # Project overview and setup instructions
└── LICENSE             # Repository license (e.g., MIT License)
```

## Repository Description

This repository is a **platform for AT command communication** between the ESP32-C3 Dev Kit v2 and the STM32F030R8 Nucleo board. It includes everything needed to:
- Flash AT firmware to the ESP32-C3.
- Set up UART communication between the two boards.
- Send and receive AT commands.
- Build Wi-Fi-enabled applications using the ESP32 as a Wi-Fi module.

## Features

- **UART Communication**: Establishes robust UART communication between STM32 and ESP32.
- **AT Command Handling**: Send and receive AT commands to configure the ESP32 and perform actions like connecting to Wi-Fi.
- **Wi-Fi Control**: Enables the STM32 to control the ESP32 for Wi-Fi-based tasks (e.g., HTTP requests, TCP/UDP communication).
- **Extensibility**: Provides a modular design to extend the functionality.

## Setup Instructions

### 1. Flash ESP32 with AT Firmware
1. Download the [AT firmware](https://www.espressif.com/).
2. Use `esptool.py` to flash the firmware:
   ```bash
   esptool.py --chip esp32 --port COMx --baud 115200 write_flash -z 0x1000 AT-firmware.bin
   ```
3. Verify the ESP32 responds to AT commands using a serial terminal.

### 2. Hardware Connections
- Connect the STM32 and ESP32 via UART:
  - **STM32 TX** → **ESP32 RX**
  - **STM32 RX** → **ESP32 TX**
  - **GND** → **GND**
- Refer to the `wiring_diagram.png` in the `doc` folder for details.

### 3. Build and Upload STM32 Code
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/ESP32-STM32_AT_Interface.git
   ```
2. Open the STM32 project in your IDE (e.g., STM32CubeIDE).
3. Build and flash the firmware onto the STM32F030R8.

## Usage

1. Power up the STM32 and ESP32.
2. Open a serial monitor to view debug output from the STM32.
3. Send AT commands from the STM32 to the ESP32 (e.g., `AT+CWJAP="SSID","PASSWORD"`).
4. Observe responses from the ESP32 and verify functionality.


## Contributing

Contributions are welcome! Feel free to submit issues, feature requests, or pull requests to enhance the functionality.

## License

This project is licensed under the [MIT License](LICENSE).
