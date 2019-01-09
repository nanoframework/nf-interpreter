//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "target_nf_devices_onewire_config.h"
#include <nf_devices_onewire_native.h>

///////////
// UART4 //
///////////

// pin configuration for UART4
// port for TX pin is: GPIOC
// TX pin: is GPIOC_10
// GPIO alternate pin function is 8
UART_CONFIG_PINS(4, GPIOC, 10, 8)

// buffers
// buffers that are R/W by DMA are recommended to be aligned with 32 bytes cache page size boundary
// because of issues with cache coherency and DMA (this is particularly important with Cortex-M7 because of cache)
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart4_TxBuffer[UART4_TX_SIZE];
#if defined(__GNUC__)
__attribute__((aligned (32)))
#endif
uint8_t Uart4_RxBuffer[UART4_RX_SIZE];
