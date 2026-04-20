//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// MCUboot flash area abstraction layer.
// This header defines the flash_area_* API contract that each platform port must implement.
// The nanoFramework porting layer implements this API on top of the platform-native flash driver.
//
// Reference: MCUboot boot/bootutil/include/bootutil/bootutil.h and
//            boot/mcu_flash/flash_map_backend/flash_map_backend.h

#ifndef FLASH_MAP_BACKEND_H
#define FLASH_MAP_BACKEND_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    //
    // Flash area descriptor — identifies a named flash region (slot, bootloader, scratch).
    // The platform port populates a static table of these descriptors indexed by fa_id.
    //
    struct flash_area
    {
        uint8_t fa_id;        // Flash area ID (FLASH_AREA_BOOTLOADER, FLASH_AREA_IMAGE_x_*)
        uint8_t fa_device_id; // Device ID: FLASH_DEVICE_INTERNAL_FLASH or FLASH_DEVICE_EXTERNAL_FLASH
        uint16_t pad16;
        uint32_t fa_off;  // Byte offset of this area within its device
        uint32_t fa_size; // Size of this area in bytes
    };

    //
    // Flash sector descriptor — one erasable sector within a flash area.
    // Used by flash_area_get_sectors() to communicate sector geometry to MCUboot's swap engine.
    //
    struct flash_sector
    {
        uint32_t fs_off;  // Byte offset of this sector from the start of its flash area
        uint32_t fs_size; // Size of this sector in bytes
    };

    //
    // v2.3.0 accessor inlines — MCUboot internals use these instead of direct struct member access.
    //

    static inline uint8_t flash_area_get_id(const struct flash_area *fa)
    {
        return fa->fa_id;
    }

    static inline uint8_t flash_area_get_device_id(const struct flash_area *fa)
    {
        return fa->fa_device_id;
    }

    static inline uint32_t flash_area_get_off(const struct flash_area *fa)
    {
        return fa->fa_off;
    }

    static inline uint32_t flash_area_get_size(const struct flash_area *fa)
    {
        return fa->fa_size;
    }

    static inline uint32_t flash_sector_get_off(const struct flash_sector *fs)
    {
        return fs->fs_off;
    }

    static inline uint32_t flash_sector_get_size(const struct flash_sector *fs)
    {
        return fs->fs_size;
    }

    // Retrieve the flash sector within a flash area at a given byte offset.
    // The nanoFramework porting layer populates sector geometry via flash_area_get_sectors();
    // this entry point is required by bootutil_area.c in v2.3.0 for the SWAP_USING_OFFSET path.
    int flash_area_get_sector(const struct flash_area *area, uint32_t off, struct flash_sector *sector);

    //
    // Open a flash area by ID for subsequent read/write/erase operations.
    // Returns 0 on success, negative errno on failure.
    //
    int flash_area_open(uint8_t id, const struct flash_area **area_outp);

    //
    // Close a previously opened flash area.
    // After this call the pointer returned by flash_area_open() must not be used.
    //
    void flash_area_close(const struct flash_area *area);

    //
    // Read len bytes from flash area area at byte offset off into buffer dst.
    // Returns 0 on success, negative errno on failure.
    //
    int flash_area_read(const struct flash_area *area, uint32_t off, void *dst, uint32_t len);

    //
    // Write len bytes from buffer src to flash area area at byte offset off.
    // The destination region must already be erased.
    // Returns 0 on success, negative errno on failure.
    //
    int flash_area_write(const struct flash_area *area, uint32_t off, const void *src, uint32_t len);

    //
    // Erase len bytes of flash area area starting at byte offset off.
    // off and len must be aligned to the device's erase-sector size.
    // Returns 0 on success, negative errno on failure.
    //
    int flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len);

    //
    // Return the minimum write alignment (in bytes) for the given flash area.
    // MCUboot uses this to align trailer and status fields.
    // Typical values: 1 (NOR flash), 4 (STM32 F4/F7), 8 (STM32 H7), 32 (ESP32 with encryption).
    //
    uint32_t flash_area_align(const struct flash_area *area);

    //
    // Return the byte value that erased flash reads as (typically 0xFF for NOR flash).
    //
    uint8_t flash_area_erased_val(const struct flash_area *area);

    //
    // Populate sectors[] with the sector geometry of flash area fa_id.
    // On entry *count is the capacity of sectors[].
    // On return *count is the actual number of sectors written.
    // Returns 0 on success, negative errno on failure.
    //
    // This function is required when MCUBOOT_USE_FLASH_AREA_GET_SECTORS is defined.
    //
    int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors);

    //
    // Translate (image_index, slot) to a flash area ID.
    // image_index: 0 = nanoCLR, 1 = deployment (multi-image).
    // slot: 0 = primary, 1 = secondary.
    // Returns the FLASH_AREA_IMAGE_x_* ID for the given combination.
    //
    int flash_area_id_from_multi_image_slot(int image_index, int slot);

    //
    // Translate slot index to a flash area ID for single-image configurations.
    // Equivalent to flash_area_id_from_multi_image_slot(0, slot).
    //
    int flash_area_id_from_image_slot(int slot);

#ifdef __cplusplus
}
#endif

#endif // FLASH_MAP_BACKEND_H
