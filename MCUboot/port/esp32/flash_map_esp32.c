//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// ESP32-specific MCUboot flash area implementation (porting layer stub).
//
// This file implements the flash_area_* API from flash_map_backend.h on top of
// the ESP-IDF partition API for all nanoFramework ESP32 variants:
//   esp32, esp32s2, esp32s3, esp32c3, esp32c5, esp32c6, esp32h2, esp32p4
//
// Both slots (primary and secondary) reside in internal SPI flash.
// Upgrade strategy: MCUBOOT_SWAP_USING_MOVE (set in mcuboot_esp32_config.h).
//
// The partition labels used here correspond to the nanoFramework MCUboot partition
// CSV files at targets/ESP32/_IDF/<variant>/partitions_nanoclr_*.csv:
//   "clr_0"     → FLASH_AREA_IMAGE_0_PRIMARY   (nanoCLR primary)
//   "clr_1"     → FLASH_AREA_IMAGE_0_SECONDARY (nanoCLR secondary)
//   "deploy_0"  → FLASH_AREA_IMAGE_1_PRIMARY   (deployment primary)
//   "deploy_1"  → FLASH_AREA_IMAGE_1_SECONDARY (deployment secondary)
//   "mcuboot"   → FLASH_AREA_BOOTLOADER        (not in CSV; located at 0x1000/0x0)
//
// Write alignment:
//   Standard: 4 bytes.
//   With flash encryption enabled: 32 bytes.
//   This stub returns 4 bytes; flash encryption support requires a runtime check.
//
// All functions are stubs that return success.  Each contains a TODO comment
// identifying the exact ESP-IDF API call to substitute in Phase 2.
//
// Phase 2 implementation tasks are tagged: // TODO(ifu-phase2): <description>

#include <stdint.h>
#include <stddef.h>

// ESP-IDF partition API
// TODO(ifu-phase2): Verify include path is correct for the ESP-IDF version in use (v5.5.4).
#include "esp_partition.h"

#include "flash_map_backend/flash_map_backend.h"
#include "sysflash/sysflash.h"

//
// Partition label strings — must match exactly the partition names in the CSV files.
// These are used to locate the esp_partition_t via esp_partition_find_first().
//
#define NF_ESP32_PARTITION_CLR_0    "clr_0"
#define NF_ESP32_PARTITION_CLR_1    "clr_1"
#define NF_ESP32_PARTITION_DEPLOY_0 "deploy_0"
#define NF_ESP32_PARTITION_DEPLOY_1 "deploy_1"

//
// Extended flash area descriptor that carries the ESP-IDF partition pointer.
// This allows flash_area_read/write/erase to call the esp_partition_* API directly.
//
typedef struct
{
    struct flash_area base;               // Must be first — returned as const struct flash_area *
    const esp_partition_t *esp_partition; // Resolved by flash_area_open(); NULL until opened
} nf_esp32_flash_area_t;

//
// Static flash area table for ESP32 targets.
// Offsets and sizes are populated at open time from the esp_partition_t resolved by label.
// TODO(ifu-phase2): Populate fa_off and fa_size at boot from partition table (esp_partition_find_first).
//
static nf_esp32_flash_area_t s_esp32_flash_areas[] = {
    // clang-format off
    { .base = { .fa_id = FLASH_AREA_BOOTLOADER,        .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = 0, .fa_size = 0 }, .esp_partition = NULL },
    { .base = { .fa_id = FLASH_AREA_IMAGE_0_PRIMARY,   .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = 0, .fa_size = 0 }, .esp_partition = NULL },
    { .base = { .fa_id = FLASH_AREA_IMAGE_0_SECONDARY, .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = 0, .fa_size = 0 }, .esp_partition = NULL },
    { .base = { .fa_id = FLASH_AREA_IMAGE_1_PRIMARY,   .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = 0, .fa_size = 0 }, .esp_partition = NULL },
    { .base = { .fa_id = FLASH_AREA_IMAGE_1_SECONDARY, .fa_device_id = FLASH_DEVICE_INTERNAL_FLASH, .fa_off = 0, .fa_size = 0 }, .esp_partition = NULL },
    // clang-format on
};

#define FLASH_AREA_TABLE_COUNT (sizeof(s_esp32_flash_areas) / sizeof(s_esp32_flash_areas[0]))

//
// Map flash area ID to the partition label string used in the partition CSV.
//
static const char *s_partition_label_for_id(uint8_t id)
{
    switch (id)
    {
        case FLASH_AREA_IMAGE_0_PRIMARY:
            return NF_ESP32_PARTITION_CLR_0;
        case FLASH_AREA_IMAGE_0_SECONDARY:
            return NF_ESP32_PARTITION_CLR_1;
        case FLASH_AREA_IMAGE_1_PRIMARY:
            return NF_ESP32_PARTITION_DEPLOY_0;
        case FLASH_AREA_IMAGE_1_SECONDARY:
            return NF_ESP32_PARTITION_DEPLOY_1;
        default:
            return NULL;
    }
}

int flash_area_open(uint8_t id, const struct flash_area **area_outp)
{
    for (uint32_t i = 0; i < FLASH_AREA_TABLE_COUNT; i++)
    {
        if (s_esp32_flash_areas[i].base.fa_id == id)
        {
            // TODO(ifu-phase2): Resolve the esp_partition_t and populate fa_off / fa_size.
            //   const char *label = s_partition_label_for_id(id);
            //   if (label != NULL) {
            //       s_esp32_flash_areas[i].esp_partition =
            //           esp_partition_find_first(ESP_PARTITION_TYPE_APP,
            //                                   ESP_PARTITION_SUBTYPE_ANY, label);
            //       if (s_esp32_flash_areas[i].esp_partition != NULL) {
            //           s_esp32_flash_areas[i].base.fa_off  = s_esp32_flash_areas[i].esp_partition->address;
            //           s_esp32_flash_areas[i].base.fa_size = s_esp32_flash_areas[i].esp_partition->size;
            //       }
            //   }
            *area_outp = &s_esp32_flash_areas[i].base;
            return 0;
        }
    }
    return -1;
}

void flash_area_close(const struct flash_area *area)
{
    (void)area;
    // No resource to release — esp_partition_t is a const pointer into ROM partition table.
}

int flash_area_read(const struct flash_area *area, uint32_t off, void *dst, uint32_t len)
{
    (void)area;
    (void)off;
    (void)dst;
    (void)len;

    // TODO(ifu-phase2): Call esp_partition_read(esp_area->esp_partition, off, dst, len).
    //   esp_partition_read handles internal cache coherency and SPI flash locking.
    //   Return (ret == ESP_OK) ? 0 : -1;

    return 0; // Stub: success
}

int flash_area_write(const struct flash_area *area, uint32_t off, const void *src, uint32_t len)
{
    (void)area;
    (void)off;
    (void)src;
    (void)len;

    // TODO(ifu-phase2): Call esp_partition_write(esp_area->esp_partition, off, src, len).
    //   Note: if flash encryption is enabled, alignment must be 32 bytes.
    //   Return (ret == ESP_OK) ? 0 : -1;

    return 0; // Stub: success
}

int flash_area_erase(const struct flash_area *area, uint32_t off, uint32_t len)
{
    (void)area;
    (void)off;
    (void)len;

    // TODO(ifu-phase2): Call esp_partition_erase_range(esp_area->esp_partition, off, len).
    //   ESP32 internal flash erase granularity is 4 kB (one "sector" in ESP-IDF terms).
    //   off and len must be 4 kB aligned.
    //   Return (ret == ESP_OK) ? 0 : -1;

    return 0; // Stub: success
}

uint32_t flash_area_align(const struct flash_area *area)
{
    (void)area;
    // Standard ESP32 SPI flash write alignment is 4 bytes.
    // TODO(ifu-phase2): Return 32 if flash encryption is enabled (check esp_flash_encryption_enabled()).
    return 4;
}

uint8_t flash_area_erased_val(const struct flash_area *area)
{
    (void)area;
    // ESP32 SPI flash (NOR) reads as 0xFF after erase.
    return 0xFF;
}

int flash_area_get_sectors(int fa_id, uint32_t *count, struct flash_sector *sectors)
{
    (void)fa_id;
    (void)count;
    (void)sectors;

    // TODO(ifu-phase2): Populate sectors[] with uniform 4 kB sectors for the ESP32 internal flash.
    //   ESP32 SPI flash uses uniform 4096-byte erase sectors regardless of variant.
    //
    //   uint32_t area_size   = /* fa_size for fa_id, from resolved esp_partition->size */;
    //   uint32_t sector_size = 4096U;
    //   uint32_t n           = area_size / sector_size;
    //   for (uint32_t i = 0; i < n && i < *count; i++) {
    //       sectors[i].fs_off  = i * sector_size;
    //       sectors[i].fs_size = sector_size;
    //   }
    //   *count = n;
    //   return 0;

    *count = 0;
    return 0; // Stub: success
}

int flash_area_id_from_multi_image_slot(int image_index, int slot)
{
    if (image_index == 0)
    {
        return (slot == 0) ? FLASH_AREA_IMAGE_0_PRIMARY : FLASH_AREA_IMAGE_0_SECONDARY;
    }
    else if (image_index == 1)
    {
        return (slot == 0) ? FLASH_AREA_IMAGE_1_PRIMARY : FLASH_AREA_IMAGE_1_SECONDARY;
    }
    return -1;
}

int flash_area_id_from_image_slot(int slot)
{
    return flash_area_id_from_multi_image_slot(0, slot);
}
