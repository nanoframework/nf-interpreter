//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_windows_devices_serialcommunication_config.h"
#include <win_dev_serial_native_target.h>

///////////
// UART3 //
///////////

// pin configuration for UART3
// port for TX pin is: GPIOD
// port for RX pin is: GPIOD
// TX pin: is GPIOD_8
// RX pin: is GPIOD_9
// GPIO alternate pin function is 7 (see "Table 12. STM32F427xx and STM32F429xx alternate function mapping" in STM32F427xx and STM32F429xx datasheet)
UART_CONFIG_PINS(3, GPIOD, GPIOD, 8, 9, 7)

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

// ///////////
// // UART6 //
// ///////////

// // pin configuration for UART6
// // port for TX pin is: GPIOC
// // port for RX pin is: GPIOC
// // TX pin: is GPIOC_6
// // RX pin: is GPIOC_7
// // GPIO alternate pin function is 7 (see "Table 12. STM32F427xx and STM32F429xx alternate function mapping" in STM32F427xx and STM32F429xx datasheet)
// UART_CONFIG_PINS(6, GPIOC, GPIOC, 6, 7, 8)

// // buffers
// // buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// // because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart6_TxBuffer[UART6_TX_SIZE];
// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart6_RxBuffer[UART6_RX_SIZE];

// // initialization for UART6
// UART_INIT(6, UART6_TX_SIZE, UART6_RX_SIZE)

// // un-initialization for UART6
// UART_UNINIT(6)
