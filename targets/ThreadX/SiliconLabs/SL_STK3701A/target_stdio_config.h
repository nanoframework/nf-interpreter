//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// Select which STM32 uart/serial port used.
// If you change STDIO_SERIAL_DRIVER here to another UARTn, also
// edit mcuconf.h to set #define STM32_SERIAL_USE_UARTn TRUE
// and edit GPIO pins defined in target_stdio_config.c
// #define STDIO_SERIAL_DRIVER SD4
