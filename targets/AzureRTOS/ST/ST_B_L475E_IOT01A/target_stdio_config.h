//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// Select which STM32 uart/serial port used.
// Always update both STDIO_UART & STDIO_SERIAL_DRIVER to indicate the same UARTn
// Edit mcuconf.h and set #define STM32_SERIAL_USE_UARTn TRUE
#define STDIO_SERIAL_DRIVER SD4
