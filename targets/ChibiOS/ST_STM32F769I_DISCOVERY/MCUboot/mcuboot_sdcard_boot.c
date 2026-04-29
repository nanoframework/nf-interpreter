//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot SD card initialisation for ST_STM32F769I_DISCOVERY (STM32F769NI).
//
// Provides mcuboot_sdcard_init(), the board-level SD card initialisation hook
// declared in mcuboot_board_iface.h and called from mcuboot_main.c.
// Compiled only when NF_FEATURE_MCUBOOT_HAS_SDCARD is enabled.

#if defined(NF_FEATURE_MCUBOOT_HAS_SDCARD)

#include "hal.h"
#include "ff.h"

// FatFs work area for the SD card — allocated once for the bootloader lifetime.
static FATFS s_fatfs;

// Initialise the SDC peripheral and mount the FatFs filesystem.
// Called once from main() before boot_go().
// Returns 0 on success, -1 on any failure (non-fatal: boot_go() will treat
// the secondary slot as unavailable and boot the primary slot directly).
int mcuboot_sdcard_init(void)
{
    // Start the ChibiOS SDC driver (SDMMC1 peripheral clocked by halInit()).
    sdcStart(&SDCD1, NULL);

    // Connect to (identify) the inserted SD card: OCR, CID, CSD negotiation.
    if (sdcConnect(&SDCD1) != HAL_SUCCESS)
    {
        return -1;
    }

    // Mount the FatFs filesystem on the SD card (immediate mount: bForce=1).
    if (f_mount(&s_fatfs, "", 1) != FR_OK)
    {
        return -1;
    }

    return 0;
}

#endif // NF_FEATURE_MCUBOOT_HAS_SDCARD
