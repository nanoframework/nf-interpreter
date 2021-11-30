//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// #include <FreeRTOS.h>
#include <ti/drivers/UART2.h>
#include <ti/sysbios/knl/Clock.h>
#include <board.h>
#include <nanoHAL_v2.h>
#include <WireProtocol.h>
#include <WireProtocol_Message.h>
#include <WireProtocol_HAL_interface.h>

// UART operations timeout
#define UART_TIMEOUT_MILLISECONDS 500000

UART2_Handle uart = NULL;

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;
    size_t read;

    // check for request with 0 size
    if (*size)
    {
        // non blocking read from serial port with 500ms timeout
        UART2_readTimeout(uart, *ptr, requestedSize, &read, UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod);

        *ptr += read;
        *size -= read;
    }
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    uint32_t writeResult;
    bool operationResult = false;

    TRACE_WP_HEADER(WP_TXMSG, message);

    // write header to uart
    UART2_writeTimeout(
        uart,
        (const void *)&message->m_header,
        sizeof(message->m_header),
        &writeResult,
        UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod);

    if (writeResult == sizeof(message->m_header))
    {
        operationResult = true;

        // if there is anything on the payload send it to the output stream
        if (message->m_header.m_size && message->m_payload)
        {
            // reset flag
            operationResult = false;

            UART2_writeTimeout(
                uart,
                (const void *)message->m_payload,
                message->m_header.m_size,
                &writeResult,
                UART_TIMEOUT_MILLISECONDS / Clock_tickPeriod);

            if (writeResult == message->m_header.m_size)
            {
                operationResult = true;

                TRACE0(TRACE_ERRORS, "TXMSG: OK\n");
            }
        }
    }

    return operationResult;
}
