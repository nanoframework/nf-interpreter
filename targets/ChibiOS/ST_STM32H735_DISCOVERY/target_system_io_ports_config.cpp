//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

///////////
// UART1 //
///////////

// pin configuration for UART1 (Arduino connector)
// port for TX pin is: GPIOB
// port for RX pin is: GPIOB
// TX pin: is GPIOB_14 (Arduino D1)
// RX pin: is GPIOB_15 (Arduino D0)
// GPIO alternate pin function is 4 (see alternate function mapping table in device datasheet)
UART_CONFIG_PINS(1, GPIOB, GPIOB, 14, 15, 4)

// initialization for UART1
UART_INIT(1)

// un-initialization for UART1
UART_UNINIT(1)
