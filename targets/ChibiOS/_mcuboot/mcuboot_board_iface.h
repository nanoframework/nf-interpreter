//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// mcuboot_board_iface.h — Board-supplied interface for the common MCUboot bootloader.
//
// Each board target must provide an implementation of these functions in its
// board-specific translation unit (e.g. mcuboot_flash_map_boot.c or a dedicated file).

#ifndef MCUBOOT_BOARD_IFACE_H
#define MCUBOOT_BOARD_IFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

    // Board-specific hardware initialisation called once from main() after chSysInit()
    // and before mcuboot_ext_flash_init(). Boards with nothing extra to do provide
    // an empty implementation.
    void mcuboot_target_init(void);

    // Initialise the external flash device.
    // Called once from main() before boot_go().
    // Should return 0 on success, negative on error (non-fatal: boot continues
    // but may fail later if the external secondary slot is needed).
    int mcuboot_ext_flash_init(void);

    // Initialise the SD card and mount the FatFs filesystem for the secondary slot.
    // Called once from main() before boot_go().
    // Should return 0 on success, negative on error (non-fatal: boot continues
    // but any upgrade using the SD card secondary slot will fail gracefully).
    int mcuboot_sdcard_init(void);

    // Heartbeat blink cadence. Selected by the shared bootloader code; boards do not set it.
    typedef enum
    {
        // Normal boot and image swap. Matches the legacy nanoBooter cadence.
        MCUBOOT_HEARTBEAT_BOOT = 0,

        // SMP serial recovery.
        MCUBOOT_HEARTBEAT_RECOVERY = 1,
    } mcuboot_heartbeat_pattern_t;

    // Start the "bootloader is alive" LED heartbeat thread.
    // Called once from main() after chSysInit() and the storage init.
    // Shared implementation in mcuboot_heartbeat.c; no board code required.
    void mcuboot_heartbeat_start(void);

    // Select the blink cadence. Safe to call from any thread.
    void mcuboot_heartbeat_set_pattern(mcuboot_heartbeat_pattern_t pattern);

    // Toggle the board's user LED once.
    // Board-supplied; a weak no-op covers boards with no suitable LED.
    void mcuboot_heartbeat_led_toggle(void);

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_BOARD_IFACE_H
