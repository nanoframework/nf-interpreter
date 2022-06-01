//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <mxc_device.h>
#include <platform_UARTDriver.h>
#include <led.h>

#include <tx_api.h>

#include <WireProtocol_Message.h>
#include <WireProtocol.h>
#include <platformHAL.h>
#include <platform.h>

TX_EVENT_FLAGS_GROUP wpUartEvent;

uint32_t receivedBytes;
uint32_t transmittedBytes;

void UART_RxCallback(mxc_uart_req_t *req, int error)
{
    if (error == E_SUCCESS)
    {
        receivedBytes = req->rxCnt;

        // use event flags group as a variable to transmit the amount of received bytes
        tx_event_flags_set(&wpUartEvent, WP_UART_RX_EVENT_FLAG, TX_OR);
    }
}

void UART_TxCallback(mxc_uart_req_t *req, int error)
{
    if (error == E_SUCCESS)
    {
        transmittedBytes = req->txCnt;

        // use event flags group as a variable to transmit the amount of received bytes
        tx_event_flags_set(&wpUartEvent, WP_UART_RX_EVENT_FLAG, TX_OR);
    }
}

void WP_ReceiveBytes(uint8_t **ptr, uint32_t *size)
{
    // save for later comparison
    uint32_t requestedSize = *size;
    // reset value
    receivedBytes = 0;
    uint8_t waitResult;
    // receive DMA request
    mxc_uart_req_t rxRequest;
    uint32_t dummy;

    // check for request with 0 size
    if (*size)
    {
        NanoUART_InitRequest(&rxRequest);

        rxRequest.uart = MXC_UART_GET_UART(WIRE_PROTOCOL_UART);
        rxRequest.rxData = *ptr;
        rxRequest.rxLen = requestedSize;
        rxRequest.callback = UART_RxCallback;

        // reset var
        receivedBytes = 0;

        if (NanoUART_TransactionAsync(&rxRequest) == E_NO_ERROR)
        {
            // wait for event
            waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_RX_EVENT_FLAG, TX_OR_CLEAR, &dummy, 20);
        }
        else
        {
            goto abort_rx;
        }

        if (waitResult != TX_SUCCESS)
        {
            goto abort_rx;
        }

        *ptr += receivedBytes;
        *size -= receivedBytes;

        // Warning: Includeing TRACE_VERBOSE will NOT output the following TRACE on every loop
        //          of the statemachine to avoid flooding the trace.
        TRACE_LIMIT(TRACE_VERBOSE, 100, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, receivedBytes);
    }

    return;

abort_rx:
    // abort any ongoing UART operation
    NanoUART_AbortAsync(MXC_UART_GET_UART(WIRE_PROTOCOL_UART));

    // done here
    return;
}

uint8_t WP_TransmitMessage(WP_Message *message)
{
    mxc_uart_req_t txRequest;
    uint8_t waitResult;
    uint32_t dummy;

    TRACE_WP_HEADER(WP_TXMSG, message);

    NanoUART_InitRequest(&txRequest);

    // setup transmit request
    txRequest.uart = MXC_UART_GET_UART(WIRE_PROTOCOL_UART);
    txRequest.callback = UART_TxCallback;
    txRequest.txData = (uint8_t *)&message->m_header;
    txRequest.txLen = sizeof(message->m_header);

    LED_Toggle(LED2);

    // reset var
    transmittedBytes = 0;

    // write header
    if (NanoUART_TransactionAsync(&txRequest) != E_NO_ERROR)
    {
        goto abort_tx;
    }

    // wait for event
    waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_RX_EVENT_FLAG, TX_OR_CLEAR, &dummy, 15);

    if (waitResult != TX_SUCCESS || transmittedBytes != txRequest.txLen)
    {
        goto abort_tx;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        NanoUART_InitRequest(&txRequest);

        // setup transmit request
        txRequest.uart = MXC_UART_GET_UART(WIRE_PROTOCOL_UART);
        txRequest.callback = UART_TxCallback;
        txRequest.txData = (uint8_t *)message->m_payload;
        txRequest.txLen = message->m_header.m_size;

        // reset var
        transmittedBytes = 0;

        if (NanoUART_TransactionAsync(&txRequest) != E_NO_ERROR)
        {
            goto abort_tx;
        }

        // wait for event
        waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_RX_EVENT_FLAG, TX_OR_CLEAR, &dummy, 60);
    }

    if (waitResult != TX_SUCCESS || transmittedBytes != txRequest.txLen)
    {
        goto abort_tx;
    }

    return true;

abort_tx:
    // abort any ongoing UART operation
    NanoUART_AbortAsync(MXC_UART_GET_UART(WIRE_PROTOCOL_UART));

    // done here
    return false;
}
