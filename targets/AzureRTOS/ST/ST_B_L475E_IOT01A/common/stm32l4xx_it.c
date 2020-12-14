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

///////////////////////////////////
// defined in hard fault handler //
// void NMI_Handler(void)        //
///////////////////////////////////

///////////////////////////////////
// defined in hard fault handler //
// void HardFault_Handler(void)  //
///////////////////////////////////

///////////////////////////////////
// defined in hard fault handler //
// void MemManage_Handler(void)  //
///////////////////////////////////

///////////////////////////////////
// defined in hard fault handler //
// void BusFault_Handler(void)   //
///////////////////////////////////

///////////////////////////////////
// defined in hard fault handler //
// void UsageFault_Handler(void) //
///////////////////////////////////

///////////////////////////////////
// defined in hard fault handler //
// void DebugMon_Handler(void)   //
///////////////////////////////////

//////////////////////////////////
// defined in ThreadX low level //
// void PendSV_Handler(void)    //
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
