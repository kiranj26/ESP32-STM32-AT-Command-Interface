#include "stm32f0xx_hal.h"   // Include HAL first
#include "hal/uart.h"
#include <string.h>

/* UART Handle */
extern UART_HandleTypeDef huart1;

/* Buffer to store received data */
volatile uint8_t rxByte;

/* Initialize UART with interrupt */
void uart_init(void) {
    __HAL_RCC_USART1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // TX/RX pins in AF1
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin       = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // UART config
    huart1.Instance          = USART1;
    huart1.Init.BaudRate     = 115200;
    huart1.Init.WordLength   = UART_WORDLENGTH_8B;
    huart1.Init.StopBits     = UART_STOPBITS_1;
    huart1.Init.Parity       = UART_PARITY_NONE;
    huart1.Init.Mode         = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        while (1);
    }

    // Enable USART1 interrupts in NVIC
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    // Enable UART Receive Interrupt
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxByte, 1);
}

/* Send data over UART */
void uart_send(const uint8_t *data, uint16_t len) {
    HAL_UART_Transmit(&huart1, (uint8_t*)data, len, HAL_MAX_DELAY);
}

/* UART IRQ Handler */
void USART1_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart1);
}

/* UART Receive Complete Callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        // Echo the received byte
        uart_send((const uint8_t *)&rxByte, 1);
        // Re-enable UART reception
        HAL_UART_Receive_IT(&huart1, (uint8_t *)&rxByte, 1);
    }
}

