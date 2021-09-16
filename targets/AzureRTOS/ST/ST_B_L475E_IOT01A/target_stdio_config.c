//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <hal.h>

#include <target_stdio_config.h>

// Select which STM32 uart/serial port used.
// Always update both STDIO_UART & STDIO_SERIAL_DRIVER to indicate the same UARTn
// Edit mcuconf.h and set #define STM32_SERIAL_USE_UARTn TRUE
#define STDIO_UART          4
#define STDIO_SERIAL_DRIVER SD4

// the following macro defines a function that configures the GPIO pins for a STM32 UART/USART
// this is required because the UART/USART peripherals can use multiple GPIO configuration combinations
#define UART_CONFIG_PINS(num, gpio_port_tx, gpio_port_rx, tx_pin, rx_pin, alternate_function)                          \
    void ConfigPins_UART##num()                                                                                        \
    {                                                                                                                  \
        palSetPadMode(gpio_port_tx, tx_pin, PAL_MODE_ALTERNATE(alternate_function));                                   \
        palSetPadMode(gpio_port_rx, rx_pin, PAL_MODE_ALTERNATE(alternate_function));                                   \
    }

// define which serial driver the stdio TRACE will be using
#if (STDIO_UART == 4)
UART_CONFIG_PINS(4, GPIOA, GPIOA, 0, 1, 8)
// Add additional ports to be supported here.
//#elif (STDIO_UART == x)
// UART_CONFIG_PINS(x, GPIOx, GPIOx, x, x, x)
#else
#error "Missing UART_CONFIG_PINS() definition"
#endif

// These macros lead to the definition of the external method used to inialise the GPIO
// for the UART used for stdio. Expands to void ConfigPins_UARTn() where n is UART used for stdio.
// Method name chosen to clash with names used by Windows.Devices.SerialCommunication.SerialDevice.
// Miscconfiguration will cause the linker stage to fail.
#define A_UART_CONFIG_PINS(num)  ConfigPins_UART##num()
#define X_UART_CONFIG_PINS(num)  A_UART_CONFIG_PINS(num)
#define STDIO_UART_CONFIG_PINS() X_UART_CONFIG_PINS(STDIO_UART)

void StdioPort_Init()
{
    STDIO_UART_CONFIG_PINS();
    sdStart(&STDIO_SERIAL_DRIVER, NULL);
}

uint32_t StdioPort_Write(int portNum, const char *data, size_t size)
{
    (void)portNum;
    size_t writeResult = chnWriteTimeout(
        &STDIO_SERIAL_DRIVER,
        (const uint8_t *)data,
        size,
        1000); //(systime_t)TIMEOUT_INFINITE); // 1000); //
    return (uint32_t)writeResult;
}
