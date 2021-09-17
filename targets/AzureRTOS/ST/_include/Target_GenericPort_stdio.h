//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_GENERIC_PORT_STDIO_H
#define TARGET_GENERIC_PORT_STDIO_H

#include <hal.h>
#include <nanoPAL_COM.h>
#include <target_stdio_config.h>

// the following macro defines a function that configures the GPIO pins for a STM32 UART/USART
// this is required because the UART/USART peripherals can use multiple GPIO configuration combinations
#define STDIO_UART_CONFIG_PINS(gpio_port_tx, gpio_port_rx, tx_pin, rx_pin, alternate_function)                         \
    void StdioConfigPins_UART()                                                                                        \
    {                                                                                                                  \
        palSetPadMode(gpio_port_tx, tx_pin, PAL_MODE_ALTERNATE(alternate_function));                                   \
        palSetPadMode(gpio_port_rx, rx_pin, PAL_MODE_ALTERNATE(alternate_function));                                   \
    }

void StdioConfigPins_UART(void);

#endif // TARGET_GENERIC_PORT_STDIO_H
