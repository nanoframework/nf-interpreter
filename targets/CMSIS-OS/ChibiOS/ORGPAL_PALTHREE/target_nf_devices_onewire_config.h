//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// the following macro defines a function that configures the GPIO pins for a STM32 UART/USART
// it gets called in the oneWire_lld_start function
// this is required because the UART/USART peripherals can use multiple GPIO configuration combinations
#define UART_CONFIG_PINS(num, gpio_port_tx, tx_pin, alternate_function) void ConfigPins_UART##num() { \
    palSetPadMode(gpio_port_tx, tx_pin, PAL_MODE_ALTERNATE(alternate_function) | PAL_STM32_OTYPE_OPENDRAIN \
    | PAL_MODE_INPUT_PULLUP | PAL_STM32_OSPEED_HIGHEST | PAL_STM32_MODE_ALTERNATE); \
}

///////////
// UART4 //
///////////

// enable USART4
#define NF_ONEWIRE_STM32_UART_USE_USART4    TRUE

// buffers size
// tx buffer size: 32 bytes
#define UART4_TX_SIZE  32
// rx buffer size: 32 bytes
#define UART4_RX_SIZE  32