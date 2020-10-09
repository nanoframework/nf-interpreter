//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_nf_devices_onewire_config.h"
#include <nf_devices_onewire_native_target.h>

///////////
// UART6 //
///////////

// pin configuration for UART6
// port for TX pin is: GPIOC
// TX pin: is GPIOC_6
// GPIO alternate pin function is 8 (see "Table 9. STM32F405xx and STM32F407xx alternate function mapping" in STM32F405xx/STM32F407xx datasheet)
UART_CONFIG_PINS(6, GPIOC, 10, 8)

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
