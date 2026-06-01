//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SERIALCFG_H
#define SERIALCFG_H

// define which serial driver the Wire Protocol will be using
// RP2040 uses SIO driver for UART (ChibiOS SIO = Serial I/O)
// UART0 is on GP0 (TX) and GP1 (RX) on the Pico board
#define SERIAL_DRIVER           SIOD0

#endif // SERIALCFG_H
