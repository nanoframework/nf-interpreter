//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "target_windows_devices_serialcommunication_config.h"
#include <win_dev_serial_native_target.h>

// ///////////
// // UART1 //
// ///////////

// // pin configuration for UART1
// // port for TX pin is: GPIOD
// // port for RX pin is: GPIOC
// // TX pin: is GPIOD_2
// // RX pin: is GPIOC_12
// // GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
// UART_CONFIG_PINS(1, GPIOD, GPIOC, 2, 12, 8)

// // buffers
// // buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// // because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart1_TxBuffer[UART1_TX_SIZE];

// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart1_RxBuffer[UART1_RX_SIZE];

// // initialization for UART1
// UART_INIT(1, UART1_TX_SIZE, UART1_RX_SIZE)

// // un-initialization for UART1
// UART_UNINIT(1)

// ///////////
// // UART2 //
// ///////////

// // pin configuration for UART2
// // port for TX pin is: GPIOD
// // port for RX pin is: GPIOC
// // TX pin: is GPIOD_2
// // RX pin: is GPIOC_12
// // GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
// UART_CONFIG_PINS(2, GPIOD, GPIOC, 2, 12, 8)

// // buffers
// // buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// // because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart2_TxBuffer[UART2_TX_SIZE];

// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart2_RxBuffer[UART2_RX_SIZE];

// // initialization for UART2
// UART_INIT(2, UART2_TX_SIZE, UART2_RX_SIZE)

// // un-initialization for UART2
// UART_UNINIT(2)

// ///////////
// // UART3 //
// ///////////

// // pin configuration for UART3
// // port for TX pin is: GPIOD
// // port for RX pin is: GPIOC
// // TX pin: is GPIOD_2
// // RX pin: is GPIOC_12
// // GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
// UART_CONFIG_PINS(3, GPIOD, GPIOC, 2, 12, 8)

// // buffers
// // buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// // because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart3_TxBuffer[UART3_TX_SIZE];

// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart3_RxBuffer[UART3_RX_SIZE];

// // initialization for UART3
// UART_INIT(3, UART3_TX_SIZE, UART3_RX_SIZE)

// // un-initialization for UART3
// UART_UNINIT(3)

// ///////////
// // UART4 //
// ///////////

// // pin configuration for UART4
// // port for TX pin is: GPIOD
// // port for RX pin is: GPIOC
// // TX pin: is GPIOD_2
// // RX pin: is GPIOC_12
// // GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
// UART_CONFIG_PINS(4, GPIOD, GPIOC, 2, 12, 8)

// // buffers
// // buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// // because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart4_TxBuffer[UART4_TX_SIZE];

// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart4_RxBuffer[UART4_RX_SIZE];

// // initialization for UART4
// UART_INIT(4, UART4_TX_SIZE, UART4_RX_SIZE)

// // un-initialization for UART4
// UART_UNINIT(4)

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

// ///////////
// // UART7 //
// ///////////

// // pin configuration for UART7
// // port for TX pin is: GPIOF
// // port for RX pin is: GPIOF
// // TX pin: is GPIOF_7
// // RX pin: is GPIOF_6
// // GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
// UART_CONFIG_PINS(7, GPIOF, GPIOF, 7, 6, 8)

// // buffers
// // buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// // because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart6_TxBuffer[UART7_TX_SIZE];

// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart6_RxBuffer[UART7_RX_SIZE];

// // initialization for UART6
// UART_INIT(7, UART7_TX_SIZE, UART7_RX_SIZE)

// // un-initialization for UART6
// UART_UNINIT(7)

// ///////////
// // UART8 //
// ///////////

// // pin configuration for UART8
// // port for TX pin is: GPIOC
// // port for RX pin is: GPIOC
// // TX pin: is GPIOC_6
// // RX pin: is GPIOC_7
// // GPIO alternate pin function is 8 (see "Table 13. STM32F765xx, STM32F767xx, STM32F768Ax and STM32F769xx alternate function mapping" in STM32F769I datasheet)
// UART_CONFIG_PINS(8, GPIOC, GPIOC, 6, 7, 8)

// // buffers
// // buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// // because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart8_TxBuffer[UART8_TX_SIZE];

// #if defined(__GNUC__)
// __attribute__((aligned (32)))
// #endif
// uint8_t Uart8_RxBuffer[UART8_RX_SIZE];

// // initialization for UART6
// UART_INIT(8, UART8_TX_SIZE, UART8_RX_SIZE)

// // un-initialization for UART6
// UART_UNINIT(8)
