#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_hal.h"  // Include HAL for UART_HandleTypeDef

#ifdef __cplusplus
extern "C" {
#endif

// Initialize the UART interface
HAL_StatusTypeDef uart_init(void);

// Send data over UART (Non-blocking)
HAL_StatusTypeDef uart_send(const uint8_t *data, uint16_t len);

// Set a callback function for received data
typedef void (*uart_rx_callback_t)(uint8_t);
void uart_set_rx_callback(uart_rx_callback_t callback);

// Retrieve received data from buffer
bool uart_receive(uint8_t *data);

#ifdef __cplusplus
}
#endif

#endif // UART_H
