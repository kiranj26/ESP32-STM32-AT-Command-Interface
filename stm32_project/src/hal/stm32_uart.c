/* stm32_uart.c */

#include "hal/uart.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdbool.h>

static UART_HandleTypeDef huart1;
static UART_HandleTypeDef huart2;
static uart_rx_callback_t rx_callback1 = NULL;
static uart_rx_callback_t rx_callback2 = NULL;

/* Add these at the top of stm32_uart.c */
static DMA_HandleTypeDef hdma_usart1_rx;
static DMA_HandleTypeDef hdma_usart2_rx;

/* DMA RX buffers */
#define RX_BUFFER_SIZE 256
__ALIGN_BEGIN static uint8_t rx_buffer1[RX_BUFFER_SIZE] __ALIGN_END;
__ALIGN_BEGIN static uint8_t rx_buffer2[RX_BUFFER_SIZE] __ALIGN_END;

HAL_StatusTypeDef uart_init(uart_instance_t instance) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    HAL_StatusTypeDef status = HAL_OK;

    if (instance == UART1_INSTANCE) {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        huart1.Instance = USART1;
        huart1.Init.BaudRate = 115200;
        huart1.Init.WordLength = UART_WORDLENGTH_8B;
        huart1.Init.StopBits = UART_STOPBITS_1;
        huart1.Init.Parity = UART_PARITY_NONE;
        huart1.Init.Mode = UART_MODE_TX_RX;
        huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        huart1.Init.OverSampling = UART_OVERSAMPLING_16;
        HAL_UART_Init(&huart1);

        /* DMA RX config */
        huart1.hdmarx = &hdma_usart1_rx;
        hdma_usart1_rx.Instance = DMA1_Channel3;
        hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
        hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
        HAL_DMA_Init(&hdma_usart1_rx);
        HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);

        __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);
        
        /* Enable IDLE interrupt */
        __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
        HAL_UART_Receive_DMA(&huart1, rx_buffer1, RX_BUFFER_SIZE);
    }
    else if (instance == UART2_INSTANCE) {
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_DMA1_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        huart2.Instance = USART2;
        huart2.Init = huart1.Init;  // Same config as UART1
        HAL_UART_Init(&huart2);

        /* DMA RX config */
        huart2.hdmarx = &hdma_usart2_rx;
        hdma_usart2_rx.Instance = DMA1_Channel5;
        hdma_usart2_rx.Init = hdma_usart1_rx.Init;
        HAL_DMA_Init(&hdma_usart2_rx);
        // For USART2 RX DMA (add after HAL_DMA_Init)
        HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

        __HAL_LINKDMA(&huart2, hdmarx, hdma_usart2_rx);
        
        __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
        HAL_UART_Receive_DMA(&huart2, rx_buffer2, RX_BUFFER_SIZE);
    }

    return status;
}

HAL_StatusTypeDef uart_send(uart_instance_t instance, const uint8_t *data, uint16_t len) {
    if (instance == UART1_INSTANCE) {
        return HAL_UART_Transmit_IT(&huart1, (uint8_t *)data, len);
    } else if (instance == UART2_INSTANCE) {
        return HAL_UART_Transmit_IT(&huart2, (uint8_t *)data, len);
    }
    return HAL_ERROR;
}

void uart_set_rx_callback(uart_instance_t instance, uart_rx_callback_t callback) {
    if (instance == UART1_INSTANCE) rx_callback1 = callback;
    else if (instance == UART2_INSTANCE) rx_callback2 = callback;
}

void USART1_IRQHandler(void) {
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        
        /* Calculate received length */
        uint16_t len = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart1.hdmarx);
        uint16_t pos = RX_BUFFER_SIZE - len;
        
        if (rx_callback1) {
            rx_callback1(&rx_buffer1[pos], len);
        }
        
        /* Reset DMA */
        __HAL_DMA_DISABLE(huart1.hdmarx);
        huart1.hdmarx->Instance->CNDTR = RX_BUFFER_SIZE;
        huart1.hdmarx->Instance->CMAR = (uint32_t)rx_buffer1;
        __HAL_DMA_ENABLE(huart1.hdmarx);
    }
    HAL_UART_IRQHandler(&huart1);
}

void USART2_IRQHandler(void) {
    if (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart2);
        
        uint16_t len = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart2.hdmarx);
        uint16_t pos = RX_BUFFER_SIZE - len;
        
        if (rx_callback2) {
            rx_callback2(&rx_buffer2[pos], len);
        }
        
        __HAL_DMA_DISABLE(huart2.hdmarx);
        huart2.hdmarx->Instance->CNDTR = RX_BUFFER_SIZE;
        huart2.hdmarx->Instance->CMAR = (uint32_t)rx_buffer2;
        __HAL_DMA_ENABLE(huart2.hdmarx);
    }
    HAL_UART_IRQHandler(&huart2);
}