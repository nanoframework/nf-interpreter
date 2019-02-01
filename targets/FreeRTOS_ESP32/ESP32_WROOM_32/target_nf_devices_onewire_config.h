//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

// use UART 2 for the 1-wire interface
#define NF_ONEWIRE_ESP32_UART_NUM UART_NUM_2
// use GPIO port 16 for RX and 17 for TX
#define NF_ONEWIRE_ESP32_UART_RX_PIN UART_NUM_2_RXD_DIRECT_GPIO_NUM
#define NF_ONEWIRE_ESP32_UART_TX_PIN UART_NUM_2_TXD_DIRECT_GPIO_NUM
