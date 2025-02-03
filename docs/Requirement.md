# STM32-ESP32 AT Command Interface Project

## Project Overview

This project enables the STM32 microcontroller to communicate with the ESP32 module using AT commands. The STM32 acts as a host controller to send AT commands and receive responses, facilitating Wi-Fi and other capabilities offered by the ESP32 module.

The focus of this project is to improve reliability, efficiency, and debugging capability for handling and processing all AT commands from the ESP32 module.

---

## Project Goals

1. **Enable Reliable Communication**:
   - Use UART (USART1) for communication between STM32 and ESP32.
   - Verify and handle AT command responses properly.
   
2. **Enhance Debugging and Visualization**:
   - Debug and verify the data flow between STM32 and ESP32 effectively.

3. **Handle All AT Commands**:
   - Develop the project to handle a comprehensive list of AT commands.

---

## Improvements Plan

### **Step 1: Improve Debugging and Visualization**

#### **1. Debugging via UART and SWO**
   - Use a dedicated UART (like USART2) or the SWO pin for debugging.
   - Transmit incoming and outgoing data from STM32 over the debug UART or SWO.
   - Add detailed debug messages to indicate the flow of data and errors.

#### **2. Real-time Visualization**
   - Display the content of the `receivedData` buffer in real time.
   - Use LEDs for basic state indications (e.g., toggling an LED on receiving data).
   - Integrate an external tool like a logic analyzer or a terminal (CoolTerm) to visualize data.

#### **3. Testing Framework**
   - Develop scripts or use CoolTerm's "Send Multiple Lines" feature to send a series of AT commands and log the responses.

### **Step 2: Comprehensive Handling of AT Commands**

#### **1. Implement Dynamic Buffering**
   - Use a dynamic or circular buffer to handle multiline responses effectively.
   - Ensure the buffer can process multiple responses simultaneously.

#### **2. Response Parsing**
   - Parse AT command responses into structured formats.
   - Handle success (`OK`) and error (`ERROR`) responses distinctly.

#### **3. Timeout Management**
   - Add timeout mechanisms to handle delayed responses from the ESP32.

#### **4. Error Handling**
   - Detect and recover from UART communication errors.
   - Implement retries for failed commands.

#### **5. AT Command Automation**
   - Maintain a list of all supported AT commands.
   - Develop functions for sending these commands and processing their responses.

#### **6. Add Logging**
   - Implement a lightweight logging system to log incoming and outgoing data.
   - Save logs to memory or transmit them over SWO/UART.

---

## Implementation Steps

### **Hardware Requirements**
   - STM32F0 Discovery Board
   - ESP32-C3 Module
   - USB to TTL Converter
   - Logic Analyzer (optional)

### **Connections**
   - **STM32 USART1 (PA9, PA10)** <--> **ESP32 UART1**
   - **STM32 USART2 (PA2)** <--> **USB to TTL Converter** (for debugging)

### **STM32CubeMX Configuration**
1. Enable `USART1` and `USART2` peripherals.
2. Configure `USART1` for communication with ESP32:
   - Baud Rate: 115200
   - Mode: TX/RX
   - DMA: Enabled (for RX)
3. Configure `USART2` for debugging:
   - Baud Rate: 115200
   - Mode: TX only (Optional: TX/RX)
4. Enable `SWO` for additional debug capabilities.

### **Code Improvements**
   - Use `SWO_Print` or `printf` over UART2 for debugging.
   - Store received data in a buffer and visualize its content periodically.
   - Implement a state machine for handling AT commands and responses.
   - Use timers for timeout mechanisms.

---

## Testing

1. **Hardware Setup**:
   - Connect STM32 `USART1` to ESP32 UART1.
   - Connect STM32 `USART2` to a USB to TTL converter for debugging.

2. **Debugging Verification**:
   - Use STM32CubeIDE's live variable view to inspect the `receivedData` buffer.
   - Check logs on CoolTerm for received AT command responses.

3. **AT Command Tests**:
   - Send basic commands like `AT` and verify `OK` response.
   - Test multiline responses using commands like `AT+GMR`.

4. **Performance Testing**:
   - Send a large number of commands in sequence to ensure stability.
   - Measure response times for different commands.

---

## Future Enhancements

1. **Add Support for Other ESP32 Features**:
   - Wi-Fi, Bluetooth, and more.

2. **Implement Advanced Debugging Tools**:
   - Use Segger RTT for non-intrusive debugging.

3. **Integrate with FreeRTOS**:
   - Enhance real-time capabilities.

4. **Develop a PC Interface**:
   - Build a GUI tool for sending commands and visualizing responses.

5. **Implement Security Features**:
   - Add CRC checks to ensure data integrity.

---