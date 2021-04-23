//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <stm32l4xx_hal.h>

#include <tx_api.h>

#include <WireProtocol_Message.h>
#include <WireProtocol.h>
#include <platform.h>

extern UART_HandleTypeDef WProtocolUart;

TX_EVENT_FLAGS_GROUP wpUartEvent;

uint32_t receivedBytes;
uint32_t transmittedBytes;

uint8_t WP_ReceiveBytes(uint8_t *ptr, uint16_t *size)
{
    // save for later comparison
    uint16_t requestedSize = *size;

    uint32_t dummy;
    uint8_t waitResult;

    // reset value
    receivedBytes = 0;

    // check for request with 0 size
    if (*size)
    {
        // read from serial stream with 250ms timeout
        if (HAL_UART_Receive_DMA(&WProtocolUart, ptr, requestedSize) == HAL_OK)
        {
            // wait for event
            waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR_CLEAR, &dummy, 100);

            if (waitResult == TX_SUCCESS)
            {
                ptr += receivedBytes;
                *size -= receivedBytes;
            }
        }

        // abort any ongoing UART operation
        HAL_UART_DMAStop(&WProtocolUart);

        TRACE(TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, receivedBytes);

        // check if any bytes where read
        return receivedBytes > 0;
    }

    return true;
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    uint32_t dummy;
    uint8_t waitResult;

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
        goto send_failed;
    }

    // wait for event
    waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR_CLEAR, &dummy, 10);

    if (waitResult != TX_SUCCESS || transmittedBytes != sizeof(message->m_header))
    {
        goto send_failed;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        // reset var
        transmittedBytes = 0;

        if (HAL_UART_Transmit_DMA(&WProtocolUart, (uint8_t *)message->m_payload, message->m_header.m_size) != HAL_OK)
        {
            goto send_failed;
        }

        // wait for event
        waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_EVENT_FLAG, TX_OR_CLEAR, &dummy, 250);
    }

    HAL_UART_DMAStop(&WProtocolUart);

    if (waitResult != TX_SUCCESS || transmittedBytes != message->m_header.m_size)
    {
        return false;
    }

    return true;

send_failed:
    HAL_UART_DMAStop(&WProtocolUart);
    return false;
}
