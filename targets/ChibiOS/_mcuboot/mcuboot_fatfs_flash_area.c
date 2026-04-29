//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Platform-level FatFs flash area backend for ChibiOS MCUboot secondary slots.
//
// Implements read/write/erase of MCUboot secondary slot data stored as flat
// binary files on a FatFs-mounted SD card.  The SD card must have been
// successfully initialised (sdcStart / sdcConnect / f_mount) by the target's
// mcuboot_sdcard_init() before boot_go() calls into these functions.
//
// Each secondary slot is represented by a dedicated file in the root directory
// of the SD card.  The file offset maps directly to the slot offset passed by
// MCUboot: f_lseek(fp, off) positions the read/write cursor for every call.
//
// Erase is intentionally a no-op: f_write() in FA_OPEN_ALWAYS mode overwrites
// bytes in place, so no pre-erase step is required or meaningful for a file.
//
// File path defaults can be overridden at compile time via -D:
//   MCUBOOT_FATFS_IMG0_PATH  (default: "img0_sec.bin")
//   MCUBOOT_FATFS_IMG1_PATH  (default: "img1_sec.bin")

#include "mcuboot_fatfs_flash_area.h"

#include "ff.h"
#include "sysflash/sysflash.h"

// Default file paths — one file per secondary slot, stored at the SD card root.
#if !defined(MCUBOOT_FATFS_IMG0_PATH)
#define MCUBOOT_FATFS_IMG0_PATH "img0_sec.bin"
#endif

#if !defined(MCUBOOT_FATFS_IMG1_PATH)
#define MCUBOOT_FATFS_IMG1_PATH "img1_sec.bin"
#endif

// Map a flash area ID to the corresponding SD card file path.
// Returns NULL for area IDs that are not FatFs-backed secondary slots.
static const char *fatfs_path_for_area(uint8_t fa_id)
{
    switch (fa_id)
    {
        case FLASH_AREA_IMAGE_0_SECONDARY:
            return MCUBOOT_FATFS_IMG0_PATH;

        case FLASH_AREA_IMAGE_1_SECONDARY:
            return MCUBOOT_FATFS_IMG1_PATH;

        default:
            return NULL;
    }
}

int fatfs_flash_area_read(const struct flash_area *area, uint32_t off, void *dst, uint32_t len)
{
    const char *path = fatfs_path_for_area(area->fa_id);

    if (path == NULL)
    {
        return -1;
    }

    FIL fp;
    FRESULT fr = f_open(&fp, path, FA_READ);

    if (fr != FR_OK)
    {
        // File not present on the SD card — no update image available.
        return -1;
    }

    fr = f_lseek(&fp, (FSIZE_t)off);

    if (fr != FR_OK)
    {
        f_close(&fp);
        return -1;
    }

    UINT br;
    fr = f_read(&fp, dst, (UINT)len, &br);
    f_close(&fp);

    return (fr == FR_OK && br == (UINT)len) ? 0 : -1;
}

int fatfs_flash_area_write(const struct flash_area *area, uint32_t off, const void *src, uint32_t len)
{
    const char *path = fatfs_path_for_area(area->fa_id);

    if (path == NULL)
    {
        return -1;
    }

    FIL fp;
    // FA_OPEN_ALWAYS: create the file if it does not exist, open at byte 0 if it does.
    FRESULT fr = f_open(&fp, path, FA_WRITE | FA_OPEN_ALWAYS);

    if (fr != FR_OK)
    {
        return -1;
    }

    fr = f_lseek(&fp, (FSIZE_t)off);

    if (fr != FR_OK)
    {
        f_close(&fp);
        return -1;
    }

    UINT bw;
    fr = f_write(&fp, src, (UINT)len, &bw);

    if (fr == FR_OK)
    {
        f_sync(&fp);
    }

    f_close(&fp);

    return (fr == FR_OK && bw == (UINT)len) ? 0 : -1;
}

int fatfs_flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len)
{
    // File-backed storage does not require an erase step before writing.
    // f_write() overwrites existing file bytes in place, making this a no-op.
    (void)area;
    (void)off;
    (void)len;
    return 0;
}
