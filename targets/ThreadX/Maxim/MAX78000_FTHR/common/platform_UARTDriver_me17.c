//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
// See LICENSE file in the project root for full license information.
//

#include <platform_UARTDriver.h>
#include <platformHAL.h>

#include <nanoRingBuffer.h>

extern NanoRingBuffer WPRingBuffer;

#define MXC_UART_REVB_ERRINT_EN                                                                                        \
    (MXC_F_UART_REVB_INT_EN_RX_FERR | MXC_F_UART_REVB_INT_EN_RX_PAR | MXC_F_UART_REVB_INT_EN_RX_OV)

#define MXC_UART_REVB_ERRINT_FL                                                                                        \
    (MXC_F_UART_REVB_INT_FL_RX_FERR | MXC_F_UART_REVB_INT_FL_RX_PAR | MXC_F_UART_REVB_INT_FL_RX_OV)

static void *NanoAsyncTxRequests[MXC_UART_INSTANCES] = {NULL, NULL, NULL, NULL};
static void *NanoAsyncRxRequests[MXC_UART_INSTANCES] = {NULL, NULL, NULL, NULL};

int NanoUART_RevB_TransactionAsync(mxc_uart_revb_req_t *req)
{
    uint32_t numToWrite, numToRead;
    int uartNum = MXC_UART_GET_IDX((mxc_uart_regs_t *)(req->uart));

    if (uartNum < 0)
    {
        return E_INVALID;
    }

    if (NanoAsyncRxRequests[uartNum] && req->rxLen)
    {
        /* RX request pending */
        return E_BUSY;
    }

    if (NanoAsyncTxRequests[uartNum] && req->txLen)
    {
        /* TX request pending */
        return E_BUSY;
    }

    req->txCnt = 0;
    req->rxCnt = 0;

    if (req->txLen)
    {
        if (req->txData == NULL)
        {
            return E_BAD_PARAM;
        }

        // MXC_UART_EnableInt((mxc_uart_regs_t*)(req->uart), MXC_F_UART_REVB_INT_EN_TX_HE);
        numToWrite = MXC_UART_GetTXFIFOAvailable((mxc_uart_regs_t *)(req->uart));
        numToWrite = numToWrite > (req->txLen - req->txCnt) ? req->txLen - req->txCnt : numToWrite;
        req->txCnt += MXC_UART_WriteTXFIFO((mxc_uart_regs_t *)(req->uart), &req->txData[req->txCnt], numToWrite);

        /* If we're finished writing to the TX FIFO and it's less than half+1 full, pend the interrupt */
        if ((MXC_UART_GetTXFIFOAvailable((mxc_uart_regs_t *)(req->uart)) >= (MXC_UART_FIFO_DEPTH / 2)) &&
            (req->txCnt == req->txLen))
        {
            NVIC_SetPendingIRQ(MXC_UART_GET_IRQ(uartNum));
        }

        NanoAsyncTxRequests[MXC_UART_GET_IDX((mxc_uart_regs_t *)(req->uart))] = (void *)req;
    }
    else if (req->rxLen)
    {
        // All error interrupts are related to RX
        // MXC_UART_EnableInt((mxc_uart_regs_t*) (req->uart), MXC_UART_REVB_ERRINT_EN);

        if (req->rxData == NULL)
        {
            // MXC_UART_DisableInt((mxc_uart_regs_t*) (req->uart), 0xFFFFFFFF);
            // MXC_UART_ClearRXFIFO((mxc_uart_regs_t *)(req->uart));
            return E_BAD_PARAM;
        }

        // get any bytes that could be in the ring buffer
        req->rxCnt += NanoRingBuffer_PopN(&WPRingBuffer, &req->rxData[req->rxCnt], req->rxLen);

        if (req->rxCnt < req->rxLen)
        {
            // not all requested bytes were read
            numToRead = MXC_UART_GetRXFIFOAvailable((mxc_uart_regs_t *)(req->uart));
            numToRead = numToRead > (req->rxLen - req->rxCnt) ? req->rxLen - req->rxCnt : numToRead;
            req->rxCnt += MXC_UART_ReadRXFIFO((mxc_uart_regs_t *)(req->uart), &req->rxData[req->rxCnt], numToRead);
            MXC_UART_ClearFlags((mxc_uart_regs_t *)(req->uart), MXC_F_UART_REVB_INT_FL_RX_THD);
        }

        if (req->rxCnt == req->rxLen)
        {
            // read completed, call callback
            if (req->callback != NULL)
            {
                req->callback((mxc_uart_req_t *)req, E_SUCCESS);
            }
        }
        else
        {
            // not all requested bytes were read
            // add request
            NanoAsyncRxRequests[MXC_UART_GET_IDX((mxc_uart_regs_t *)(req->uart))] = (void *)req;
        }
    }

    return E_NO_ERROR;
}

int NanoUART_RevB_AsyncHandler(mxc_uart_revb_regs_t *uart)
{
    volatile uint32_t numToWrite, numToRead, flags;
    mxc_uart_req_t *req;
    uint8_t tmp;

    volatile int uartNum = MXC_UART_GET_IDX((mxc_uart_regs_t *)uart);

    if (uartNum < 0)
    {
        return E_INVALID;
    }

    flags = MXC_UART_GetFlags((mxc_uart_regs_t *)uart);

    //////////////
    // any error flags
    if (flags & MXC_UART_REVB_ERRINT_FL)
    {
        // clear flags
        MXC_UART_ClearFlags((mxc_uart_regs_t *)uart, MXC_UART_REVB_ERRINT_FL);

        // MXC_UART_AsyncStop((mxc_uart_regs_t*)uart);
        NanoUART_RevB_AsyncCallback(uart, E_COMM_ERR);

        return E_NO_ERROR;
    }
    ////////////////////////////
    // Transmit FIFO Half-Empty
    else if (flags & MXC_F_UART_REVB_INT_FL_TX_HE)
    {
        // clear flag
        MXC_UART_ClearFlags((mxc_uart_regs_t *)uart, MXC_F_UART_REVB_INT_FL_TX_HE);

        req = (mxc_uart_req_t *)NanoAsyncTxRequests[uartNum];
        if ((req != NULL) && (req->txLen))
        {
            numToWrite = MXC_UART_GetTXFIFOAvailable((mxc_uart_regs_t *)(req->uart));
            numToWrite = numToWrite > (req->txLen - req->txCnt) ? req->txLen - req->txCnt : numToWrite;
            req->txCnt += MXC_UART_WriteTXFIFO((mxc_uart_regs_t *)(req->uart), &req->txData[req->txCnt], numToWrite);

            // if request is complete, perform callback
            if (req->txCnt == req->txLen)
            {
                NanoUART_RevB_AsyncTxCallback(uartNum, E_SUCCESS);
            }
        }

        return E_NO_ERROR;
    }
    ////////////////////////////////
    // MXC_F_UART_REVB_INT_FL_RX_THD
    else if (flags & MXC_F_UART_REVB_INT_FL_RX_THD)
    {
        MXC_UART_ClearFlags((mxc_uart_regs_t *)uart, MXC_F_UART_REVB_INT_FL_RX_THD);

        req = (mxc_uart_req_t *)NanoAsyncRxRequests[uartNum];
        if ((req != NULL) && (flags & MXC_F_UART_REVB_INT_FL_RX_THD) && (req->rxLen))
        {
            numToRead = MXC_UART_GetRXFIFOAvailable((mxc_uart_regs_t *)(req->uart));
            numToRead = numToRead > (req->rxLen - req->rxCnt) ? req->rxLen - req->rxCnt : numToRead;
            req->rxCnt += MXC_UART_ReadRXFIFO((mxc_uart_regs_t *)(req->uart), &req->rxData[req->rxCnt], numToRead);

            // if ((req->rxLen - req->rxCnt) < MXC_UART_GetRXThreshold((mxc_uart_regs_t *)(req->uart)))
            // {
            //     MXC_UART_SetRXThreshold((mxc_uart_regs_t *)(req->uart), req->rxLen - req->rxCnt);
            // }

            // if request is complete, perform callback
            if (req->rxCnt == req->rxLen)
            {
                NanoUART_RevB_AsyncRxCallback(uartNum, E_SUCCESS);
            }

            return E_NO_ERROR;
        }

        if (uartNum == WIRE_PROTOCOL_UART)
        {
            // no rx request pending, add to ring buffer
            numToRead = MXC_UART_GetRXFIFOAvailable((mxc_uart_regs_t *)uart);

            if (numToRead)
            {
                // check available space
                numToRead = numToRead <= (WPRingBuffer._capacity - WPRingBuffer._size)
                                ? numToRead
                                : (WPRingBuffer._capacity - WPRingBuffer._size);

                while ((WPRingBuffer._capacity - WPRingBuffer._size) && numToRead > 0)
                {
                    MXC_UART_ReadRXFIFO((mxc_uart_regs_t *)uart, &tmp, 1);
                    NanoRingBuffer_Push(&WPRingBuffer, tmp);
                    numToRead--;
                }

                // if there are still bytes to read, flush them
                MXC_UART_ClearRXFIFO((mxc_uart_regs_t *)uart);
            }
        }
    }

    return E_NO_ERROR;
}

int NanoUART_RevB_AbortAsync(mxc_uart_revb_regs_t *uart)
{
    if (MXC_UART_GET_IDX((mxc_uart_regs_t *)uart) < 0)
    {
        return E_BAD_PARAM;
    }

    NanoUART_RevB_AsyncCallback((mxc_uart_revb_regs_t *)uart, E_ABORT);

    return E_NO_ERROR;
}

int NanoUART_RevB_AsyncCallback(mxc_uart_revb_regs_t *uart, int retVal)
{
    int uartNum = MXC_UART_GET_IDX((mxc_uart_regs_t *)uart);

    // AsyncTxCallback
    NanoUART_RevB_AsyncTxCallback(uartNum, retVal);

    // AsyncRxCallback
    NanoUART_RevB_AsyncRxCallback(uartNum, retVal);

    return E_NO_ERROR;
}

int NanoUART_RevB_AsyncTxCallback(int uartNum, int retVal)
{
    mxc_uart_req_t *req = (mxc_uart_req_t *)NanoAsyncTxRequests[uartNum];
    if (req != NULL)
    {
        if (req->callback != NULL)
        {
            req->callback(req, retVal);
        }

        // remove request
        NanoAsyncTxRequests[uartNum] = NULL;
    }

    return E_NO_ERROR;
}

int NanoUART_RevB_AsyncRxCallback(int uartNum, int retVal)
{
    mxc_uart_req_t *req = (mxc_uart_req_t *)NanoAsyncRxRequests[uartNum];
    if (req != NULL)
    {
        if (req->callback != NULL)
        {
            req->callback(req, retVal);
        }

        // remove request
        NanoAsyncRxRequests[uartNum] = NULL;
    }

    return E_NO_ERROR;
}

void NanoUART_InitRequest(mxc_uart_req_t *rxRequest)
{
    memset(rxRequest, 0, sizeof(mxc_uart_req_t));
}