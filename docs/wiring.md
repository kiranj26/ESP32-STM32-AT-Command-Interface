# UART Wiring Guide

## Overview

This guide details the UART connections between the STM32 Nucleo board and the ESP32-C3 Dev Kit v2. Proper wiring is essential for reliable communication using AT commands.

## Hardware Connections

### STM32F030R8 Nucleo ↔ ESP32-C3 Dev Kit v2

| **STM32F030R8 Nucleo** | **ESP32-C3 Dev Kit v2** |
|------------------------|-------------------------|
| **PA9 (USART1_TX)**    | **RX0 (U0RXD)**          |
| **PA10 (USART1_RX)**   | **TX0 (U0TXD)**          |
| **GND**                | **GND**                  |

### STM32F446RE Nucleo ↔ ESP32-C3 Dev Kit v2

| **STM32F446RE Nucleo** | **ESP32-C3 Dev Kit v2** |
|-------------------------|-------------------------|
| **PA2 (USART2_TX)**     | **RX0 (U0RXD)**          |
| **PA3 (USART2_RX)**     | **TX0 (U0TXD)**          |
| **GND**                 | **GND**                  |

## Voltage Levels

- **STM32 Nucleo Boards:** Operate at 3.3V logic levels.
- **ESP32-C3 Dev Kit v2:** Operates at 3.3V logic levels.

**Important:** Ensure that both devices are connected to a common ground to prevent communication issues.

## Pin Mapping Diagram

![Wiring Diagram](../docs/wiring_diagram.png)

**Note:** Create and include a `wiring_diagram.png` image in the `docs/` directory illustrating the connections.

## Additional Notes

- **UART Ports:** Depending on the STM32 board, different USART instances are used. Refer to the `platformio.ini` configuration for specifics.
- **Flow Control:** This setup does not use hardware flow control. If needed, additional connections for RTS/CTS can be implemented.
