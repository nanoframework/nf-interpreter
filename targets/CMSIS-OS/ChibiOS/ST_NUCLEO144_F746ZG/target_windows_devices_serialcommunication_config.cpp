//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "target_windows_devices_serialcommunication_config.h"
#include "win_dev_serial_native.h"

///////////
// UART2 //
///////////

// pin configuration for UART2
// port for TX pin is: GPIOD
// port for RX pin is: GPIOD
// TX pin: is GPIOD_5
// RX pin: is GPIOD_6
// GPIO alternate pin function is 7 (see "Table 12. STM32F427xx and STM32F429xx alternate function mapping" in STM32F427xx and STM32F429xx datasheet)
UART_CONFIG_PINS(2, GPIOD, GPIOD, 5, 6, 7)

// buffers
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart2_TxBuffer[UART2_TX_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart2_RxBuffer[UART2_RX_SIZE];

// initialization for UART2
UART_INIT(2, UART2_TX_SIZE, UART2_RX_SIZE)

// un-initialization for UART2
UART_UNINIT(2)


///////////
// UART6 //
///////////

// pin configuration for UART6
// port for TX pin is: GPIOC
// port for RX pin is: GPIOC
// TX pin: is GPIOC_6
// RX pin: is GPIOC_7
// GPIO alternate pin function is 8 (see "Table 12. STM32F427xx and STM32F429xx alternate function mapping" in STM32F427xx and STM32F429xx datasheet)
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
