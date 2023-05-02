//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

// ///////////
// // UART1 //
// ///////////

// NOT AVAILABLE!
// USED FOR NF_WIREPROTOCOL

// ///////////
// // UART2 //
// ///////////

// NOT AVAILABLE!

// ///////////
// // UART3 //
// ///////////

// NOT AVAILABLE!

// ///////////
// // UART4 //
// ///////////

// NOT AVAILABLE!
///////////
// UART5 //
///////////

// Located on CN2, the WiFi/I2C daughter board connector pins 1 and 8
// pin configuration for UART5
// port for TX pin is: GPIOD
// port for RX pin is: GPIOC
// TX pin: is GPIOD_2
// RX pin: is GPIOC_12
// GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate
// function mapping" in STM32F769I datasheet)
UART_CONFIG_PINS(5, GPIOD, GPIOC, 2, 12, 8)

// initialization for UART5
UART_INIT(5)

// un-initialization for UART5
UART_UNINIT(5)

///////////
// UART6 //
///////////

// Located on CN13, the Arduino connector pins D0 and D1
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

// ///////////
// // UART7 //
// ///////////
// NOT AVAILABLE!

// ///////////
// // UART8 //
// ///////////
// NOT AVAILABLE!
