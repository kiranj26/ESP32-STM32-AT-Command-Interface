# ESP32-STM32 AT Command Interface

Welcome to the **ESP32-STM32 AT Command Interface** repository! This project facilitates communication between various STM32 boards and the ESP32-C3 Dev Kit v2 using AT commands over UART. The ESP32 runs the official AT firmware provided by Espressif, while the STM32 handles sending commands and processing responses.

## 📁 Repository Structure
```
ESP32-STM32_AT_Interface/
├── stm32_project/                  # STM32 PlatformIO Project
│   ├── include/                    # Header files
│   │   ├── at/                     # AT command handlers
│   │   │   ├── wifi.h              # Wi-Fi commands (AT+CWJAP, etc.)
│   │   │   ├── tcp.h               # TCP/IP commands (AT+CIPSTART, etc.)
│   │   │   └── ...                 # Other command groups
│   │   ├── hal/                    # Hardware abstraction
│   │   │   └── uart.h              # UART interface (send/receive)
│   │   └── utils/                  # Utilities (logging, buffers)
│   ├── src/                        # Source files
│   │   ├── hal/                    # Hardware-specific implementations
│   │   │   └── stm32_uart.c        # STM32 UART implementation
│   │   ├── at/                     # AT command logic
│   │   │   ├── wifi.c              # Wi-Fi command implementations
│   │   │   ├── tcp.c               # TCP command implementations
│   │   │   └── core.c              # Core AT handler (parsing, responses)
│   │   ├── main.c                  # Entry point
│   │   └── utils/                  # Shared utilities
│   ├── platformio.ini              # PlatformIO configuration for STM32
│   └── tests/                      # Unit/integration tests for STM32
│       ├── unit/                   # AT command parser tests
│       └── integration/            # Full UART ↔ ESP32 tests
├── docs/                           # Documentation
│   ├── commands.md                 # AT command reference
│   ├── wiring.md                   # Hardware connections
│   ├── flashing_esp32.md           # Instructions to flash ESP32 with AT firmware
│   └── porting.md                  # Adding new STM32 boards
├── scripts/                        # Helper scripts (e.g., flash ESP32)
│   └── flash_esp32.sh              # Script to flash ESP32 with AT firmware
├── examples/                       # Demo projects
│   ├── stm32_wifi_connect/         # STM32 example: Connect to Wi-Fi
│   ├── stm32_http_client/          # STM32 example: Send HTTP requests
│   └── ...                         # Additional examples
├── LICENSE                         # Project license (e.g., MIT)
├── README.md                       # Project overview and setup instructions
└── .gitignore                      # Git ignore file (e.g., .pio/, *.bin)
```

## 🎯 Milestones

Track the progress of the project by marking each milestone and its subtasks as complete. Simply replace `[ ]` with `[x]` when a task is done.

### [ ] **Milestone 1: Repository Setup & Initial Documentation**

- [ ] **Create Repository Structure**
  - Set up directories as outlined above.
- [ ] **Initialize Git and `.gitignore`**
  - Initialize Git.
  - Create `.gitignore` to exclude build artifacts.
  - Commit the initial structure.
- [ ] **Draft `README.md`**
  - Provide an overview, repository structure, supported boards, and high-level setup instructions.
- [ ] **Create Essential Documentation**
  - `docs/wiring.md`: Describe UART connections between STM32 and ESP32.
  - `docs/flashing_esp32.md`: Instructions to flash ESP32 with AT firmware.
  - `docs/commands.md`: Reference for supported AT commands.
  - `docs/porting.md`: Guide on adding new STM32 boards.
- [ ] **Add License**
  - Choose and add an appropriate license (e.g., MIT).
- [ ] **Prepare `CONTRIBUTING.md`**
  - Outline how others can contribute to the project.

## 📜 License

This project is licensed under the [MIT License](LICENSE).

## 🛠 Setup Instructions

Detailed setup instructions will be provided in subsequent milestones.

## 📚 Additional Resources

- [Espressif AT Firmware Guide](https://docs.espressif.com/projects/esp-at/en/latest/)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [STM32Cube HAL API Reference](https://www.st.com/en/development-tools/stm32cubef0.html)
- [Unity Test Framework](http://www.throwtheswitch.org/unity)

---

**Happy Coding!** 🚀
