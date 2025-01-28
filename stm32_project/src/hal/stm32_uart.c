#include "hal/uart.h"
#include "stm32f0xx_hal.h"
#include <string.h>

/* Static UART Handle */
static UART_HandleTypeDef huart1;

/* Callback function for received data */
static uart_rx_callback_t rx_callback = NULL;

/* Circular Buffer Configuration */
#define RX_BUFFER_SIZE 256
static volatile uint8_t rx_buffer[RX_BUFFER_SIZE];
static volatile uint16_t rx_head = 0;
static volatile uint16_t rx_tail = 0;

/* Transmit Buffer Configuration */
#define TX_BUFFER_SIZE 256
static volatile uint8_t tx_buffer[TX_BUFFER_SIZE];
static volatile uint16_t tx_head = 0;
static volatile uint16_t tx_tail = 0;
static volatile bool uart_tx_busy = false;

/* Initialize UART with interrupt */
HAL_StatusTypeDef uart_init(void) {
    /* Enable USART1 and GPIOA clocks */
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure TX (PA9) as alternate function push-pull */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_9;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Configure RX (PA10) as alternate function input */
    GPIO_InitStruct.Pin       = GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART configuration */
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;

    /* Initialize UART */
    if (HAL_UART_Init(&huart1) != HAL_OK) {
        // Initialization Error
        return HAL_ERROR;
    }

    /* Enable USART1 interrupts in NVIC */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    /* Start UART reception in interrupt mode */
    if (HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_buffer[rx_head], 1) != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}

/* Send data over UART (Non-blocking) */
HAL_StatusTypeDef uart_send(const uint8_t *data, uint16_t len) {
    if (len == 0) {
        return HAL_OK;
    }

    // Check if there is enough space in the TX buffer
    uint16_t available_space = (tx_tail + TX_BUFFER_SIZE - tx_head - 1) % TX_BUFFER_SIZE;
    if (len > available_space) {
        // Not enough space, handle accordingly (e.g., discard data or wait)
        return HAL_ERROR;
    }

    // Copy data to TX buffer
    for (uint16_t i = 0; i < len; i++) {
        tx_buffer[tx_tail] = data[i];
        tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    }

    // If UART is not busy, start transmission
    if (!uart_tx_busy) {
        uart_tx_busy = true;
        uint8_t byte = tx_buffer[tx_head];
        tx_head = (tx_head + 1) % TX_BUFFER_SIZE;
        if (HAL_UART_Transmit_IT(&huart1, &byte, 1) != HAL_OK) {
            uart_tx_busy = false;
            return HAL_ERROR;
        }
    }

    return HAL_OK;
}

/* Set callback for received data */
void uart_set_rx_callback(uart_rx_callback_t callback) {
    rx_callback = callback;
}

/* UART IRQ Handler */
void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart1);
}

/* UART Receive Complete Callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        /* Store the current head before incrementing */
        uint16_t current_head = rx_head;
        rx_head = (rx_head + 1) % RX_BUFFER_SIZE;

        /* Check for buffer overflow */
        if (rx_head == rx_tail) {
            rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
        }

        /* Start receiving next byte */
        if (HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_buffer[rx_head], 1) != HAL_OK) {
            // Handle error
        }

        /* Invoke callback with the received data */
        if (rx_callback) {
            rx_callback(rx_buffer[current_head]);
        }
    }
}

/* UART Transmit Complete Callback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        if (tx_head != tx_tail) {
            // There is more data to send
            uint8_t byte = tx_buffer[tx_head];
            tx_head = (tx_head + 1) % TX_BUFFER_SIZE;
            if (HAL_UART_Transmit_IT(&huart1, &byte, 1) != HAL_OK) {
                uart_tx_busy = false;
                // Handle transmission error
            }
        } else {
            // No more data to send
            uart_tx_busy = false;
        }
    }
}

/* Retrieve received data from buffer */
bool uart_receive(uint8_t *data) {
    if (rx_tail == rx_head) {
        // No data available
        return false;
    }

    *data = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % RX_BUFFER_SIZE;
    return true;
}
