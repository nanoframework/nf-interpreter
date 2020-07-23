//
// Copyright (c) 2020 The nanoFramework project contributors
// Portions Copyright (c) 2016 STMicroelectronics. All rights reserved.
// See LICENSE file in the project root for full license information.
//

/**
 ******************************************************************************
 * @file    UART/UART_TwoBoards_ComPolling/Src/stm32f0xx_it.c
 * @author  MCD Application Team
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stm32f0xx.h>
#include <FreeRTOS.h>
#include <task.h>

extern UART_HandleTypeDef WPUartHandle;
extern TaskHandle_t xReceiverThreadHandle;

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief   This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {
    }
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&WPUartHandle);
}

/**
 * @brief  This function handles External EXTI_Line4_15 interrupt request.
 * @param  None
 * @retval None
 */
void EXTI4_15_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (UartHandle->Instance == USART2)
    {
        xTaskNotifyFromISR(xReceiverThreadHandle,
                           UartHandle->RxXferSize - UartHandle->RxXferCount,
                           eSetValueWithOverwrite,
                           &xHigherPriorityTaskWoken); 
    }
}
