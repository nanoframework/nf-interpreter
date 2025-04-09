//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

///////////
// UART3 //
///////////

// Note on the ST_B_L475E_IOT01A UART3 is internal and connects to WiFi module.

// pin configuration for UART3
// port for TX pin is: GPIOD
// port for RX pin is: GPIOD
// TX pin: is GPIOD_8
// RX pin: is GPIOD_9
// GPIO alternate pin function is 7 (see "Table 12. STM32F427xx and STM32F429xx alternate function mapping" in
// STM32F427xx and STM32F429xx datasheet)
// UART_CONFIG_PINS(3, GPIOD, GPIOD, 8, 9, 7)

// initialization for UART3
// UART_INIT(3)

// un-initialization for UART3
// UART_UNINIT(3)

///////////
// UART4 //
///////////

// Note on the ST_B_L475E_IOT01A UART4 is on connector CN3. RX on pin 1, TX on pin 2

// pin configuration for UART4
// port for TX pin is: GPIOA
// port for RX pin is: GPIOA
// TX pin: is GPIOA_0
// RX pin: is GPIOA_1
// GPIO alternate pin function is 8
UART_CONFIG_PINS(4, GPIOA, GPIOA, 0, 1, 8)

// initialization for UART3
UART_INIT(4)

// un-initialization for UART3
UART_UNINIT(4)
