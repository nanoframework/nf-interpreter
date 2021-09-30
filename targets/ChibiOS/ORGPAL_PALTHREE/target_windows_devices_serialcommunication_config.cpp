//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_windows_devices_serialcommunication_config.h"
#include <win_dev_serial_native_target.h>

///////////
// UART2 //
///////////

UART_CONFIG_PINS__(2, GPIOD, GPIOD, 5, 6, 7)

// buffers
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart2_TxBuffer__[UART2_TX_SIZE];
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart2_RxBuffer__[UART2_RX_SIZE];

// initialization for UART2
UART_INIT__(2, UART2_TX_SIZE, UART2_RX_SIZE)

// un-initialization for UART2
UART_UNINIT__(2)

///////////
// UART3 //
///////////

// pin configuration for UART3
UART_CONFIG_PINS__(3, GPIOB, GPIOB, 10, 11, 7)

// buffers
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart3_TxBuffer__[UART3_TX_SIZE];
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart3_RxBuffer__[UART3_RX_SIZE];

// initialization for UART3
UART_INIT__(3, UART3_TX_SIZE, UART3_RX_SIZE)

// un-initialization for UART3
UART_UNINIT__(3)

///////////
// UART6 //
///////////

// pin configuration for UART6
// port for TX pin is: GPIOC
// port for RX pin is: GPIOC
// TX pin: is GPIOC_6
// RX pin: is GPIOC_7
// GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate
// function mapping" in STM32F769I datasheet)
UART_CONFIG_PINS__(6, GPIOC, GPIOC, 6, 7, 8)

// buffers
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart6_TxBuffer__[UART6_TX_SIZE];

#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart6_RxBuffer__[UART6_RX_SIZE];

// initialization for UART6
UART_INIT__(6, UART6_TX_SIZE, UART6_RX_SIZE)

// un-initialization for UART6
UART_UNINIT__(6)

///////////
// UART7 //
///////////

// pin configuration for UART7
UART_CONFIG_PINS__(7, GPIOF, GPIOF, 7, 6, 8)

// buffers
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart7_TxBuffer__[UART7_TX_SIZE];

#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart7_RxBuffer__[UART7_RX_SIZE];

// initialization for UART7
UART_INIT__(7, UART7_TX_SIZE, UART7_RX_SIZE)

// un-initialization for UART7
UART_UNINIT__(7)
