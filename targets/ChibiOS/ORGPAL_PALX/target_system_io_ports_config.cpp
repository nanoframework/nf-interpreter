//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

///////////
// UART2 //
///////////

UART_CONFIG_PINS(2, GPIOD, GPIOD, 5, 6, 7)

// initialization for UART2
UART_INIT(2)

// un-initialization for UART2
UART_UNINIT(2)

///////////
// UART3 //
///////////

// pin configuration for UART3
UART_CONFIG_PINS(3, GPIOB, GPIOB, 10, 11, 7)

// initialization for UART3
UART_INIT(3)

// un-initialization for UART3
UART_UNINIT(3)

///////////
// UART6 //
///////////

// pin configuration for UART6
// port for TX pin is: GPIOC
// port for RX pin is: GPIOC
// TX pin: is GPIOC_6
// RX pin: is GPIOC_7
// GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate
// function mapping" in STM32F769I datasheet)
UART_CONFIG_PINS(6, GPIOC, GPIOC, 6, 7, 8)

// initialization for UART6
UART_INIT(6)

// un-initialization for UART6
UART_UNINIT(6)

///////////
// UART7 //
///////////

// pin configuration for UART7
UART_CONFIG_PINS(7, GPIOF, GPIOF, 7, 6, 8)

// initialization for UART7
UART_INIT(7)

// un-initialization for UART7
UART_UNINIT(7)
