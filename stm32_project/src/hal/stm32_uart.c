/* stm32_uart.c */

#include "hal/uart.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdbool.h>

/* Static UART Handles */
static UART_HandleTypeDef huart1;
static UART_HandleTypeDef huart2;

/* DMA Handles */
static DMA_HandleTypeDef hdma_usart1_tx;
static DMA_HandleTypeDef hdma_usart1_rx;
static DMA_HandleTypeDef hdma_usart2_tx;
static DMA_HandleTypeDef hdma_usart2_rx;

/* Callback functions for received data */
static uart_rx_callback_t rx_callback1 = NULL;
static uart_rx_callback_t rx_callback2 = NULL;

/* Receive Buffers */
#define RX_BUFFER_SIZE1 256
static uint8_t rx_buffer1[RX_BUFFER_SIZE1];

#define RX_BUFFER_SIZE2 256
static uint8_t rx_buffer2[RX_BUFFER_SIZE2];

/* Transmit Buffers */
#define TX_BUFFER_SIZE1 256
static uint8_t tx_buffer1[TX_BUFFER_SIZE1];
static uint16_t tx_size1 = 0;

#define TX_BUFFER_SIZE2 256
static uint8_t tx_buffer2[TX_BUFFER_SIZE2];
static uint16_t tx_size2 = 0;

/* Initialize UART with DMA */
HAL_StatusTypeDef uart_init(uart_instance_t instance) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_StatusTypeDef status = HAL_OK;

    if (instance == UART1_INSTANCE) {
        /* Enable USART1, GPIOA and DMA1 clocks */
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        /* Configure TX (PA9) as alternate function push-pull */
        GPIO_InitStruct.Pin       = GPIO_PIN_9;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Configure RX (PA10) as alternate function push-pull */
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

        if (HAL_UART_Init(&huart1) != HAL_OK) {
            return HAL_ERROR;
        }

        /* Configure DMA for USART1 TX */
        hdma_usart1_tx.Instance                 = DMA1_Channel2;
        hdma_usart1_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_usart1_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_usart1_tx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_usart1_tx.Init.Mode                = DMA_NORMAL;
        hdma_usart1_tx.Init.Priority            = DMA_PRIORITY_LOW;

        if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK) {
            return HAL_ERROR;
        }
        __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);

        /* Configure DMA for USART1 RX */
        hdma_usart1_rx.Instance                 = DMA1_Channel3;
        hdma_usart1_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma_usart1_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_usart1_rx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_usart1_rx.Init.Mode                = DMA_CIRCULAR;
        hdma_usart1_rx.Init.Priority            = DMA_PRIORITY_HIGH;

        if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK) {
            return HAL_ERROR;
        }
        __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);

        /* Enable USART1 DMA for TX and RX */
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  // IDLE interrupt enabled (if needed)

        HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);

        /* Start UART1 reception with DMA */
        /* if (HAL_UART_Receive_DMA(&huart1, rx_buffer1, RX_BUFFER_SIZE1) != HAL_OK) {
            return HAL_ERROR;
        } */
       HAL_StatusTypeDef ret;
        ret = HAL_UART_Receive_DMA(&huart1, rx_buffer1, RX_BUFFER_SIZE1);
        if (ret != HAL_OK) {
            // Optionally, break here to inspect ret and the error code.
            uint32_t err = HAL_UART_GetError(&huart1);
            while(1); // Loop here for debugging
        }

    }
    else if (instance == UART2_INSTANCE) {
        /* Enable USART2, GPIOA and DMA1 clocks */
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        /* Configure TX (PA2) as alternate function push-pull */
        GPIO_InitStruct.Pin       = GPIO_PIN_2;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_NOPULL;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* Configure RX (PA3) as alternate function push-pull */
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

        if (HAL_UART_Init(&huart2) != HAL_OK) {
            return HAL_ERROR;
        }

        /* Configure DMA for USART2 TX */
        hdma_usart2_tx.Instance                 = DMA1_Channel4;
        hdma_usart2_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        hdma_usart2_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_usart2_tx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_usart2_tx.Init.Mode                = DMA_NORMAL;
        hdma_usart2_tx.Init.Priority            = DMA_PRIORITY_LOW;

        if (HAL_DMA_Init(&hdma_usart2_tx) != HAL_OK) {
            return HAL_ERROR;
        }
        __HAL_LINKDMA(&huart2, hdmatx, hdma_usart2_tx);

        /* Configure DMA for USART2 RX */
        hdma_usart2_rx.Instance                 = DMA1_Channel5;
        hdma_usart2_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
        hdma_usart2_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
        hdma_usart2_rx.Init.MemInc              = DMA_MINC_ENABLE;
        hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart2_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        hdma_usart2_rx.Init.Mode                = DMA_CIRCULAR;
        hdma_usart2_rx.Init.Priority            = DMA_PRIORITY_HIGH;

        if (HAL_DMA_Init(&hdma_usart2_rx) != HAL_OK) {
            return HAL_ERROR;
        }
        __HAL_LINKDMA(&huart2, hdmarx, hdma_usart2_rx);

        /* Enable USART2 DMA for TX and RX */
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);

        HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);

        /* Start UART2 reception with DMA */
        if (HAL_UART_Receive_DMA(&huart2, rx_buffer2, RX_BUFFER_SIZE2) != HAL_OK) {
            return HAL_ERROR;
        }
    }
    else {
        return HAL_ERROR; // Unsupported UART instance
    }

    return status;
}

/* Send data over UART using DMA (Non-blocking) */
HAL_StatusTypeDef uart_send_dma(uart_instance_t instance, const uint8_t *data, uint16_t len) {
    if (len == 0) {
        return HAL_OK;
    }

    if (instance == UART1_INSTANCE) {
        if (len > TX_BUFFER_SIZE1) {
            // Data too large for buffer
            return HAL_ERROR;
        }
        memcpy(tx_buffer1, data, len);
        tx_size1 = len;

        if (HAL_UART_Transmit_DMA(&huart1, tx_buffer1, tx_size1) != HAL_OK) {
            return HAL_ERROR;
        }
    }
    else if (instance == UART2_INSTANCE) {
        if (len > TX_BUFFER_SIZE2) {
            // Data too large for buffer
            return HAL_ERROR;
        }
        memcpy(tx_buffer2, data, len);
        tx_size2 = len;

        if (HAL_UART_Transmit_DMA(&huart2, tx_buffer2, tx_size2) != HAL_OK) {
            return HAL_ERROR;
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

/* We are not overriding HAL_UART_IRQHandler here because that causes a multiple definition error.
   Instead, we rely on the default HAL implementation.
   If you need custom idle detection handling, consider using HAL_UARTEx_ReceiveToIdle_DMA (if available)
   or implementing your idle detection in a separate function. */

/* UART Error Callback */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    // Implement error handling here (e.g., reset UART or log errors)
    if (huart->Instance == USART1) {
        // Handle USART1 errors
    }
    else if (huart->Instance == USART2) {
        // Handle USART2 errors
    }
}

/* UART Transmission Complete Callback */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    // Transmission complete; additional processing can be added here if needed.
}

/* Retrieve received data from buffer (not used in DMA mode) */
bool uart_receive(uart_instance_t instance, uint8_t *data) {
    // In DMA mode, data is processed via callbacks.
    return false;
}
