#include "stm32f0xx_hal.h"
#include <string.h>

/* Define the fixed RX size */
#define FIXED_RX_SIZE 4

/* Peripheral handles */
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_tx;
DMA_HandleTypeDef hdma_usart1_rx;

/* Data buffers */
uint8_t txData[] = "Hello via DMA USART1\r\n";  // Initial message
uint8_t fixedRxBuffer[FIXED_RX_SIZE];            // Buffer to receive fixed number of bytes

/* Function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
void Error_Handler(void);

int main(void)
{
    /* Initialize the HAL Library */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize GPIO, DMA and USART1 */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();

    /* Start DMA-based UART reception for a fixed number of bytes */
    HAL_StatusTypeDef status = HAL_UART_Receive_DMA(&huart1, fixedRxBuffer, FIXED_RX_SIZE);
    if (status != HAL_OK) {
        // Check 'status' here to determine if it's HAL_BUSY or HAL_ERROR
        Error_Handler();
    }

    /* Send the initial message over DMA TX */
    if (HAL_UART_Transmit_DMA(&huart1, txData, sizeof(txData) - 1) != HAL_OK)
    {
        Error_Handler();
    }

    /* Main loop */
    while (1)
    {
        /* Nothing to do here; processing is handled via DMA callbacks */
    }
}

/* USART1 initialization function */
static void MX_USART1_UART_Init(void)
{
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }
}

/* DMA initialization function */
static void MX_DMA_Init(void)
{
    /* Enable DMA1 clock */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure DMA for USART1_TX on DMA1_Channel2 */
    hdma_usart1_tx.Instance = DMA1_Channel2;
    hdma_usart1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_tx.Init.Mode = DMA_NORMAL;
    hdma_usart1_tx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_tx) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_LINKDMA(&huart1, hdmatx, hdma_usart1_tx);

    /* Configure DMA for USART1_RX on DMA1_Channel3 in NORMAL mode for fixed reception */
    hdma_usart1_rx.Instance = DMA1_Channel3;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_NORMAL; // Normal mode is used so that we get an Rx complete callback after FIXED_RX_SIZE bytes
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
        Error_Handler();
    }
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);

    /* Configure and enable DMA interrupt for Channels 2 & 3 if needed */
    HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

/* GPIO initialization function */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIOA clock (for PA9 and PA10) */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure USART1 TX (PA9) and RX (PA10) pins */
    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/* System Clock Configuration */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}
void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart1);
}

/* DMA interrupt handler for Channels 2 and 3 */
void DMA1_Channel2_3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_tx);
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

/* Override the UART MSP Initialization function */
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(uartHandle->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

/* Callback function executed when fixed RX transfer is complete */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        /* Echo back the received fixed number of bytes */
        if (HAL_UART_Transmit_DMA(&huart1, fixedRxBuffer, FIXED_RX_SIZE) != HAL_OK)
        {
            Error_Handler();
        }

        /* Restart the DMA reception for the next fixed RX_SIZE bytes */
        if (HAL_UART_Receive_DMA(&huart1, fixedRxBuffer, FIXED_RX_SIZE) != HAL_OK)
        {
            Error_Handler();
        }
    }
}

/* Callback function executed when TX transfer is complete */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Optionally, you can add code here if you need to do something when TX completes */
}

/* Simple error handler */
void Error_Handler(void)
{
    while (1)
    {
        /* Stay here in case of error */
    }
}
/* Optional SysTick_Handler if needed for HAL_Delay() usage
   (Here, we don't call HAL_Delay, so we might not need it.)
   Uncomment if needed:

void SysTick_Handler(void)
{
    HAL_IncTick();
}
*/
void SysTick_Handler(void)
{
    HAL_IncTick();
}