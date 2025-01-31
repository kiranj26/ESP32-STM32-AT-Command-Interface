/* main.c */

#include "hal/uart.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdio.h>

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void send_at_command(const char *command);

/* Callbacks now handle buffer pointers */
void uart1_rx_handler(uint8_t *data, uint16_t len);
void uart2_rx_handler(uint8_t *data, uint16_t len);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    if (uart_init(UART1_INSTANCE) != HAL_OK) {
        while (1) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); HAL_Delay(100); }
    }

    if (uart_init(UART2_INSTANCE) != HAL_OK) {
        while (1) { HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); HAL_Delay(100); }
    }

    uart_set_rx_callback(UART1_INSTANCE, uart1_rx_handler);
    uart_set_rx_callback(UART2_INSTANCE, uart2_rx_handler);

    send_at_command("AT+GMR\r\n");

    while (1) {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);

        static uint32_t last_send_time = 0;
        if (HAL_GetTick() - last_send_time > 5000) {
            send_at_command("AT\r\n");
            last_send_time = HAL_GetTick();
        }
    }
}

/* Updated callbacks in main.c */
void uart1_rx_handler(uint8_t *data, uint16_t len) {
    uart_send(UART2_INSTANCE, data, len);
}

void uart2_rx_handler(uint8_t *data, uint16_t len) {
    uart_send(UART1_INSTANCE, data, len);
}

/* Remove the old byte-by-byte handlers from main.c */

void send_at_command(const char *command) {
    uart_send(UART1_INSTANCE, (uint8_t *)command, strlen(command));
}

/* Clock and GPIO config unchanged from original */

/**
  * @brief  System Clock Configuration
  *         HSI = 8 MHz, no PLL, SysClk = 8 MHz.
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Enable PWR clock */
    __HAL_RCC_PWR_CLK_ENABLE();

    /* 1) Configure HSI oscillator */
    RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE; // No PLL
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }

    /* 2) Initialize CPU, AHB, and APB clocks to 8MHz */
    RCC_ClkInitStruct.ClockType       = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource    = RCC_SYSCLKSOURCE_HSI;  // 8MHz
    RCC_ClkInitStruct.AHBCLKDivider   = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider  = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        /* Initialization Error */
        while (1);
    }
}

/**
  * @brief GPIO Initialization Function
  *        Configures PA5 as push-pull output for the onboard user LED.
  */
static void MX_GPIO_Init(void)
{
    /* Enable GPIOA clock */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Configure PA5 for the user LED */
    GPIO_InitStruct.Pin   = GPIO_PIN_5;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* Start with LED off */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/* --------------------------------------------------------------------------
   Callback Functions
   -------------------------------------------------------------------------- */

/* Function to handle received UART1 data (from ESP32) */
void uart1_rx_handler(uint8_t received_byte) {
    // Forward the received byte to USART2
    uart_send(UART2_INSTANCE, &received_byte, 1);
}

/* Function to handle received UART2 data (from PC) */
void uart2_rx_handler(uint8_t received_byte) {
    // Forward the received byte to USART1
    uart_send(UART1_INSTANCE, &received_byte, 1);
}

/* Function to send AT command */
void send_at_command(const char *command) {
    uart_send(UART1_INSTANCE, (uint8_t *)command, strlen(command));
}

/* --------------------------------------------------------------------------
   SysTick Handler
   -------------------------------------------------------------------------- */

/* Remove any SysTick_Handler from main.c to avoid multiple definitions */
void SysTick_Handler(void)
{
    HAL_IncTick();
}