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
    uint32_t txBytes;
    uint8_t success = false;

    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header.m_size);

    // write header with 10ms timeout
    txBytes = nano_HAL_UART_SendTimeout((uint8_t *)&message->m_header, sizeof(message->m_header), 10);

    if(txBytes == sizeof(message->m_header))
    {
        // if there is anything on the payload, send it to the output stream
        if (message->m_header.m_size && message->m_payload)
        {
            txBytes = nano_HAL_UART_SendTimeout((uint8_t *)message->m_payload, message->m_header.m_size, 250);
            
            if (txBytes == message->m_header.m_size)
            {
                success = true;    
            }
        }
    }
    else
    {
        success = true;
    }
    
    return success;
}
