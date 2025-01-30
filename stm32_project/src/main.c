/* main.c */

#include "hal/uart.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdio.h> // Added for snprintf

/* Function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* Callback function prototypes */
void uart1_rx_handler(uint8_t received_byte);
void uart2_rx_handler(uint8_t received_byte);

/* Function to send AT commands */
void send_at_command(const char *command);

int main(void)
{
    /* Initialize the HAL library (includes setting up SysTick) */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize GPIO (for LED on PA5) */
    MX_GPIO_Init();

    /* Initialize USART1 and USART2 */
    if (uart_init(UART1_INSTANCE) != HAL_OK) {
        // Handle UART1 initialization error (e.g., blink LED rapidly)
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            HAL_Delay(100);
        }
    }

    if (uart_init(UART2_INSTANCE) != HAL_OK) {
        // Handle UART2 initialization error (e.g., blink LED rapidly)
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            HAL_Delay(100);
        }
    }

    /* Set the UART1 receive callback */
    uart_set_rx_callback(UART1_INSTANCE, uart1_rx_handler);

    /* Set the UART2 receive callback */
    uart_set_rx_callback(UART2_INSTANCE, uart2_rx_handler);

    /* Send a test string over USART2 (e.g., to PC) */
    /* const char *test_string = "USART2 initialized for debugging\r\n";
    uart_send(UART2_INSTANCE, (uint8_t *)test_string, strlen(test_string)); */

    /* Send an initial AT command to ESP32 via USART1 */
    send_at_command("AT+GMR\r\n");

    /* Main loop: blink LED and periodically send AT commands */
    while (1)
    {
        /* Toggle the user LED on PA5 every 500ms */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);

        /* Example: Send another AT command every 5 seconds */
        static uint32_t last_send_time = 0;
        if (HAL_GetTick() - last_send_time > 5000) {
            send_at_command("AT\r\n"); // Example AT command to get firmware version
            last_send_time = HAL_GetTick();
        }
    }
}

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