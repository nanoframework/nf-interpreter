//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_windows_devices_serialcommunication_config.h"
#include <win_dev_serial_native_target.h>

////////////
// USART8 //
////////////

// pin configuration for USART8
// port for TX pin is: GPIOC
// port for RX pin is: GPIOC
// TX pin: is GPIOC_2
// RX pin: is GPIOC_3
// GPIO alternate pin function is 1 (see "Table 16. Alternate functions selected through GPIOC_AFR registers for port C" in STM32F091RC datasheet)
UART_CONFIG_PINS(8, GPIOC, GPIOC, 2, 3, 2)

// buffers
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart8_TxBuffer[UART8_TX_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart8_RxBuffer[UART8_RX_SIZE];

// initialization for UART8
UART_INIT(8, UART8_TX_SIZE, UART8_RX_SIZE)

// un-initialization for UART8
UART_UNINIT(8)
