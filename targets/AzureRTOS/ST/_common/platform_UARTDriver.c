//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) 2017 STMicroelectronics.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <platform_UARTDriver.h>

#include <tx_api.h>

// RX control vars
uint32_t rxOperationSize;
uint32_t rxOperationCount;
uint8_t *rxOperationBuffer;

// TX control vars
uint32_t txOperationSize;
uint32_t txOperationCount;
uint8_t *txOperationBuffer;

void WP_Reception_Callback()
{
    uint8_t rxChar = LL_USART_ReceiveData8(USART1);

    // reception active?
    if (rxOperationCount > 0)
    {
        // ongoing RX operation
        *rxOperationBuffer = rxChar;
        rxOperationBuffer++;
        rxOperationCount--;

        if (rxOperationCount == 0U)
        {
            // RX complete, signal event
            tx_event_flags_set(&wpUartEvent, WP_UART_RX_EVENT_FLAG, TX_OR);
        }
    }
    else
    {
        // store in ring buffer
        NanoRingBuffer_Push(&WPRingBuffer, rxChar);
    }
}

void WP_Transmition_Callback()
{
    if(txOperationCount == 1)
    {
        // Disable TXE interrupt
        LL_USART_DisableIT_TXE(USART1);
        
        // Enable TC interrupt
        LL_USART_EnableIT_TC(USART1);
    }

    // Fill TDR with a new char
    LL_USART_TransmitData8(USART1, *txOperationBuffer);

    txOperationBuffer++;
    txOperationCount--;
}

void WP_TransmitionComplete_Callback()
{
    // Disable TC interrupt
    LL_USART_DisableIT_TC(USART1);

    // TX complete, signal event
    tx_event_flags_set(&wpUartEvent, WP_UART_TX_EVENT_FLAG, TX_OR);
}

uint32_t nano_HAL_UART_ReadTimeout(uint8_t *ptr, uint32_t requestedSize, uint32_t timeout)
{
    TX_INTERRUPT_SAVE_AREA
    
    uint32_t readBytes;
    uint32_t dummy;

    // are there enough bytes in the ring buffer?
    rxOperationSize = requestedSize;
    rxOperationCount = requestedSize;
    rxOperationBuffer = ptr;

    TX_DISABLE

    // try read whatever is in the ring buffer
    readBytes = NanoRingBuffer_PopN(&WPRingBuffer, rxOperationBuffer, requestedSize);
    rxOperationCount -= readBytes;
    
    // update pointer
    rxOperationBuffer += readBytes;

    TX_RESTORE

    if (rxOperationCount > 0)
    {
        // wait for event
        tx_event_flags_get(&wpUartEvent, WP_UART_RX_EVENT_FLAG, TX_OR_CLEAR, &dummy, TX_TICKS_PER_MILLISEC(timeout));

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

uint32_t nano_HAL_UART_SendTimeout(uint8_t *ptr, uint32_t requestedSize, uint32_t timeout)
{
    uint32_t txBytes;
    uint32_t dummy;

    // are there enough bytes in the ring buffer?
    txOperationSize = requestedSize;
    txOperationCount = requestedSize;
    txOperationBuffer = ptr;
    
    if (txOperationCount > 0)
    {
        // Start USART transmission : Will initiate TXE interrupt after TDR register is empty
        LL_USART_TransmitData8(USART1, *txOperationBuffer);

        txOperationBuffer++;
        txOperationCount--;

        // Enable TXE interrupt
        LL_USART_EnableIT_TXE(USART1); 

        // wait for event
        tx_event_flags_get(&wpUartEvent, WP_UART_TX_EVENT_FLAG, TX_OR_CLEAR, &dummy, TX_TICKS_PER_MILLISEC(timeout));

        txBytes = txOperationSize - txOperationCount;
    }
    else
    {
        txBytes = requestedSize;
    }

    // zero counters
    txOperationSize = 0;
    txOperationCount = 0;

    return txBytes;
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

    // reception active?
    if (rxOperationCount > 0)
    {
        rxOperationCount = 0;        
    }

    if(txOperationCount > 0)
    {
        txOperationCount = 0;
        
        // Disable TXE interrupt
        LL_USART_DisableIT_TXE(USART1);
        
        // Disable TC interrupt
        LL_USART_DisableIT_TC(USART1); 
    }
}
