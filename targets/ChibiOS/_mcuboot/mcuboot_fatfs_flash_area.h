//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// Platform-level FatFs flash area backend for ChibiOS MCUboot secondary slots.
//
// Declares the read/write/erase functions that implement flash_area_read(),
// flash_area_write() and flash_area_erase() for areas backed by FatFs files on
// the SD card (fa_device_id == FLASH_DEVICE_EXTERNAL_FLASH when an SD card is
// the off-chip secondary slot medium).
//
// Target-specific flash_area_* dispatch code (mcuboot_flash_map_boot.c) calls
// into these functions for external-flash areas when NF_FEATURE_MCUBOOT_HAS_SDCARD
// is enabled.
//
// File naming on the SD card is controlled by the macros MCUBOOT_FATFS_IMG0_PATH
// and MCUBOOT_FATFS_IMG1_PATH, which may be overridden via compile definitions.

#ifndef MCUBOOT_FATFS_FLASH_AREA_H
#define MCUBOOT_FATFS_FLASH_AREA_H

#include <stdint.h>
#include "flash_map_backend/flash_map_backend.h"

// Read len bytes from the FatFs-backed secondary slot starting at byte offset off.
// Returns 0 on success, -1 on any error (file not found, seek failure, short read).
int fatfs_flash_area_read(const struct flash_area *area, uint32_t off, void *dst, uint32_t len);

// Write len bytes to the FatFs-backed secondary slot at byte offset off.
// Creates the file if it does not exist; overwrites existing bytes in place.
// Returns 0 on success, -1 on any error.
int fatfs_flash_area_write(const struct flash_area *area, uint32_t off, const void *src, uint32_t len);

// Erase len bytes in the FatFs-backed secondary slot starting at offset off.
// File-backed slots do not require an erase step before writing; this is a no-op
// that always returns 0.
int fatfs_flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len);

#endif // MCUBOOT_FATFS_FLASH_AREA_H
