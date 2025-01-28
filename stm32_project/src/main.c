#include "hal/uart.h"
#include "stm32f0xx_hal.h"

#include <stdio.h>
#include <string.h>

/* Function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* Function to handle received UART data */
void uart_rx_handler(uint8_t received_byte) {
    // Example: Echo received data
    uart_send(&received_byte, 1);

    // Future: Process received data or trigger events
}

/* Implementing printf over UART using buffer */
int __io_putchar(int ch) {
    uint8_t c = (uint8_t)ch;
    if (uart_send(&c, 1) != HAL_OK) {
        // Handle UART send error (optional)
    }
    return ch;
}

int main(void)
{
    /* Initialize the HAL library (includes setting up SysTick) */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize GPIO (for LED on PA5) */
    MX_GPIO_Init();

    /* Initialize UART and set receive callback */
    if (uart_init() != HAL_OK) {
        // Handle UART initialization error (e.g., blink LED rapidly)
        while (1) {
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
            HAL_Delay(100);
        }
    }

    uart_set_rx_callback(uart_rx_handler);

    /* Print "UART is initialized" message */
    printf("UART is initialized with interrupts\r\n");

    /* Main loop: blink LED */
    while (1)
    {
        /* Toggle the user LED on PA5 every 500ms */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);

        /* Other non-blocking tasks can be performed here */
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
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;  // 8MHz
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
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
   SysTick Handler for the HAL time base
   -------------------------------------------------------------------------- */
void SysTick_Handler(void)
{
    HAL_IncTick();
}
