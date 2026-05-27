//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
// See LICENSE file in the project root for full license information.
//

#include <platform_UARTDriver.h>
#include <platformHAL.h>

int NanoUART_AsyncHandler(mxc_uart_regs_t* uart)
{
    return NanoUART_RevB_AsyncHandler((mxc_uart_revb_regs_t*) uart);
}

int NanoUART_TransactionAsync(mxc_uart_req_t* req)
{
    return NanoUART_RevB_TransactionAsync((mxc_uart_revb_req_t*) req);
}

void NanoUART_EnableInt(mxc_uart_regs_t *uart)
{
    //MXC_UART_EnableInt(uart, MXC_UART_REVB_ERRINT_EN);
    MXC_UART_EnableInt(uart, MXC_F_UART_REVB_INT_EN_TX_HE);
    MXC_UART_EnableInt(uart, MXC_F_UART_REVB_INT_EN_RX_THD);
}

int NanoUART_AbortAsync(mxc_uart_regs_t* uart)
{
    return NanoUART_RevB_AbortAsync((mxc_uart_revb_regs_t*) uart);
}
