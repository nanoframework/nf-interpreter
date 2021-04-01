//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <mxc_device.h>
#include <uart.h>
#include <dma.h>
#include <mxc_delay.h>
#include <led.h>

#include <tx_api.h>

#include <WireProtocol_Message.h>
#include <WireProtocol.h>
#include <platformHAL.h>

TX_EVENT_FLAGS_GROUP wpUartEvent;

WP_Message inboundMessage;
uint32_t receivedBytes;
uint32_t transmittedBytes;

void UART_RxCallback(mxc_uart_req_t *req, int error)
{
    if(error == E_ABORT)
    {
        //req = NULL;
        return;
    }

    if(error == E_SUCCESS)
    {
        if (MXC_UART_GET_IDX((mxc_uart_regs_t *)req->uart) == WIRE_PROTOCOL_UART)
        {
            // use event flags group as a variable to transmit the amount of received bytes
            tx_event_flags_set(&wpUartEvent, req->rxCnt, TX_OR);

            //req = NULL;
        }
    }
}

void UART_TxCallback(mxc_uart_req_t *req, int error)
{
    if(error == E_ABORT)
    {
        //req = NULL;
        return;
    }

    if (MXC_UART_GET_IDX((mxc_uart_regs_t *)req->uart) == WIRE_PROTOCOL_UART)
    {
        //LED_Toggle(LED2);

        // use event flags group as a variable to transmit the amount of received bytes
        tx_event_flags_set(&wpUartEvent, req->txCnt, TX_OR);
    }

    //req = NULL;
}

int WP_ReceiveBytes(uint8_t *ptr, uint16_t *size)
{
    // save for latter comparison
    uint16_t requestedSize = *size;
    // reset value
    receivedBytes = 0;
    uint8_t waitResult;
    // receive DMA request
    mxc_uart_req_t rxRequest;

    // sanity check for request of 0 size
    if (*size)
    {
        rxRequest.uart = MXC_UART_GET_UART(WIRE_PROTOCOL_UART);
        rxRequest.rxData = ptr;
        rxRequest.rxLen = requestedSize;
        rxRequest.txLen = 0;
        rxRequest.callback = UART_RxCallback;

        // // //int l = 6;
//         // MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(WIRE_PROTOCOL_UART));
//         // setup transmit request
//         mxc_uart_req_t txRequest;
//         txRequest.uart = MXC_UART_GET_UART(WIRE_PROTOCOL_UART);
//         txRequest.txData = (const uint8_t *)"HELLO\n";
//         txRequest.txLen = 6;
//         txRequest.rxLen = 0;
//         txRequest.callback = UART_TxCallback;

//         MXC_UART_TransactionAsync(&txRequest);

// tx_thread_sleep(1000);
//         //MXC_Delay(200000);

        // MXC_UART_Write(MXC_UART_GET_UART(WIRE_PROTOCOL_UART), (const uint8_t *)"HELLO\n", &l);
        // LED_Toggle(LED2);

        //MXC_UART_ClearRXFIFO(rxRequest.uart);

        if (MXC_UART_TransactionAsync(&rxRequest) == E_NO_ERROR)
        {
            // wait for event
            waitResult = tx_event_flags_get(&wpUartEvent, 0xFFFF, TX_OR_CLEAR, &receivedBytes, 10);
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

        // check if the requested read matches the actual read count
        return (requestedSize == receivedBytes);
    }

    return true;

abort_rx:
    // abort any ongoing UART operation
    MXC_UART_AbortAsync(MXC_UART_GET_UART(WIRE_PROTOCOL_UART));
    // done here
    return false;
}

int WP_TransmitMessage(WP_Message *message)
{
    // transmit DMA request
    mxc_uart_req_t txRequest;
    uint8_t waitResult;

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

    // MXC_UART_ClearTXFIFO(txRequest.uart);
        LED_Toggle(LED2);

    // write header with 250ms timeout
    if (MXC_UART_TransactionAsync(&txRequest) != E_SUCCESS)
    {
        goto abort_tx;
    }

    // wait for event
    waitResult = tx_event_flags_get(&wpUartEvent, 0xFFFF, TX_OR_CLEAR, &transmittedBytes, 10);

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

        if (MXC_UART_TransactionAsync(&txRequest) != E_SUCCESS)
        {
            goto abort_tx;
        }

        // wait for event
        waitResult = tx_event_flags_get(&wpUartEvent, 0xFFFF, TX_OR_CLEAR, &transmittedBytes, 50);
    }

    if(waitResult != TX_SUCCESS||
       transmittedBytes != txRequest.txLen)
    {
        goto abort_tx;
    }
    
    return true;

abort_tx:
    // abort any ongoing UART operation
    MXC_UART_AbortAsync(MXC_UART_GET_UART(WIRE_PROTOCOL_UART));
    // done here
    return false;
}
