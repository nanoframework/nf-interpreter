//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Serial recovery port declarations for the bare-metal
// MCUboot bootloader (STM32 ChibiOS targets).
//
// boot_serial_detect_pin() is board-specific and lives in each board's
// MCUboot/mcuboot_detect_pin.c translation unit.
//
// mcuboot_serial_get_channel() is board-specific and returns the ChibiOS
// I/O channel used for serial recovery transport (BaseChannel *).
// For USB CDC boards (e.g. ORGPAL_PALTHREE) it returns (BaseChannel *)&SDU1.
// For UART boards (e.g. ST_STM32F769I_DISCOVERY) it returns (BaseChannel *)&SD1.
//
// mcuboot_uart_init() is an optional board hook for UART initialisation before
// entering the recovery loop. The default (weak) implementation is a no-op;
// USB CDC boards don't need to override it (transport is started by
// mcuboot_target_init()). UART boards override it in mcuboot_detect_pin.c.
//
// mcuboot_serial_recovery_try() is called from mcuboot_main() after:
//   - halInit() and chSysInit() have set up hardware and the ChibiOS kernel.
//   - mcuboot_ext_flash_init() and mcuboot_sdcard_init() have initialized storage.
//   - But BEFORE boot_go() so recovery is available even if no valid image exists.
//
// It checks the recovery button; if asserted, it calls boot_serial_start() from
// MCUboot (boot/boot_serial/src/boot_serial.c). boot_serial_start() runs the full
// SMP/mcumgr receive loop, allowing the host to upload a new firmware image to the
// secondary slot. When recovery is complete, it resets the device via
// hal_system_reset() (= NVIC_SystemReset()), and the device reboots. On the next
// boot cycle, boot_go() will find the newly-uploaded image ready to boot.
//
// If the recovery button is not asserted, mcuboot_serial_recovery_try() returns
// immediately and boot_go() is called to validate and boot the current image.

#ifndef MCUBOOT_SERIAL_PORT_H
#define MCUBOOT_SERIAL_PORT_H

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(MCUBOOT_SERIAL)

#include "hal.h"
#include "boot_serial/boot_serial.h"

    // Board-specific: return the ChibiOS I/O channel for serial recovery.
    // Must be implemented in each board's mcuboot_detect_pin.c.
    BaseChannel *mcuboot_serial_get_channel(void);

    // Board-specific: return true if the serial recovery GPIO is asserted.
    // Implemented in each board's mcuboot_detect_pin.c.
    bool boot_serial_detect_pin(void);

    // Optional board-specific UART/USB initialisation.
    // The default (weak) implementation is a no-op.
    void mcuboot_uart_init(void);

    // Check the recovery button and — if pressed — run the SMP serial recovery loop.
    // Returns only if the button was not pressed. Resets via NVIC on recovery completion.
    void mcuboot_serial_recovery_try(void);

    // Start the SMP serial recovery loop unconditionally (never returns).
    __attribute__((noreturn)) void mcuboot_serial_recovery_start(void);

#endif // MCUBOOT_SERIAL

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_SERIAL_PORT_H
