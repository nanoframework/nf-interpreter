//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// STM32-specific MCUboot configuration overrides for nanoFramework.
//
// This header is included AFTER mcuboot_config.h to apply STM32-specific
// configuration values that differ from the generic defaults.
//
// Upgrade strategy: MCUBOOT_SWAP_USING_OFFSET
//   Primary slot:   internal STM32 flash
//   Secondary slot: external (Q)SPI flash (PALTHREE/PALX) or SD card FatFs (F769I)
//
// Write alignment: STM32F76xx = 4 bytes (FLASH_CR_PSIZE_WORD)
//                  STM32F429  = 4 bytes (word writes are safest across all sectors)

#ifndef MCUBOOT_STM32_CONFIG_H
#define MCUBOOT_STM32_CONFIG_H

// External flash sector size used for secondary-slot sector enumeration.
// AT25SF641 (PALTHREE SPI1) and W25Q128 (PALX QSPI): 64 kB erase blocks.
// Override per board if smaller sub-sector erase (4 kB) is preferred.
// TODO(ifu-phase1): Confirm with board-specific flash datasheet before use.
#define MCUBOOT_EXTERNAL_FLASH_SECTOR_SIZE (64 * 1024U)

// MCUboot image header size in bytes.
// Standard value for STM32 Cortex-M targets using ARM Embedded GCC.
// Must match the --header-size argument passed to imgtool sign.
#define MCUBOOT_IMAGE_HEADER_SIZE 0x200

#endif // MCUBOOT_STM32_CONFIG_H
