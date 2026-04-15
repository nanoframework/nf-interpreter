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

    // Initialise the external flash device.
    // Called once from main() before boot_go().
    // Should return 0 on success, negative on error (non-fatal: boot continues
    // but may fail later if the external secondary slot is needed).
    int mcuboot_ext_flash_init(void);

#ifdef __cplusplus
}
#endif

#endif // MCUBOOT_BOARD_IFACE_H
