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

// AT25SF641 4 kB sub-sector erase (0x20) is used as the MCUboot erase unit for
// secondary slots. The primary slot (IMG0) is 928 kB which is not divisible by
// 32 kB or 64 kB (the block erase sizes), so the 4 kB sub-sector must be used
// to allow MCUboot to enumerate the secondary slot cleanly.
// The AT25SF641 also supports 32 kB block erase (0x52), used by AT25SF641_EraseChip
// (not called by MCUboot).
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE (4U * 1024U)

#endif // MCUBOOT_CONFIG_ORGPAL_PALTHREE_H
