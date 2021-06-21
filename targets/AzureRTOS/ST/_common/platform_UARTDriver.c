//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <platform_UARTDriver.h>

#include <tx_api.h>

uint32_t rxOperationSize;
uint32_t rxOperationCount;
uint8_t *rxOperationBuffer;

void WP_Reception_Callback()
{
    // reception active?
    if (rxOperationCount > 0)
    {
        // ongoing RX operation
        *rxOperationBuffer = LL_USART_ReceiveData8(USART1);
        rxOperationBuffer++;
        rxOperationCount--;

        if (rxOperationCount == 0U)
        {
            // TX complete, signal event
            tx_event_flags_set(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR);
        }
    }
    else
    {
        // store in ring buffer
        uint8_t rxChar = LL_USART_ReceiveData8(USART1);
        NanoRingBuffer_Push(&WPRingBuffer, rxChar);
    }
}

uint32_t nano_HAL_UART_ReadTimeout(uint8_t *ptr, uint32_t requestedSize, uint32_t timeout)
{
    uint32_t readBytes;
    uint32_t dummy;

    // are there enough bytes in the ring buffer?
    rxOperationSize = requestedSize;
    rxOperationCount = requestedSize;
    rxOperationBuffer = ptr;

    // try read whatever is in the ring buffer
    readBytes = NanoRingBuffer_PopN(&WPRingBuffer, rxOperationBuffer, requestedSize);
    rxOperationCount -= readBytes;
    
    // update pointer
    rxOperationBuffer += readBytes;

    if (rxOperationCount > 0)
    {
        // not all requested bytes were read
        // uart->RxState = HAL_UART_STATE_BUSY_RX;

        // wait for event
        tx_event_flags_get(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR_CLEAR, &dummy, TX_TICKS_PER_MILLISEC(timeout));

        readBytes = rxOperationSize - rxOperationCount;

    }
    else
    {
        readBytes = requestedSize;
    }

    // zero counters
    rxOperationSize = 0;
    rxOperationCount = 0;

    return readBytes;
}

void USART_Error_Callback()
{
    __IO uint32_t isr_reg;

    isr_reg = LL_USART_ReadReg(USART1, ISR);

    if (isr_reg & LL_USART_ISR_NE)
    {
        // case Noise Error flag is raised : Clear NF Flag
        LL_USART_ClearFlag_NE(USART1);
    }
    else if (isr_reg & LL_USART_ISR_PE)
    {
        // Unexpected IT source
        LL_USART_ClearFlag_PE(USART1);
    }
    else if (isr_reg & LL_USART_ISR_FE)
    {
        // Unexpected IT source
        LL_USART_ClearFlag_FE(USART1);
    }
    else if (isr_reg & LL_USART_ISR_ORE)
    {
        // Unexpected IT source
        LL_USART_ClearFlag_ORE(USART1);
    }
}
