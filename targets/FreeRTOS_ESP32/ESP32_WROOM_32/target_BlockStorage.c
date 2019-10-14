//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <targetHAL.h>
#include <Target_BlockStorage_ESP32FlashDriver.h>

extern struct BlockStorageDevice    Device_BlockStorage;
extern struct MEMORY_MAPPED_NOR_BLOCK_CONFIG   Device_BlockStorageConfig;
extern IBlockStorageDevice ESP32Flash_BlockStorageInterface;

void BlockStorage_AddDevices()
{
    // add device AND request initialization
    // required to setup flash partitions memory mapping
    BlockStorageList_AddDevice( (BlockStorageDevice*)&Device_BlockStorage, &ESP32Flash_BlockStorageInterface, &Device_BlockStorageConfig, true);
}
