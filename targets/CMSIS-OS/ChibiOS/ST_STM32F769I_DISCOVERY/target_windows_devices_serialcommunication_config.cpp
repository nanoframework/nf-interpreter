//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_serial_native.h"


// pin configuration for UART6
// port: GPIOC
// TX pin: is GPIOC_6
// RX pin: is GPIOC_7
// GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
UART_CONFIG_PINS(6, GPIOC, 6, 7, 8)

uint8_t* Uart6TxBuffer;

// initialization for UART6
// tx buffer siz: 256 bytes
// rx buffer siz: 256 bytes
UART_INIT(6, 256, 256)

// un-initialization for UART6
UART_UNINIT(6)
