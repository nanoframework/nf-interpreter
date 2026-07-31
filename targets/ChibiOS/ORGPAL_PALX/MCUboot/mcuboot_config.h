//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot configuration for ORGPAL_PALX (STM32F769, W25Q512 QSPI flash).
//
// Primary slots:   internal STM32F769 flash (FLASHv2 peripheral)
// Secondary slots: W25Q512 64 MB QSPI flash
// Upgrade strategy: MCUBOOT_SWAP_USING_OFFSET
// IMAGE_NUMBER: 2 (Image 0 = nanoCLR, Image 1 = deployment)

#ifndef MCUBOOT_CONFIG_ORGPAL_PALX_H
#define MCUBOOT_CONFIG_ORGPAL_PALX_H

// W25Q512 32 kB block erase (0x52) is the MCUboot erase unit for secondary slots.
// This matches CONFIG_NF_MCUBOOT_LOGICAL_SECTOR_SIZE (32 kB).
// The W25Q512 also supports 4 kB sector erase (0x20), used by littlefs.
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE (32U * 1024U)

#endif // MCUBOOT_CONFIG_ORGPAL_PALX_H
