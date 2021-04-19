//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>

#include <tx_api.h>

#include <WireProtocol_Message.h>
#include <WireProtocol.h>

extern UART_HandleTypeDef WProtocolUart;

TX_EVENT_FLAGS_GROUP wpUartEvent;

WP_Message inboundMessage;
uint32_t receivedBytes;
uint32_t transmittedBytes;

bool WP_ReceiveBytes(uint8_t *ptr, uint16_t *size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;
    // reset value
    receivedBytes = 0;

    // check for request with 0 size
    if (*size)
    {
        // read from serial stream with 250ms timeout
        if (HAL_UART_Receive_DMA(&WProtocolUart, ptr, *size) == HAL_OK)
        {
            // wait for event
            tx_event_flags_get(&wpUartEvent, 0xFFFF, TX_OR_CLEAR, &receivedBytes, 100);

            ptr += receivedBytes;
            *size -= receivedBytes;
        }

        // abort any ongoing UART operation
        HAL_UART_DMAStop(&WProtocolUart);

        TRACE(TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, receivedBytes);

        // check if any bytes where read
        return receivedBytes > 0;
    }

    return true;
}

bool WP_TransmitMessage(WP_Message *message)
{
    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header.m_size);

    // write header with 250ms timeout
    if (HAL_UART_Transmit_DMA(&WProtocolUart, (uint8_t *)&message->m_header, sizeof(message->m_header)) != HAL_OK)
    {
        return false;
    }
    
    // wait for event
    tx_event_flags_get(&wpUartEvent, 0xFFFF, TX_OR_CLEAR, &transmittedBytes, 25);

    HAL_UART_DMAStop(&WProtocolUart);

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        if (HAL_UART_Transmit_DMA(&WProtocolUart, (uint8_t *)message->m_payload, message->m_header.m_size) != HAL_OK)
        {
            return false;
        }

        // wait for event
        tx_event_flags_get(&wpUartEvent, 0xFFFF, TX_OR_CLEAR, &transmittedBytes, 25);

        HAL_UART_DMAStop(&WProtocolUart);
    }

    return true;
}
