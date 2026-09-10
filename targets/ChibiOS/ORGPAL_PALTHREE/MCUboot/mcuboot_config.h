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

// AT25SF641 32 kB block erase (0x52) is the MCUboot erase unit for the secondary
// slots. All secondary slot offsets and sizes are 256 kB-aligned and
// CONFIG_NF_MCUBOOT_LOGICAL_SECTOR_SIZE (256 kB) is a whole multiple of 32 kB
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE (32U * 1024U)

#endif // MCUBOOT_CONFIG_ORGPAL_PALTHREE_H
