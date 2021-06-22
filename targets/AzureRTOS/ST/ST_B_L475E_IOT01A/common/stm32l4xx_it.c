//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>
#include <stm32l475e_iot01.h>

#include <platform.h>
#include <platform_UARTDriver.h>

extern UART_HandleTypeDef WProtocolUart;

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

    if(LL_USART_IsEnabledIT_TXE(USART1) && LL_USART_IsActiveFlag_TXE(USART1))
    {
        // TXE flag will be automatically cleared when writing new data in TDR register

        // Call function in charge of handling empty DR => will lead to transmission of next character
        WP_Transmition_Callback();
    }
  
    if(LL_USART_IsEnabledIT_TC(USART1) && LL_USART_IsActiveFlag_TC(USART1))
    {
        // Clear TC flag
        LL_USART_ClearFlag_TC(USART1);
        
        // Call function in charge of handling end of transmission of sent character
        // and prepare next character transmission 
        WP_TransmitionComplete_Callback();
    }

    if(LL_USART_IsEnabledIT_ERROR(USART1) && LL_USART_IsActiveFlag_NE(USART1))
    {
        /* Call Error function */
        USART_Error_Callback();
    }
}
