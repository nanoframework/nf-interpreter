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

// SD card virtual sector size = 4 kB (FAT cluster granularity used for FatFs
// file-backed secondary slots). FatFs integration is deferred; stub returns -1.
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE (4U * 1024U)

#endif // MCUBOOT_CONFIG_ST_STM32F769I_DISCOVERY_H
