//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_serial_native.h"

///////////
// UART3 //
///////////

// pin configuration for UART3
// port: GPIOD
// TX pin: is GPIOD_8
// RX pin: is GPIOD_9
// GPIO alternate pin function is 7 (see "Table 12. STM32F427xx and STM32F429xx alternate function mapping" in STM32F427xx and STM32F429xx datasheet)
UART_CONFIG_PINS(3, GPIOD, 8, 9, 7)

// buffers size
// tx buffer size: 256 bytes
#define UART3_TX_SIZE  256
// rx buffer size: 256 bytes
#define UART3_RX_SIZE  256

// buffers
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart3_TxBuffer[UART3_TX_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart3_RxBuffer[UART3_RX_SIZE];

// initialization for UART3
UART_INIT(3, UART3_TX_SIZE, UART3_RX_SIZE)

// un-initialization for UART3
UART_UNINIT(3)

