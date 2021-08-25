//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "target_system_io_serial_config.h"
#include <sys_io_ser_native_target.h>

///////////
// UART1 //
///////////

// buffers
#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart1_TxBuffer[UART1_TX_SIZE];

#if defined(__GNUC__)
__attribute__((aligned(32)))
#endif
uint8_t Uart1_RxBuffer[UART1_RX_SIZE];
