#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_hal.h"  // Include HAL for UART_HandleTypeDef

// Initialize the UART interface
void uart_init(void);

// Send data over UART
void uart_send(const uint8_t *data, uint16_t len);

// Placeholder for UART receive with interrupt (can be expanded later)
void uart_receive_IT(void);

#endif // UART_H
