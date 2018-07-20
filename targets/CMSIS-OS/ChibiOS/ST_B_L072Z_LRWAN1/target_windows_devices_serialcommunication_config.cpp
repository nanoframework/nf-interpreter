//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "win_dev_serial_native.h"

///////////
// UART1 //
///////////

// pin configuration for UART1
// port for TX pin is: GPIOA
// port for RX pin is: GPIOA
// TX pin: is GPIOA_9
// RX pin: is GPIOA_10
// GPIO alternate pin function is 7 (see "Table 17. Alternate functions port A" in STM32L073xZ datasheet)
UART_CONFIG_PINS(1, GPIOA, GPIOA, 9, 10, 4)

// buffers size
// tx buffer size: 256 bytes
#define UART1_TX_SIZE  256
// rx buffer size: 256 bytes
#define UART1_RX_SIZE  256

// buffers
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart1_TxBuffer[UART1_TX_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart1_RxBuffer[UART1_RX_SIZE];

// initialization for UART1
UART_INIT(1, UART1_TX_SIZE, UART1_RX_SIZE)

// un-initialization for UART1
UART_UNINIT(1)
