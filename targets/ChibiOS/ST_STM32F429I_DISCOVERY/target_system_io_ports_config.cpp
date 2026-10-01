//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

// UART1 (PA9/PA10) is used by Wire Protocol through the ST-LINK Virtual COM port

///////////
// UART3 //
///////////

// pin configuration for UART3
// port for TX pin is: GPIOD
// port for RX pin is: GPIOD
// TX pin: is GPIOD_8
// RX pin: is GPIOD_9
// GPIO alternate pin function is 7 (see "Table 12. STM32F427xx and STM32F429xx alternate function mapping" in
// STM32F427xx and STM32F429xx datasheet)
UART_CONFIG_PINS(3, GPIOD, GPIOD, 8, 9, 7)

// initialization for UART3
UART_INIT(3)

// un-initialization for UART3
UART_UNINIT(3)
