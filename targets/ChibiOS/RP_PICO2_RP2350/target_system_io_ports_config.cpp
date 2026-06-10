//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

////////////
// USART1 //
////////////

// Default RP2350 COM1 pin configuration:
// TX = GP0
// RX = GP1
void ConfigPins_UART1()
{
    palSetPadMode(IOPORT1, 0, PAL_MODE_ALTERNATE_UART);
    palSetPadMode(IOPORT1, 1, PAL_MODE_ALTERNATE_UART);
}

// initialization for UART1 (mapped to RP2350 UART0)
UART_INIT(1)

// un-initialization for UART1 (mapped to RP2350 UART0)
UART_UNINIT(1)

////////////
// USART2 //
////////////

// Default RP2350 COM2 pin configuration:
// TX = GP8
// RX = GP9
void ConfigPins_UART2()
{
    palSetPadMode(IOPORT1, 8, PAL_MODE_ALTERNATE_UART);
    palSetPadMode(IOPORT1, 9, PAL_MODE_ALTERNATE_UART);
}

// initialization for UART2 (mapped to RP2350 UART1)
UART_INIT(2)

// un-initialization for UART2 (mapped to RP2350 UART1)
UART_UNINIT(2)