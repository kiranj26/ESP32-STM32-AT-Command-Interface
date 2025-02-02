#include "stm32f0xx_hal.h"
#include <string.h>

#define RX_BUFFER_SIZE 256
#define MAX_RESPONSE_LEN 128

// Global handles
UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

// Circular buffer for RX
uint8_t rx_buffer[RX_BUFFER_SIZE];
volatile uint16_t rx_head = 0;  // DMA write position
volatile uint16_t rx_tail = 0;  // Application read position

// Response tracking
volatile uint8_t response_ready = 0;
char response_buffer[MAX_RESPONSE_LEN];
volatile uint32_t last_rx_time = 0;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART1_UART_Init(void);
void Error_Handler(void);
// ✅ Added prototype
void process_data_chunk(uint16_t start_pos, uint16_t length);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_USART1_UART_Init();

    // Start continuous DMA reception
    // ✅ Check DMA start success
    if (HAL_UART_Receive_DMA(&huart1, rx_buffer, RX_BUFFER_SIZE) != HAL_OK) {
        Error_Handler();
    }
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);

    // Main loop
    while (1) {
        if (response_ready) {
            // ✅ Use PA5 LED only
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            
            // Echo response
            HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n[RX] ", 7, 100);
            HAL_UART_Transmit(&huart1, (uint8_t*)response_buffer, strlen(response_buffer), 100);
            HAL_UART_Transmit(&huart1, (uint8_t*)"\r\n", 2, 100);
            
            response_ready = 0;
        }
        
        // Send AT every 5s (PA5 will blink on successful send)
        static uint32_t last_cmd = 0;
        if (HAL_GetTick() - last_cmd > 5000) {
            HAL_UART_Transmit(&huart1, (uint8_t*)"AT\r\n", 4, 100);
            last_cmd = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // ✅ Blink on transmit
        }
    }
}

// USART1 Initialization
static void MX_USART1_UART_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);
}

// DMA Initialization
static void MX_DMA_Init(void) {
    __HAL_RCC_DMA1_CLK_ENABLE();
    hdma_usart1_rx.Instance = DMA1_Channel3;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_HIGH;
    HAL_DMA_Init(&hdma_usart1_rx);
    __HAL_LINKDMA(&huart1, hdmarx, hdma_usart1_rx);
    
    HAL_NVIC_SetPriority(DMA1_Channel2_3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
}

// UART Idle Interrupt Handler
// ✅ Revised idle handler with PA5 debug
void USART1_IRQHandler(void) {
    if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_IDLE)) {
        __HAL_UART_CLEAR_IDLEFLAG(&huart1);
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // ✅ Toggle on idle
        
        rx_head = RX_BUFFER_SIZE - hdma_usart1_rx.Instance->CNDTR;
        uint16_t start = rx_tail;
        uint16_t end = rx_head;

        if (end >= start) {
            process_data_chunk(start, end - start);
        } else {
            process_data_chunk(start, RX_BUFFER_SIZE - start);
            process_data_chunk(0, end);
        }
        
        rx_tail = end; // Update tail
    }
    HAL_UART_IRQHandler(&huart1);
}

// ✅ Helper function for data processing
void process_data_chunk(uint16_t start_pos, uint16_t length) {
    for (uint16_t i = 0; i < length; i++) {
        uint16_t pos = (start_pos + i) % RX_BUFFER_SIZE;
        response_buffer[i] = rx_buffer[pos];
        
        if (i > 0 && response_buffer[i-1] == '\r' && response_buffer[i] == '\n') {
            response_buffer[i-1] = '\0';
            response_ready = 1;
            break;
        }
    }
}

// ✅ PA5 LED initialization
static void MX_GPIO_Init(void) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    // On-board LED (PA5)
    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

// Clock Configuration (48 MHz from HSI PLL)
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    // Voltage scaling removed for STM32F0 compatibility

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV2;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
}

// Interrupt Handlers
void DMA1_Channel2_3_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart1_rx);
}

// MSP Initialization
void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(uartHandle->Instance == USART1)
    {
        __HAL_RCC_USART1_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
        // Toggle LED or other error indication
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
#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
    (void)file;
    (void)line;
    Error_Handler();
}
#endif