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

TX_EVENT_FLAGS_GROUP wpUartEvent;

#define WP_UART_FLAG    0x80000000

uint32_t receivedBytes;
uint32_t transmittedBytes;

void UART_RxCallback(mxc_uart_req_t *req, int error)
{
    if(error == E_SUCCESS)
    {
        receivedBytes = req->rxCnt;

        // use event flags group as a variable to transmit the amount of received bytes
        tx_event_flags_set(&wpUartEvent, WP_UART_FLAG, TX_OR);
    }
}

void UART_TxCallback(mxc_uart_req_t *req, int error)
{
    if(error == E_SUCCESS)
    {
        transmittedBytes = req->txCnt;

        // use event flags group as a variable to transmit the amount of received bytes
        tx_event_flags_set(&wpUartEvent, WP_UART_FLAG, TX_OR);
    }
}

bool WP_ReceiveBytes(uint8_t *ptr, uint16_t *size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;
    // reset value
    receivedBytes = 0;
    uint8_t waitResult;
    // receive DMA request
    mxc_uart_req_t rxRequest;
    uint32_t dummy;

    // check for request with 0 size
    if (*size)
    {
        rxRequest.uart = MXC_UART_GET_UART(WIRE_PROTOCOL_UART);
        rxRequest.rxData = ptr;
        rxRequest.rxLen = requestedSize;
        rxRequest.txLen = 0;
        rxRequest.callback = UART_RxCallback;

        // reset var
        receivedBytes = 0;

        if (NanoUART_TransactionAsync(&rxRequest) == E_NO_ERROR)
        {
            // wait for event
            waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_FLAG, TX_OR_CLEAR, &dummy, 20 );
        }
        else
        {
            goto abort_rx;
        }

        if(waitResult != TX_SUCCESS)
        {
            goto abort_rx;
        }

        ptr += receivedBytes;
        *size -= receivedBytes;

        TRACE(TRACE_STATE, "RXMSG: Expecting %d bytes, received %d.\n", requestedSize, receivedBytes);

        // check if any bytes where read
        return receivedBytes > 0;
    }

    return true;

abort_rx:
    // abort any ongoing UART operation
    NanoUART_AbortAsync(MXC_UART_GET_UART(WIRE_PROTOCOL_UART));

    // done here
    return false;
}

bool WP_TransmitMessage(WP_Message *message)
{
    mxc_uart_req_t txRequest;
    uint8_t waitResult;
    uint32_t dummy;

    TRACE(
        TRACE_HEADERS,
        "TXMSG: 0x%08X, 0x%08X, 0x%08X\n",
        message->m_header.m_cmd,
        message->m_header.m_flags,
        message->m_header.m_size);

    // setup transmit request
    txRequest.uart = MXC_UART_GET_UART(WIRE_PROTOCOL_UART);
    txRequest.txData = (uint8_t *)&message->m_header;
    txRequest.txLen = sizeof(message->m_header);
    txRequest.rxLen = 0;
    txRequest.callback = UART_TxCallback;

    LED_Toggle(LED2);

    // reset var
    transmittedBytes = 0;

    // write header
    if (NanoUART_TransactionAsync(&txRequest) != E_NO_ERROR)
    {
        goto abort_tx;
    }

    // wait for event
    waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_FLAG, TX_OR_CLEAR, &dummy, 15);

    if(waitResult != TX_SUCCESS ||
       transmittedBytes != txRequest.txLen)
    {
        goto abort_tx;
    }

    // if there is anything on the payload send it to the output stream
    if (message->m_header.m_size && message->m_payload)
    {
        // setup transmit request
        txRequest.txData = (uint8_t *)message->m_payload;
        txRequest.txLen = message->m_header.m_size;

        // reset var
        transmittedBytes = 0;

        if (NanoUART_TransactionAsync(&txRequest) != E_NO_ERROR)
        {
            goto abort_tx;
        }

        // wait for event
        waitResult = tx_event_flags_get(&wpUartEvent, WP_UART_FLAG, TX_OR_CLEAR, &dummy, 60);
    }

    if(waitResult != TX_SUCCESS||
       transmittedBytes != txRequest.txLen)
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
