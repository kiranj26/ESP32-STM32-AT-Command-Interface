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

- [x] **Create Repository Structure**
  - Set up directories as outlined above.
- [x] **Initialize Git and `.gitignore`**
  - Initialize Git.
  - Create `.gitignore` to exclude build artifacts.
  - Commit the initial structure.
- [x] **Draft `README.md`**
  - Provide an overview, repository structure, supported boards, and high-level setup instructions.
- [x] **Create Essential Documentation**
  - `docs/wiring.md`: Describe UART connections between STM32 and ESP32.
  - `docs/flashing_esp32.md`: Instructions to flash ESP32 with AT firmware.
  - `docs/commands.md`: Reference for supported AT commands.
  - `docs/porting.md`: Guide on adding new STM32 boards.
- [x] **Add License**
  - Choose and add an appropriate license (e.g., MIT).
- [x] **Prepare `CONTRIBUTING.md`**
  - Outline how others can contribute to the project.

### [ ] **Milestone 2: Implement UART Hardware Abstraction Layer (HAL) on STM32**

- [x] **Define UART Interface**
  - Create `stm32_project/include/hal/uart.h`.
- [x] **Implement STM32 UART**
  - Develop `stm32_project/src/hal/stm32_uart.c`.
- [x] **Configure STM32 UART in `main.c`**
  - Initialize UART and implement a basic loop.
- [x] **Update `platformio.ini`**
  - Configure multiple environments for different STM32 boards.
- [x] **Build and Upload Firmware**
  - Compile and upload to STM32 boards.
- [x] **Verify UART Initialization**
  - Implement a simple UART echo or LED toggle to confirm successful initialization.

### [ ] **Milestone 3: Flash ESP32 with Official AT Firmware and Verify Communication**

- [x] **Download Official AT Firmware**
  - Obtain the latest AT firmware compatible with ESP32-C3 from Espressif's repository.
- [x] **Prepare Flashing Script**
  - Create `scripts/flash_esp32.sh` with necessary commands.
- [x] **Flash ESP32**
  - Execute the flashing script to install AT firmware on ESP32.
- [x] **Verify AT Firmware Installation**
  - Use a serial terminal to send the `AT` command and expect an `OK` response.
- [x] **Troubleshoot Flashing Issues**
  - Address any problems encountered during flashing.

### [ ] **Milestone 4: Implement Basic AT Command Communication from STM32 to ESP32**

- [ ] **Modify `main.c` for Basic AT Communication**
  - Implement sending the `AT` command and receiving the response.
- [ ] **Implement AT Command Core**
  - Develop `stm32_project/include/at/core.h` and `stm32_project/src/at/core.c`.
- [ ] **Build and Upload STM32 Firmware**
  - Compile and upload the updated firmware.
- [ ] **Monitor UART Communication**
  - Use PlatformIO’s Serial Monitor to verify communication.
- [ ] **Troubleshoot Communication Issues**
  - Ensure correct UART connections and configurations.

### [ ] **Milestone 5: Implement Wi-Fi Connection Commands**

- [ ] **Implement Wi-Fi Command Handlers**
  - Create `stm32_project/include/at/wifi.h` and `stm32_project/src/at/wifi.c`.
- [ ] **Update `main.c` to Use Wi-Fi Commands**
  - Integrate Wi-Fi connection functionality in the main application.
- [ ] **Build and Upload STM32 Firmware**
  - Compile and upload the updated firmware.
- [ ] **Monitor UART Communication**
  - Verify successful Wi-Fi connection via Serial Monitor.
- [ ] **Validate Wi-Fi Connection**
  - Check the ESP32’s connection on your router or network.

### [ ] **Milestone 6: Implement TCP/IP Connection Commands**

- [ ] **Implement TCP/IP Command Handlers**
  - Create `stm32_project/include/at/tcp.h` and `stm32_project/src/at/tcp.c`.
- [ ] **Update `main.c` to Use TCP/IP Commands**
  - Integrate TCP connection functionality in the main application.
- [ ] **Build and Upload STM32 Firmware**
  - Compile and upload the updated firmware.
- [ ] **Monitor UART Communication**
  - Verify successful TCP connection via Serial Monitor.
- [ ] **Validate TCP Connection**
  - Ensure the ESP32 is connected to the server.

### [ ] **Milestone 7: Implement HTTP GET Request**

- [ ] **Implement HTTP GET Functionality**
  - Extend TCP/IP command handlers if necessary.
- [ ] **Update `main.c` to Perform HTTP GET**
  - Integrate HTTP GET request functionality in the main application.
- [ ] **Build and Upload STM32 Firmware**
  - Compile and upload the updated firmware.
- [ ] **Monitor UART Communication**
  - Verify HTTP GET request and response via Serial Monitor.
- [ ] **Implement Response Handling**
  - Capture and display the HTTP response.
- [ ] **Troubleshoot HTTP Request Issues**
  - Address incomplete responses, timeouts, or connection issues.

### [ ] **Milestone 8: Implement Comprehensive AT Command Handling and Response Parsing**

- [ ] **Enhance AT Command Core with Response Parsing**
  - Improve `at_send_command` to handle multi-line and asynchronous responses.
- [ ] **Implement Retry Mechanism**
  - Add retry functionality for failed commands.
- [ ] **Implement Advanced Response Parsing**
  - Develop functions to extract specific information from responses (e.g., IP address).
- [ ] **Implement Additional AT Commands**
  - Expand the AT command library as needed (e.g., DNS lookup, MQTT).
- [ ] **Update `main.c` to Utilize Enhanced AT Commands**
  - Integrate robust AT command handling in the main application.
- [ ] **Build, Upload, and Test**
  - Compile, upload, and verify the enhanced functionality.

### [ ] **Milestone 9: Develop Unit and Integration Tests**

- [ ] **Set Up Unit Testing Framework**
  - Integrate Unity or Ceedling for unit testing.
- [ ] **Implement Unit Tests**
  - Develop tests for AT command parsing and UART functions.
- [ ] **Implement Integration Tests**
  - Validate end-to-end communication between STM32 and ESP32.
- [ ] **Configure Testing in `platformio.ini`**
  - Set up testing environments and configurations.
- [ ] **Run Tests**
  - Execute unit and integration tests.
- [ ] **Analyze Test Results**
  - Ensure all tests pass and address any failures.
- [ ] **Automate Testing with CI**
  - Set up GitHub Actions to automate build and test processes.

### [ ] **Milestone 10: Finalize Documentation and Create Comprehensive Examples**

- [ ] **Enhance Documentation**
  - Finalize `README.md` with complete setup instructions and usage guides.
  - Update `docs/wiring.md` with detailed UART connections and diagrams.
  - Ensure `docs/flashing_esp32.md` is comprehensive.
  - Complete `docs/commands.md` with all supported AT commands.
  - Finalize `docs/porting.md` for adding new STM32 boards.
- [ ] **Develop Comprehensive Examples**
  - **Example 1: Connect to Wi-Fi and Retrieve IP Address**
    - Create `examples/stm32_wifi_connect/`.
  - **Example 2: Establish TCP Connection and Send Data**
    - Create `examples/stm32_tcp_connection/`.
  - **Example 3: Perform HTTP GET Request**
    - Create `examples/stm32_http_get/`.
- [ ] **Create Helper Scripts**
  - Ensure `scripts/flash_esp32.sh` is functional and documented.
- [ ] **Finalize `platformio.ini` for STM32 Project**
  - Confirm all environments are correctly configured.
- [ ] **Review and Polish**
  - Proofread all documentation.
  - Validate example projects are functional.
- [ ] **Test Examples**
  - Build, upload, and verify all example projects.
- [ ] **Ensure Clean Separation**
  - Maintain clear boundaries between STM32 and ESP32 codebases.
- [ ] **Encourage Community Contributions**
  - Promote the project and invite contributions.
