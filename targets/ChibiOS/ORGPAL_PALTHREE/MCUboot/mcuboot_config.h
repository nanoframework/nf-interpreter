//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot configuration for ORGPAL_PALTHREE (STM32F769ZI, AT25SF641 SPI flash).
//
// Primary slots:   internal STM32F769ZI flash (FLASHv2 peripheral)
// Secondary slots: AT25SF641 8 MB SPI flash on SPI1
// Upgrade strategy: MCUBOOT_SWAP_USING_OFFSET
// IMAGE_NUMBER: 2 (Image 0 = nanoCLR, Image 1 = deployment)

#ifndef MCUBOOT_CONFIG_ORGPAL_PALTHREE_H
#define MCUBOOT_CONFIG_ORGPAL_PALTHREE_H

// STM32F7 FLASHv2: minimum write unit is 4 bytes (FLASH_CR_PSIZE_WORD).
// MCUboot uses this to align swap-state trailer fields.
#define MCUBOOT_FLASH_WRITE_ALIGNMENT 4U

// AT25SF641 64 kB block erase (0xD8) is the MCUboot erase unit for secondary slots.
// The AT25SF641 also supports 4 kB sub-sector erase (0x20), used by littlefs.
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE (64U * 1024U)

// MCUboot image header size in bytes.
// Must match the --header-size argument passed to imgtool sign.
#define MCUBOOT_IMAGE_HEADER_SIZE 0x200U

// Maximum number of sectors across any single image slot.
// Largest slot: Image 1 secondary = 1024 kB / 64 kB = 16 blocks.
#define MCUBOOT_MAX_IMG_SECTORS 16U

#endif // MCUBOOT_CONFIG_ORGPAL_PALTHREE_H
