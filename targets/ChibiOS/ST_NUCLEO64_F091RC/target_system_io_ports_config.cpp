//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

////////////
// USART8 //
////////////

// pin configuration for USART8
// port for TX pin is: GPIOC
// port for RX pin is: GPIOC
// TX pin: is GPIOC_8
// RX pin: is GPIOC_9
// GPIO alternate pin function is 1 (see "Table 16. Alternate functions selected through GPIOC_AFR registers for port C"
// in STM32F091RC datasheet)
UART_CONFIG_PINS(8, GPIOC, GPIOC, 8, 9, 1)

// initialization for UART8
UART_INIT(8)

// un-initialization for UART8
UART_UNINIT(8)
