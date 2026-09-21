//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot update media for ST_STM32F769I_DISCOVERY (STM32F769NI).
//
// Lists the storage volumes the bootloader sweeps for update files
// (nano-clr-update-*.bin, nano-deployment-update-*.bin) before boot_go() — see
// MCUboot/common/mcuboot_media_import.c: the SD card on SDMMC1 (SDCD1), FatFs volume
// "D:", the same volume the application uses as regular storage. Compiled only when
// NF_FEATURE_MCUBOOT_HAS_SDCARD is enabled.
//
// Note: this board has no external NVM for the secondary slots yet, so an imported image
// cannot be staged until that is defined (see mcuboot_flash_map_boot.c).

#if defined(CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD) && CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD

#include <hal.h>

#include <mcuboot_media_fatfs.h>
#include <mcuboot_board_iface.h>

static int sdcard_init(void)
{
    // Start the ChibiOS SDC driver (SDMMC1 peripheral clocked by halInit()).
    sdcStart(&SDCD1, NULL);

    // Connect to (identify) the inserted SD card: OCR, CID, CSD negotiation.
    if (sdcConnect(&SDCD1) != HAL_SUCCESS)
    {
        sdcStop(&SDCD1);
        return -1;
    }

    return 0;
}

static void sdcard_deinit(void)
{
    if (blkGetDriverState(&SDCD1) == BLK_READY)
    {
        (void)sdcDisconnect(&SDCD1);
    }

    sdcStop(&SDCD1);
}

static mcuboot_media_fatfs_ctx s_sdCardCtx = {
    .volume = "D:",
    .device_init = sdcard_init,
    .device_deinit = sdcard_deinit,
};

static const mcuboot_media s_mediaTable[] = {
    {.name = "SD card", .ops = &mcuboot_media_fatfs_ops, .ctx = &s_sdCardCtx},
};

const mcuboot_media *mcuboot_media_table(uint32_t *count)
{
    *count = sizeof(s_mediaTable) / sizeof(s_mediaTable[0]);
    return s_mediaTable;
}

#endif // CONFIG_NF_FEATURE_MCUBOOT_HAS_SDCARD
