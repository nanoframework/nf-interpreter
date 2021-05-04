//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>
#include <stm32l475e_iot01.h>
#include <stm32l4xx_ll_dma.h>

#include <tx_api.h>

#include <platform.h>
#include <platform_UARTDriver.h>

extern UART_HandleTypeDef WProtocolUart;
extern TX_EVENT_FLAGS_GROUP wpUartEvent;
// extern uint32_t receivedBytes;
extern uint32_t transmittedBytes;

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

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(NFC_GPIO_GPO_PIN);
}

void EXTI1_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI2_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(NFC_GPIO_GPO_PIN);
}

void EXTI15_10_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(USER_BUTTON_PIN);
}

void USART1_IRQHandler(void)
{
    // Check RXNE flag value in ISR register
    if(LL_USART_IsActiveFlag_RXNE(USART1) && LL_USART_IsEnabledIT_RXNE(USART1))
    {
        // RXNE flag will be cleared by reading of RDR register (done in call)
        WP_Reception_Callback();
    }
    else
    {
        // Call Error function
        USART_Error_Callback();
    }
}

void DMA1_Channel4_IRQHandler(void)
{
    if (LL_DMA_IsActiveFlag_TC7(DMA1))
    {
        LL_DMA_ClearFlag_GI7(DMA1);

        // set event flag
        tx_event_flags_set(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR);
    }
    else if (LL_DMA_IsActiveFlag_TE7(DMA1))
    {
        // Disable DMA1 Tx Channel
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_7);
    }
}

// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
// {
//     if (UartHandle->Instance == USART1)
//     {
//         // update count
//         transmittedBytes = UartHandle->TxXferSize - UartHandle->TxXferCount;
//     }
// }

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
// {
//     if (UartHandle->Instance == USART1)
//     {
//         // update count
//         receivedBytes = UartHandle->RxXferSize - UartHandle->RxXferCount;

//         // set event flag
//         tx_event_flags_set(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR);
//     }
// }
