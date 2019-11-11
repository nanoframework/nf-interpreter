//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>

extern struct BlockStorageDevice    Device_BlockStorage;
extern struct MEMORY_MAPPED_NOR_BLOCK_CONFIG   Device_BlockStorageConfig;
extern IBlockStorageDevice STM32Flash_BlockStorageInterface;

void BlockStorage_AddDevices()
{
    BlockStorageList_AddDevice( (BlockStorageDevice*)&Device_BlockStorage, &STM32Flash_BlockStorageInterface, &Device_BlockStorageConfig, false);
}
