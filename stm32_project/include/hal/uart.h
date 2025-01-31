#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f0xx_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    UART1_INSTANCE,
    UART2_INSTANCE,
} uart_instance_t;

typedef void (*uart_rx_callback_t)(uint8_t *data, uint16_t len);

HAL_StatusTypeDef uart_init(uart_instance_t instance);
HAL_StatusTypeDef uart_send(uart_instance_t instance, const uint8_t *data, uint16_t len);
void uart_set_rx_callback(uart_instance_t instance, uart_rx_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif // UART_H