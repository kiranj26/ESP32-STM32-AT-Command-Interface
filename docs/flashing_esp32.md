# ESP32 AT Firmware Flashing 

## ⚙️ Prerequisites

### 💻 Hardware

1. **ESP32-C3-MINI-1 Module**
   - Example: ESP32-C3-MINI-1-AT-V3.3.0.0
2. **USB-to-Serial Converter** (if not using a development board with built-in USB)
   - Common Modules: CP2102, CH340, FT232RL
3. **USB Cable**
   - Ensure it's a data-capable USB cable (not charge-only).
4. **Jumper Wires**
   - For making UART connections between ESP32-C3 and USB-to-Serial converter.
5. **Push Buttons** (Optional)
   - **BOOT Button**: To enter bootloader mode.
   - **RESET Button**: To reset the ESP32-C3 module.

### 🖥️ Software

1. **Operating System**
   - macOS
2. **Python 3**
   - Installable via Homebrew.
3. **pip3**
   - Python package installer.
4. **esptool.py**
   - Python-based utility for flashing firmware onto ESP32 modules.
5. **Serial Terminal Application**
   - Options include `screen`, `minicom`, `CoolTerm`, or PlatformIO's Serial Monitor.

---

## 🔌 Hardware Connections

To communicate with the ESP32-C3's AT firmware, establish connections to **UART1**. Here's how to set it up:

### 📍 Pin Mapping
```
| USB-to-Serial Converter | ESP32-C3-MINI-1 Module |
|-------------------------|-------------------------|
| **TXD**                 | **RXD (Pin 6)**          |
| **RXD**                 | **TXD (Pin 7)**          |
| **CTS**                 | **CTS (Pin 5)**          |
| **RTS**                 | **RTS (Pin 4)**          |
| **GND**                 | **GND**                  |
| **3V3**                 | **3V3**                  |
```
### 🔗 Connection Diagram
```
    [USB-to-Serial Converter]         [ESP32-C3-MINI-1]
            +-----------+                   +-----------+
            |           |                   |           |
            |   TXD     | --------------->  | RXD (6)   |
            |           |                   |           |
            |   RXD     | <---------------  | TXD (7)   |
            |           |                   |           |
            |   GND     | --------------->  | GND       |
            |           |                   |           |
            |   3V3     | --------------->  | 3V3       |
            |           |                   |           |
            |   CTS     | --------------->  | CTS (5)   |
            |           |                   |           |
            |   RTS     | <---------------  | RTS (4)   |
            +-----------+                   +-----------+
```
### 📝 Steps to Connect

1. **Power Connections:**
   - Connect the **3V3** pin of the USB-to-Serial converter to the **3V3** pin on the ESP32-C3-MINI-1.
   - Connect the **GND** pin of the converter to the **GND** pin on the module.

2. **Data Connections:**
   - **TXD (Converter)** → **RXD (Pin 6)**
   - **RXD (Converter)** → **TXD (Pin 7)**

3. **Hardware Flow Control (Optional but Recommended):**
   - **CTS (Converter)** → **CTS (Pin 5)**
   - **RTS (Converter)** → **RTS (Pin 4)**

4. **Bootloader Mode Entry:**
   - **Manual Method:**
     1. Press and hold the **BOOT** button on the ESP32-C3-MINI-1.
     2. While holding **BOOT**, press and release the **RESET** button.
     3. Release the **BOOT** button. The module is now in bootloader mode.

---

## 🛠️ Software Setup

### 1. **Install Homebrew (If Not Already Installed)**

Homebrew is a package manager for macOS. Install it using the following command:

    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

**Follow the on-screen instructions** to complete the installation.

### 2. **Install Python 3**

Update Homebrew and install Python 3:
```
    brew update
    brew install python
```
### 3. **Verify Python and pip3 Installation**

Check the installed versions:
```
    python3 --version
    pip3 --version
```
**Expected Output:**
```
    Python 3.x.x
    pip 21.x.x from /usr/local/lib/python3.x/site-packages/pip (python 3.x)
```
### 4. **Install esptool.py**

Use `pip3` to install `esptool.py`:
```
    pip3 install esptool
```
**Verify Installation:**
```
    esptool.py --version
```
**Expected Output:**
```
    esptool.py v4.x.x
```
---

## 🚀 Flashing the AT Firmware

### 1. **Navigate to the Firmware Directory**

Change to the directory containing the AT firmware binaries:
```
    cd esp32/esp-at-firmware/ESP32-C3-MINI-1-AT-V3.3.0.0/ESP32-C3-MINI-1-AT-V3.3.0.0
```
### 2. **Ensure ESP32-C3-MINI-1 Is in Bootloader Mode**

If not already in bootloader mode:

1. **Press and Hold the BOOT Button.**
2. **Press and Release the RESET Button** while still holding the BOOT button.
3. **Release the BOOT Button.**

### 3. **Identify the Correct Serial Port**

List available serial ports:
```
    ls /dev/tty.*
    ls /dev/cu.*
```
Identify the active port by connecting and disconnecting the ESP32-C3-MINI-1 and observing which port appears/disappears.

**Example Ports:**
```
- `/dev/tty.usbserial-1420`
- `/dev/cu.usbserial-1420`
```
**Use `/dev/cu.usbserial-1420`** for macOS.

### 4. **Flash the Firmware Using esptool.py**

Run the following command to flash the firmware:
```
    esptool.py --chip esp32c3 --port /dev/cu.usbserial-1420 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB \
    0x8000 partition_table/partition-table.bin \
    0xd000 ota_data_initial.bin \
    0x0 bootloader/bootloader.bin \
    0x60000 esp-at.bin \
    0x1e000 at_customize.bin \
    0x1f000 customized_partitions/mfg_nvs.bin
```
**Explanation of Parameters:**

- `--chip esp32c3`: Specifies the chip type.
- `--port /dev/cu.usbserial-1420`: Serial port connected to ESP32-C3-MINI-1.
- `--baud 115200`: Communication speed.
- `--before default_reset --after hard_reset`: Reset behavior before and after flashing.
- `write_flash -z`: Flashing command with compression.
- `--flash_mode dio`: Flash mode.
- `--flash_freq 40m`: Flash frequency.
- `--flash_size 4MB`: Flash size.
- The hexadecimal addresses and corresponding `.bin` files specify where each binary should be written in the flash memory.

**Sample Successful Output:**
```
    esptool.py v4.8.1
    Serial port /dev/cu.usbserial-1420
    Connecting....
    Chip is ESP32-C3 (QFN32) (revision v0.4)
    Features: WiFi, BLE
    Crystal is 40MHz
    MAC: 84:fc:e6:1f:74:e8
    Uploading stub...
    Running stub...
    Stub running...
    Configuring flash size...
    Flash will be erased from 0x00008000 to 0x00008fff...
    ...
    Wrote 126976 bytes (13054 compressed) at 0x0001f000 in 2.8 seconds (effective 357.0 kbit/s)...
    Hash of data verified.

    Leaving...
    Hard resetting via RTS pin...
```
---

## ✅ Verifying the AT Firmware Installation

After flashing, verify that the AT firmware is operational by communicating with the ESP32-C3-MINI-1 via UART1.

### 1. **Connect to UART1 Using a Serial Terminal**

Use your preferred serial terminal application. Below are instructions for `screen` and `minicom`.

#### 🔹 **Using `screen`**

1. **Open Terminal.**
2. **Run the following command:**

       screen /dev/cu.usbserial-1420 115200

3. **Interact with ESP32-C3-MINI-1:**
   - **Type `AT`** and press **Enter**.
   - **Expected Response:**
     ```
     OK
     ```

4. **Exit `screen`:**
   - Press `Ctrl + A`, then `K`, and confirm with `Y`.

#### 🔹 **Using `minicom`**

1. **Install `minicom` (if not already installed):**

       brew install minicom

2. **Configure and Start `minicom`:**

       minicom -b 115200 -o -D /dev/cu.usbserial-1420

3. **Interact with ESP32-C3-MINI-1:**
   - **Type `AT`** and press **Enter**.
   - **Expected Response:**
     ```
     OK
     ```

4. **Exit `minicom`:**
   - Press `Ctrl + A`, then `X`, and confirm exit.

#### 🔹 **Using PlatformIO Serial Monitor**

1. **Run the Serial Monitor:**

       pio device monitor -p /dev/cu.usbserial-1420 -b 115200

2. **Interact with ESP32-C3-MINI-1:**
   - **Type `AT`** and press **Enter**.
   - **Expected Response:**
     ```
     OK
     ```

#### 🔹 **Using `CoolTerm` (GUI Application)**

1. **Download and Install CoolTerm:**
   - [CoolTerm Download](https://freeware.the-meiers.org/)

2. **Configure Connection:**
   - **Serial Port:** `/dev/cu.usbserial-1420`
   - **Baud Rate:** `115200`

3. **Open Connection and Interact:**
   - **Type `AT`** and press **Enter**.
   - **Expected Response:**
     ```
     OK
     ```

### 2. **Troubleshooting Verification Issues**

If you do not receive an `OK` response, consider the following steps:

#### 🔸 **A. Ensure ESP32-C3-MINI-1 Is in Normal Operation Mode**

1. **Press the RESET Button** once to ensure the module is in normal mode.

#### 🔸 **B. Verify UART1 Connections**

- **Double-Check Connections:**
  - **TXD (Converter)** ↔ **RXD (Pin 6)**
  - **RXD (Converter)** ↔ **TXD (Pin 7)**
  - **GND ↔ GND**
  - **3V3 ↔ 3V3**
  - **CTS ↔ CTS (Pin 5)**
  - **RTS ↔ RTS (Pin 4)**

#### 🔸 **C. Check Serial Port Configuration**

- **Correct Serial Port:**
  - Ensure you are connected to `/dev/cu.usbserial-1420`.

- **Baud Rate:**
  - Confirm the baud rate is set to `115200` in the serial terminal.

#### 🔸 **D. Verify Firmware Configuration**

- **AT Commands on UART1:**
  - The firmware is configured to handle AT commands on UART1. Ensure you are communicating through UART1 and not UART0 (used for boot messages).

#### 🔸 **E. Re-enter Bootloader Mode and Re-flash (If Necessary)**

1. **Enter Bootloader Mode:**
   - **Press and Hold the BOOT Button.**
   - **Press and Release the RESET Button** while holding BOOT.
   - **Release the BOOT Button.**

2. **Re-flash the Firmware:**
   - Repeat the flashing process using `esptool.py`.

#### 🔸 **F. Check USB Cable and Drivers**

- **Use a Reliable USB Cable:**
  - Ensure the cable supports data transfer.

- **Verify USB-to-Serial Drivers:**
  - macOS typically doesn't require additional drivers, but if you're using a specific USB-to-Serial converter, ensure its drivers are installed.

#### 🔸 **G. Use Verbose Mode for Detailed Logs**

Run `esptool.py` with the `--verbose` flag to get more detailed output:

**Command:**

    esptool.py --verbose --chip esp32c3 --port /dev/cu.usbserial-1420 --baud 115200 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 40m --flash_size 4MB \
    0x8000 partition_table/partition-table.bin \
    0xd000 ota_data_initial.bin \
    0x0 bootloader/bootloader.bin \
    0x60000 esp-at.bin \
    0x1e000 at_customize.bin \
    0x1f000 customized_partitions/mfg_nvs.bin

**Analyze the Output:** Look for any errors or warnings that can indicate the issue.

---

## 🧩 Next Steps

With the AT firmware successfully flashed and verified, you can proceed to integrate the ESP32-C3-MINI-1 with your STM32 microcontroller. This involves:

1. **Establishing UART Communication Between STM32 and ESP32-C3:**
   - Ensure proper UART configuration on the STM32 side to communicate with ESP32-C3's UART1.

2. **Implementing AT Command Handling on STM32:**
   - Develop firmware on the STM32 to send AT commands and process responses from ESP32-C3.

3. **Testing Wi-Fi and BLE Functionalities:**
   - Use AT commands to connect to Wi-Fi networks, establish TCP/UDP connections, and utilize BLE features.

4. **Developing Application Logic:**
   - Integrate the communication capabilities into your application's specific requirements.

---

## 📝 Additional Resources

- **Espressif's ESP-AT Documentation:**
  - [ESP32-C3 AT Command Set](https://docs.espressif.com/projects/esp-at/en/latest/esp32c3/index.html)

- **esptool.py GitHub Repository:**
  - [esptool.py on GitHub](https://github.com/espressif/esptool)

- **Serial Terminal Tools:**
  - [CoolTerm](https://freeware.the-meiers.org/)
  - [Minicom](https://linux.die.net/man/1/minicom)
  - [PlatformIO Serial Monitor](https://docs.platformio.org/en/latest/cli/commands/device.html#pio-device-monitor)

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).

---

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

---

**Happy Developing!** 🚀✨
