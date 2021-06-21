//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>

#include <tx_api.h>

#include <targetHAL.h>
#include <nanoHAL_v2.h>
#include <platform_UARTDriver.h>
#include <WireProtocol_Message.h>
#include <WireProtocol.h>
#include <platform.h>

extern UART_HandleTypeDef WProtocolUart;

TX_EVENT_FLAGS_GROUP wpUartEvent;

uint32_t receivedBytes;
uint32_t transmittedBytes;

uint8_t WP_ReceiveBytes(uint8_t *ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;

    volatile uint32_t read;

    // check for request with 0 size
    if (*size)
    {
        // read from serial stream with 20ms timeout
        read = nano_HAL_UART_ReadTimeout(ptr, requestedSize, 20);

        TRACE(TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, receivedBytes);

        // check if any bytes where read
        if(read == 0)
        {
            return false;
        }

        ptr += read;
        *size -= read;
    }

    return true;
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    uint32_t dummy;
    uint8_t waitResult;
    uint8_t success = false;

    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header.m_size);

    // reset var
    transmittedBytes = 0;

    // write header with 250ms timeout
    if (HAL_UART_Transmit_DMA(&WProtocolUart, (uint8_t *)&message->m_header, sizeof(message->m_header)) != HAL_OK)
    {
        goto complete_operation;
    }

    // wait for event
    waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR_CLEAR, &dummy, TX_TICKS_PER_MILLISEC(10));

    if (waitResult != TX_SUCCESS || transmittedBytes != sizeof(message->m_header))
    {
        goto complete_operation;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        // reset var
        transmittedBytes = 0;

        if (HAL_UART_Transmit_DMA(&WProtocolUart, (uint8_t *)message->m_payload, message->m_header.m_size) != HAL_OK)
        {
            goto complete_operation;
        }

        // wait for event
        waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR_CLEAR, &dummy, TX_TICKS_PER_MILLISEC(250));
    }

    if (waitResult == TX_SUCCESS && transmittedBytes == message->m_header.m_size)
    {
        success = true;
    }

complete_operation:
    // stop any ongoing DMA operation
    //HAL_UART_DMAStop(&WProtocolUart);
    
    // done here
    return success;
}
