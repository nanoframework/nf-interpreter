//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _PLATFORM_UART_DRIVER_H_
#define _PLATFORM_UART_DRIVER_H_

#include <nanoCLR_Headers.h>
#include <nanoHAL_v2.h>

#include <mxc_device.h>
#include <uart.h>
#include <uart_revb.h>
#include <dma.h>

#ifdef __cplusplus
extern "C"
{
#endif

int NanoUART_AsyncHandler(mxc_uart_regs_t* uart);

int NanoUART_TransactionAsync(mxc_uart_req_t* req);

void NanoUART_EnableInt(mxc_uart_regs_t* uart);
int NanoUART_AbortAsync(mxc_uart_regs_t* uart);

// specific drivers
int NanoUART_RevB_TransactionAsync(mxc_uart_revb_req_t* req);
int NanoUART_RevB_AsyncHandler(mxc_uart_revb_regs_t* uart);
int NanoUART_RevB_AbortAsync(mxc_uart_revb_regs_t* uart);
int NanoUART_RevB_AsyncCallback(mxc_uart_revb_regs_t *uart, int retVal);
int NanoUART_RevB_AsyncTxCallback(int uartNum, int retVal);
int NanoUART_RevB_AsyncRxCallback(int uartNum, int retVal);
void NanoUART_InitRequest(mxc_uart_req_t *rxRequest);

#ifdef __cplusplus
}
#endif

#endif //_PLATFORM_UART_DRIVER_H_
