//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>
#include <stm32l475e_iot01.h>

#include <tx_api.h>

extern UART_HandleTypeDef WProtocolUart;
extern TX_EVENT_FLAGS_GROUP wpUartEvent;

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

// void NMI_Handler(void)
// {
// }

// void HardFault_Handler(void)
// {
//   /* Go to infinite loop when Hard Fault exception occurs */
//   while (1)
//   {
//   }
// }

void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {
    }
}

void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {
    }
}

void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {
    }
}

// void DebugMon_Handler(void)
// {
// }

//////////////////////////////////
// defined in ThreadX low level //
// void PendSV_Handler(void)     //
//////////////////////////////////

//////////////////////////////////
// defined in ThreadX low level //
// void SysTick_Handler(void)   //
//////////////////////////////////

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(NFC_GPIO_GPO_PIN);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&WProtocolUart);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
    if (UartHandle->Instance == USART1)
    {
        // use event flags group as a variable to transmit the amount of received bytes
        tx_event_flags_set(&wpUartEvent, UartHandle->RxXferSize - UartHandle->RxXferCount, TX_OR);
    }
}

// void TIM1_CC_IRQHandler(void)
// {
//   HAL_TIM_IRQHandler(&TimCCHandle);
// }
