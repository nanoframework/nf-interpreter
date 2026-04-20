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

// STM32F7 FLASHv2: minimum write unit is 4 bytes (FLASH_CR_PSIZE_WORD).
// MCUboot uses this to align swap-state trailer fields.
#define MCUBOOT_FLASH_WRITE_ALIGNMENT 4U

// SD card virtual sector size = 4 kB (FAT cluster granularity used for FatFs
// file-backed secondary slots). FatFs integration is deferred; stub returns -1.
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE (4U * 1024U)

// MCUboot image header size in bytes.
// Must match the --header-size argument passed to imgtool sign.
#define MCUBOOT_IMAGE_HEADER_SIZE 0x200U

// Maximum number of sectors across any single image slot.
// Largest slot: Image 1 secondary = 1024 kB / 4 kB virtual sectors = 256.
#define MCUBOOT_MAX_IMG_SECTORS 256U

#endif // MCUBOOT_CONFIG_ST_STM32F769I_DISCOVERY_H
