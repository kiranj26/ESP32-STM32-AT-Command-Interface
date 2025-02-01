/* uart.h */

#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

// UART identifiers
typedef enum {
    UART1_INSTANCE,
    UART2_INSTANCE,
} uart_instance_t;

// Initialize the specified UART interface with DMA
HAL_StatusTypeDef uart_init(uart_instance_t instance);

// Send data over the specified UART using DMA (Non-blocking)
HAL_StatusTypeDef uart_send_dma(uart_instance_t instance, const uint8_t *data, uint16_t len);

// Set a callback function for received data on the specified UART
typedef void (*uart_rx_callback_t)(uint8_t);
void uart_set_rx_callback(uart_instance_t instance, uart_rx_callback_t callback);

// Retrieve received data from buffer (if using a buffer-based approach)
bool uart_receive(uart_instance_t instance, uint8_t *data);

// Optional: Function to start UART reception with DMA
HAL_StatusTypeDef uart_start_receive_dma(uart_instance_t instance, uint8_t *buffer, uint16_t len);

#ifdef __cplusplus
}
#endif

#endif // UART_H
