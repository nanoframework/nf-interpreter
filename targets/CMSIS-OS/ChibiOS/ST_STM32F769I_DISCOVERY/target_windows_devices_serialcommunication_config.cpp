//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "target_windows_devices_serialcommunication_config.h"
#include <win_dev_serial_native.h>

///////////
// UART5 //
///////////

// pin configuration for UART5
// port for TX pin is: GPIOD
// port for RX pin is: GPIOC
// TX pin: is GPIOD_2
// RX pin: is GPIOC_12
// GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
UART_CONFIG_PINS(5, GPIOD, GPIOC, 2, 12, 8)

// buffers
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart5_TxBuffer[UART5_TX_SIZE];

#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart5_RxBuffer[UART5_RX_SIZE];

// initialization for UART5
UART_INIT(5, UART5_TX_SIZE, UART5_RX_SIZE)

// un-initialization for UART5
UART_UNINIT(5)

///////////
// UART6 //
///////////

// pin configuration for UART6
// port for TX pin is: GPIOC
// port for RX pin is: GPIOC
// TX pin: is GPIOC_6
// RX pin: is GPIOC_7
// GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
UART_CONFIG_PINS(6, GPIOC, GPIOC, 6, 7, 8)

// buffers
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart6_TxBuffer[UART6_TX_SIZE];

#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart6_RxBuffer[UART6_RX_SIZE];

// initialization for UART6
UART_INIT(6, UART6_TX_SIZE, UART6_RX_SIZE)

// un-initialization for UART6
UART_UNINIT(6)
