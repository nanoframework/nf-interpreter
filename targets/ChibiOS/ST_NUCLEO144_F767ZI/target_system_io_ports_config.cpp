//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

///////////
// UART1 //
///////////

// NOT AVAILABLE

///////////
// UART2 //
///////////

// pin configuration for UART2
// port for TX pin is: GPIOD
// port for RX pin is: GPIOD
// TX pin: is GPIOD_5
// RX pin: is GPIOC_6
// GPIO alternate pin function is 7 (see alternate function mapping table in device datasheet)
UART_CONFIG_PINS(2, GPIOD, GPIOD, 5, 6, 7)

// initialization for UART2
UART_INIT(2)

// un-initialization for UART2
UART_UNINIT(2)

///////////
// UART3 //
///////////

// Used for STLINK.

// ///////////
// // UART4 //
// ///////////

// NOT AVAILABLE

///////////
// UART5 //
///////////

// NOT AVAILABLE

///////////
// UART6 //
///////////

// pin configuration for UART6
// port for TX pin is: GPIOG
// port for RX pin is: GPIOG
// TX pin: is GPIOC_14
// RX pin: is GPIOC_9
// GPIO alternate pin function is 8 (see alternate function mapping table in device datasheet)
UART_CONFIG_PINS(6, GPIOG, GPIOG, 14, 9, 8)

// initialization for UART6
UART_INIT(6)

// un-initialization for UART6
UART_UNINIT(6)

///////////
// UART7 //
///////////

// NOT AVAILABLE

///////////
// UART8 //
///////////

// NOT AVAILABLE
