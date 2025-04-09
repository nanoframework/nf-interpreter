//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_ports_config.h"
#include <sys_io_ser_native_target.h>

////////////
// USART2 //
////////////

// GPIOs for USART2 are exposed in the J102 connector.

// pin configuration for USART2
// port for TX pin is: GPIOA
// port for RX pin is: GPIOA
// TX pin: is PF0
// RX pin: is PF1
// GPIO location is 5
UART_INIT_CONFIG(2, gpioPortF, 0, 5, gpioPortF, 1, 5)

// un-initialization for UART2
UART_UNINIT(2, gpioPortF, 0, 5, gpioPortF, 1, 5)
