//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef TARGET_ESP32FLASH_DRIVER_H
#define TARGET_ESP32FLASH_DRIVER_H

#include <esp32_os.h>
#include <esp_partition.h>

#include <nanoPAL_BlockStorage.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This is ESP_PARTITION sub type specific to .NET nanoFramework.
// See official docs here:
// https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/partition-tables.html#subtype
//
// We are hijacking subtype 0x84.
// Partitions table must use this SUBTYPE code in the table.
#define ESP_PARTITION_SUBTYPE_DATA_NANOCLR (esp_partition_subtype_t)0x84
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool Esp32FlashDriver_InitializeDevice(void *);
bool Esp32FlashDriver_UninitializeDevice(void *);
DeviceBlockInfo *Esp32FlashDriver_GetDeviceInfo(void *);
bool Esp32FlashDriver_Read(void *, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer);
bool Esp32FlashDriver_Write(
    void *,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite);
bool Esp32FlashDriver_IsBlockErased(void *, ByteAddress blockAddress, unsigned int length);
bool Esp32FlashDriver_EraseBlock(void *, ByteAddress address);
void Esp32FlashDriver_SetPowerState(void *, unsigned int state);
bool Esp32FlashDriver_GetMemoryMappedAddress(
    void *,
    unsigned int blockRegionIndex,
    unsigned int blockRangeIndex,
    unsigned int *address);

extern const DRAM_ATTR esp_partition_t *g_pFlashDriver_partition;
extern const void *g_esp32_flash_start_ptr;

#endif //TARGET_ESP32FLASH_DRIVER_H
