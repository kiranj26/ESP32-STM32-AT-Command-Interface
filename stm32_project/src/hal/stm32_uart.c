/* stm32_uart.c */

#include "hal/uart.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdbool.h>

/* Static UART Handles */
static UART_HandleTypeDef huart1;
static UART_HandleTypeDef huart2;

/* Callback functions for received data */
static uart_rx_callback_t rx_callback1 = NULL;
static uart_rx_callback_t rx_callback2 = NULL;

/* Circular Buffer Configuration for USART1 */
#define RX_BUFFER_SIZE1 256
static uint8_t rx_buffer1[RX_BUFFER_SIZE1];
static volatile uint16_t rx_head1 = 0;
static volatile uint16_t rx_tail1 = 0;

/* Circular Buffer Configuration for USART2 */
#define RX_BUFFER_SIZE2 256
static uint8_t rx_buffer2[RX_BUFFER_SIZE2];
static volatile uint16_t rx_head2 = 0;
static volatile uint16_t rx_tail2 = 0;

/* Transmit Buffer Configuration for USART1 */
#define TX_BUFFER_SIZE1 256
static uint8_t tx_buffer1[TX_BUFFER_SIZE1];
static volatile uint16_t tx_head1 = 0;
static volatile uint16_t tx_tail1 = 0;
static volatile bool uart1_tx_busy = false;

/* Transmit Buffer Configuration for USART2 */
#define TX_BUFFER_SIZE2 256
static uint8_t tx_buffer2[TX_BUFFER_SIZE2];
static volatile uint16_t tx_head2 = 0;
static volatile uint16_t tx_tail2 = 0;
static volatile bool uart2_tx_busy = false;

/* Initialize UART with interrupt */
HAL_StatusTypeDef uart_init(uart_instance_t instance) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_StatusTypeDef status = HAL_OK;

    if (instance == UART1_INSTANCE) {
        /* Enable USART1 and GPIOA clocks */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* Configure TX (PA9) as alternate function push-pull */
        GPIO_InitStruct.Pin       = GPIO_PIN_9;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Configure RX (PA10) as alternate function input */
        GPIO_InitStruct.Pin       = GPIO_PIN_10;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* UART1 configuration */
        huart1.Instance          = USART1;
        huart1.Init.BaudRate     = 115200;
        huart1.Init.WordLength   = UART_WORDLENGTH_8B;
        huart1.Init.StopBits     = UART_STOPBITS_1;
        huart1.Init.Parity       = UART_PARITY_NONE;
        huart1.Init.Mode         = UART_MODE_TX_RX;
        huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
        huart1.Init.OverSampling = UART_OVERSAMPLING_16;

        /* Initialize UART1 */
        if (HAL_UART_Init(&huart1) != HAL_OK) {
            return HAL_ERROR;
        }

        /* Enable USART1 interrupts in NVIC */
        HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);

        /* Start UART1 reception in interrupt mode */
        if (HAL_UART_Receive_IT(&huart1, &rx_buffer1[rx_head1], 1) != HAL_OK) {
            return HAL_ERROR;
        }
    }
    else if (instance == UART2_INSTANCE) {
        /* Enable USART2 and GPIOA clocks */
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* Configure TX (PA2) as alternate function push-pull */
        GPIO_InitStruct.Pin       = GPIO_PIN_2;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Configure RX (PA3) as alternate function input */
        GPIO_InitStruct.Pin       = GPIO_PIN_3;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* UART2 configuration */
        huart2.Instance          = USART2;
        huart2.Init.BaudRate     = 115200;
        huart2.Init.WordLength   = UART_WORDLENGTH_8B;
        huart2.Init.StopBits     = UART_STOPBITS_1;
        huart2.Init.Parity       = UART_PARITY_NONE;
        huart2.Init.Mode         = UART_MODE_TX_RX;
        huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
        huart2.Init.OverSampling = UART_OVERSAMPLING_16;

        /* Initialize UART2 */
        if (HAL_UART_Init(&huart2) != HAL_OK) {
            return HAL_ERROR;
        }

        /* Enable USART2 interrupts in NVIC */
        HAL_NVIC_SetPriority(USART2_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);

        /* Start UART2 reception in interrupt mode */
        if (HAL_UART_Receive_IT(&huart2, &rx_buffer2[rx_head2], 1) != HAL_OK) {
            return HAL_ERROR;
        }
    }
    else {
        return HAL_ERROR; // Unsupported UART instance
    }

    return status;
}

/* Send data over UART (Non-blocking) */
HAL_StatusTypeDef uart_send(uart_instance_t instance, const uint8_t *data, uint16_t len) {
    if (len == 0) {
        return HAL_OK;
    }

    if (instance == UART1_INSTANCE) {
        // Check if there is enough space in the TX buffer
        uint16_t available_space = (tx_tail1 + TX_BUFFER_SIZE1 - tx_head1 - 1) % TX_BUFFER_SIZE1;
        if (len > available_space) {
            // Not enough space, handle accordingly (e.g., discard data or wait)
            return HAL_ERROR;
        }

        // Copy data to TX buffer
        for (uint16_t i = 0; i < len; i++) {
            tx_buffer1[tx_tail1] = data[i];
            tx_tail1 = (tx_tail1 + 1) % TX_BUFFER_SIZE1;
        }

        // If UART is not busy, start transmission
        if (!uart1_tx_busy) {
            uart1_tx_busy = true;
            uint8_t byte = tx_buffer1[tx_head1];
            tx_head1 = (tx_head1 + 1) % TX_BUFFER_SIZE1;
            if (HAL_UART_Transmit_IT(&huart1, &byte, 1) != HAL_OK) {
                uart1_tx_busy = false;
                return HAL_ERROR;
            }
        }
    }
    else if (instance == UART2_INSTANCE) {
        // Check if there is enough space in the TX buffer
        uint16_t available_space = (tx_tail2 + TX_BUFFER_SIZE2 - tx_head2 - 1) % TX_BUFFER_SIZE2;
        if (len > available_space) {
            // Not enough space, handle accordingly (e.g., discard data or wait)
            return HAL_ERROR;
        }

        // Copy data to TX buffer
        for (uint16_t i = 0; i < len; i++) {
            tx_buffer2[tx_tail2] = data[i];
            tx_tail2 = (tx_tail2 + 1) % TX_BUFFER_SIZE2;
        }

        // If UART is not busy, start transmission
        if (!uart2_tx_busy) {
            uart2_tx_busy = true;
            uint8_t byte = tx_buffer2[tx_head2];
            tx_head2 = (tx_head2 + 1) % TX_BUFFER_SIZE2;
            if (HAL_UART_Transmit_IT(&huart2, &byte, 1) != HAL_OK) {
                uart2_tx_busy = false;
                return HAL_ERROR;
            }
        }
    }
    else {
        return HAL_ERROR; // Unsupported UART instance
    }

    return HAL_OK;
}

/* Set callback for received data */
void uart_set_rx_callback(uart_instance_t instance, uart_rx_callback_t callback) {
    if (instance == UART1_INSTANCE) {
        rx_callback1 = callback;
    }
    else if (instance == UART2_INSTANCE) {
        rx_callback2 = callback;
    }
}

/* UART IRQ Handlers */
void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart1);
}

void USART2_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart2);
}

/* UART Receive Complete Callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        /* Retrieve the received byte */
        uint8_t received_byte = rx_buffer1[rx_head1];

        /* Update head */
        rx_head1 = (rx_head1 + 1) % RX_BUFFER_SIZE1;

        /* Check for buffer overflow */
        if (rx_head1 == rx_tail1) {
            // Buffer overflow, move tail
            rx_tail1 = (rx_tail1 + 1) % RX_BUFFER_SIZE1;
        }

        /* Re-enable UART1 reception */
        if (HAL_UART_Receive_IT(&huart1, &rx_buffer1[rx_head1], 1) != HAL_OK) {
            // Handle error
        }

        /* Invoke callback with the received data */
        if (rx_callback1) {
            rx_callback1(received_byte);
        }
    }
    else if (huart->Instance == USART2) {
        /* Retrieve the received byte */
        uint8_t received_byte = rx_buffer2[rx_head2];

        /* Update head */
        rx_head2 = (rx_head2 + 1) % RX_BUFFER_SIZE2;

        /* Check for buffer overflow */
        if (rx_head2 == rx_tail2) {
            // Buffer overflow, move tail
            rx_tail2 = (rx_tail2 + 1) % RX_BUFFER_SIZE2;
        }

        /* Re-enable UART2 reception */
        if (HAL_UART_Receive_IT(&huart2, &rx_buffer2[rx_head2], 1) != HAL_OK) {
            // Handle error
        }

        /* Invoke callback with the received data */
        if (rx_callback2) {
            rx_callback2(received_byte);
        }
    }
}

/* UART Transmit Complete Callback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (tx_head1 != tx_tail1) {
            // There is more data to send
            uint8_t byte = tx_buffer1[tx_head1];
            tx_head1 = (tx_head1 + 1) % TX_BUFFER_SIZE1;
            if (HAL_UART_Transmit_IT(&huart1, &byte, 1) != HAL_OK) {
                uart1_tx_busy = false;
                // Handle transmission error
            }
        } else {
            // No more data to send
            uart1_tx_busy = false;
        }
    }
    else if (huart->Instance == USART2) {
        if (tx_head2 != tx_tail2) {
            // There is more data to send
            uint8_t byte = tx_buffer2[tx_head2];
            tx_head2 = (tx_head2 + 1) % TX_BUFFER_SIZE2;
            if (HAL_UART_Transmit_IT(&huart2, &byte, 1) != HAL_OK) {
                uart2_tx_busy = false;
                // Handle transmission error
            }
        } else {
            // No more data to send
            uart2_tx_busy = false;
        }
    }
}

/* Retrieve received data from buffer */
bool uart_receive(uart_instance_t instance, uint8_t *data) {
    if (instance == UART1_INSTANCE) {
        if (rx_tail1 == rx_head1) {
            // No data available
            return false;
        }

        *data = rx_buffer1[rx_tail1];
        rx_tail1 = (rx_tail1 + 1) % RX_BUFFER_SIZE1;
        return true;
    }
    else if (instance == UART2_INSTANCE) {
        if (rx_tail2 == rx_head2) {
            // No data available
            return false;
        }

        *data = rx_buffer2[rx_tail2];
        rx_tail2 = (rx_tail2 + 1) % RX_BUFFER_SIZE2;
        return true;
    }
    else {
        return false; // Unsupported UART instance
    }
}
