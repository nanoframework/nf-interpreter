//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// ESP32-specific MCUboot configuration overrides for nanoFramework.
//
// This header is included AFTER mcuboot_config.h to apply ESP32-specific
// configuration values that differ from the generic defaults.
//
// Upgrade strategy: MCUBOOT_SWAP_USING_MOVE
//   Both slots reside in internal SPI flash.
//   No dedicated scratch area is required — MCUboot moves sectors within the
//   flash address space during a swap.
//
// Write alignment: 4 bytes standard; 32 bytes when flash encryption is enabled.

#ifndef MCUBOOT_ESP32_CONFIG_H
#define MCUBOOT_ESP32_CONFIG_H

// Internal SPI flash sector size for all ESP32 variants.
// ESP-IDF erases in units of 4 kB regardless of the physical flash chip.
// MCUboot uses this for sector enumeration in flash_area_get_sectors().
#define MCUBOOT_ESP32_FLASH_SECTOR_SIZE (4 * 1024U)

// MCUboot image header size in bytes for ESP32 Espressif port.
// Must match the --header-size argument passed to imgtool sign.
// 0x20 is the standard value for the Espressif MCUboot port.
// TODO(ifu-phase2): Confirm with MCUboot Espressif port boot/espressif/include/esp_mcuboot_image.h.
#define MCUBOOT_IMAGE_HEADER_SIZE 0x20

#endif // MCUBOOT_ESP32_CONFIG_H
