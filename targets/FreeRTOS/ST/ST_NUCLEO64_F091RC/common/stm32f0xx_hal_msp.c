//
// Copyright (c) 2020 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stm32f0xx_hal.h>

extern UART_HandleTypeDef WPUartHandle;

void HAL_UART_MspInit(UART_HandleTypeDef *uart)
{
    if (uart->Instance == USART2)
    {
        GPIO_InitTypeDef gpioInit;

        // Enable GPIO TX/RX clock
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        // Enable USARTx clock
        __HAL_RCC_USART2_CLK_ENABLE();

        // UART TX & RX GPIO pins configuration
        gpioInit.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        gpioInit.Mode = GPIO_MODE_AF_PP;
        gpioInit.Pull = GPIO_PULLUP;
        gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;
        gpioInit.Alternate = GPIO_AF1_USART2;

        HAL_GPIO_Init(GPIOA, &gpioInit);

        // Configure the NVIC for UART
        HAL_NVIC_SetPriority(USART2_IRQn, 0, 1);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *uart)
{
    if (uart->Instance == USART2)
    {  
        // Reset peripherals
        __HAL_RCC_USART1_FORCE_RESET();
        __HAL_RCC_USART1_RELEASE_RESET();

        // Disable peripherals and GPIO Clocks 
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
        // Configure UART Rx as alternate function
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);

        // Disable the NVIC for UART
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
}

void STM32_UART_Init()
{
    WPUartHandle.Instance        = USART2;

    WPUartHandle.Init.BaudRate   = 460800;
    WPUartHandle.Init.WordLength = UART_WORDLENGTH_8B;
    WPUartHandle.Init.StopBits   = UART_STOPBITS_1;
    WPUartHandle.Init.Parity     = UART_PARITY_NONE;
    WPUartHandle.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
    WPUartHandle.Init.Mode       = UART_MODE_TX_RX;
    WPUartHandle.Init.OverSampling = UART_OVERSAMPLING_8;
    WPUartHandle.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    WPUartHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if(HAL_UART_DeInit(&WPUartHandle) != HAL_OK)
    {
        //Error_Handler();
    }  
    if(HAL_UART_Init(&WPUartHandle) != HAL_OK)
    {
        //Error_Handler();
    }
}

void STM32_GPIO_Init()
{
    GPIO_InitTypeDef  gpioInit;

    // user button
    gpioInit.Mode  = GPIO_MODE_INPUT;
    gpioInit.Pull  = GPIO_NOPULL;
    gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;

    gpioInit.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOA, &gpioInit);

    // Configure IOs in output push-pull mode to drive external LEDs
    gpioInit.Mode  = GPIO_MODE_OUTPUT_PP;
    gpioInit.Pull  = GPIO_PULLUP;
    gpioInit.Speed = GPIO_SPEED_FREQ_HIGH;

    gpioInit.Pin = GPIO_PIN_5;
    HAL_GPIO_Init(GPIOA, &gpioInit);
}
