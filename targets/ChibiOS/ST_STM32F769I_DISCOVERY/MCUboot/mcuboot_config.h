//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot configuration for ST_STM32F769I_DISCOVERY (STM32F769NI).
//
// Primary slots:   internal STM32F769NI flash (FLASHv2 peripheral)
// Secondary slots: SD card via FatFs (not yet integrated — stubs return -1)
// Upgrade strategy: MCUBOOT_SWAP_USING_OFFSET
// IMAGE_NUMBER: 2 (Image 0 = nanoCLR, Image 1 = deployment)

#ifndef MCUBOOT_CONFIG_ST_STM32F769I_DISCOVERY_H
#define MCUBOOT_CONFIG_ST_STM32F769I_DISCOVERY_H

// SD card virtual sector size for the FatFs file-backed secondary slots. This is
// only the granularity MCUboot chunks the file at (fatfs_flash_area_read/write/erase
// take plain byte offset + length, no alignment requirement), so it is set to 32 kB
// to match ORGPAL_PALTHREE / ORGPAL_PALX: CONFIG_NF_MCUBOOT_LOGICAL_SECTOR_SIZE
// (256 kB) is a whole multiple of it and every secondary slot offset/size is
// 256 kB-aligned, and it keeps flash_area_get_sectors() well inside
// MCUBOOT_MAX_IMG_SECTORS. FatFs integration is deferred; the stub returns -1.
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE (32U * 1024U)

#endif // MCUBOOT_CONFIG_ST_STM32F769I_DISCOVERY_H
