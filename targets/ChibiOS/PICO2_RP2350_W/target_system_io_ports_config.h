//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_SYSTEM_IO_PORTS_CONFIG_H
#define TARGET_SYSTEM_IO_PORTS_CONFIG_H

// Expose RP2350 UART0/UART1 as COM1/COM2 for the ChibiOS System.IO.Ports implementation.
#define NF_SERIAL_COMM_STM32_UART_USE_USART1 TRUE
#define NF_SERIAL_COMM_STM32_UART_USE_USART2 TRUE

// Compatibility alias for shared code paths that still reference UARTD2 directly.
#if !defined(UARTD2)
#define UARTD2 UARTD1
#endif

#endif // TARGET_SYSTEM_IO_PORTS_CONFIG_H