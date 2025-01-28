#include "hal/uart.h"
#include "stm32f0xx_hal.h"  // Include HAL for UART_HandleTypeDef

#include <stdio.h>
#include <string.h>

/* Define UART Handle */
UART_HandleTypeDef huart1;

/* Function prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);

/* Implementing printf over UART */
int __io_putchar(int ch) {
    uint8_t c = ch;
    uart_send(&c, 1);
    return ch;
}

int main(void)
{
    /* Initialize the HAL library (includes setting up SysTick) */
    HAL_Init();

    /* Configure the system clock (8 MHz from HSI, no PLL here) */
    SystemClock_Config();

    /* Initialize GPIO (for LED on PA5) */
    MX_GPIO_Init();

    /* Initialize UART using HAL */
    uart_init();

    /* Print "UART is initialized" message */
    const char *initMsg = "UART is initialized with interrupts\r\n";
    uart_send((const uint8_t*)initMsg, strlen(initMsg));

    /* Main loop: blink LED */
    while (1)
    {
        /* Toggle the user LED on PA5 every 500ms */
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        HAL_Delay(500);

        /* Other tasks can be performed here without blocking */
        // No polling here
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

    /* Optional: start with LED off */
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

/* --------------------------------------------------------------------------
   IMPORTANT: SysTick Handler for the HAL time base
   --------------------------------------------------------------------------
   By default, STM32Cube projects include a weak SysTick_Handler() that calls 
   HAL_IncTick(). If yours doesn't, or if you need to override it, do:
*/

void SysTick_Handler(void)
{
    HAL_IncTick();
}
