//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_serial_port.h — Serial recovery port declarations for the bare-metal
// MCUboot bootloader (STM32 ChibiOS targets).
//
// boot_serial_start() is a placeholder — the full SMP serial recovery stack
// (boot_serial.c, zcbor, base64, crc16) is not yet included in the build.
// When the button is detected, the bootloader enters an infinite loop and
// requires a hardware reset to recover.
//
// boot_serial_detect_pin() is board-specific and lives in each board's
// MCUboot/mcuboot_detect_pin.c translation unit.

#ifndef MCUBOOT_SERIAL_PORT_H
#define MCUBOOT_SERIAL_PORT_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(MCUBOOT_SERIAL)
#include "boot_serial/boot_serial.h"

    // Placeholder implementation of boot_serial_start().
    // Satisfies the linker when MCUBOOT_SERIAL=1 without pulling in the full
    // SMP serial stack. Loops forever; hardware reset is required to exit.
    void boot_serial_start(const struct boot_uart_funcs *f);

    // Optional board-specific UART initialisation, called by boot_serial_start()
    // before entering the recovery loop. The default (weak) implementation is a
    // no-op; boards that require a hardware UART (e.g. ST_STM32F769I_DISCOVERY)
    // override this in their mcuboot_detect_pin.c.
    void mcuboot_uart_init(void);

#endif // MCUBOOT_SERIAL

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_SERIAL_PORT_H
