//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <Target_GenericPort_stdio.h>

// pin configuration for stdio using UART4
// port for TX pin is: GPIOA
// port for RX pin is: GPIOA
// TX pin: is GPIOA_0
// RX pin: is GPIOA_1
// GPIO alternate pin function is 8 see alternate function mapping ST datasheet
STDIO_UART_CONFIG_PINS(GPIOA, GPIOA, 0, 1, 8)
