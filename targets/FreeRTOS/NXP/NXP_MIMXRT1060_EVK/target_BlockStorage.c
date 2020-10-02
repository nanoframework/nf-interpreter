//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>

extern struct BlockStorageDevice    Device_BlockStorage;
extern struct MEMORY_MAPPED_NOR_BLOCK_CONFIG   Device_BlockStorageConfig;
extern IBlockStorageDevice iMXRT_BlockStorageInterface;

void BlockStorage_AddDevices()
{
    BlockStorageList_AddDevice( (BlockStorageDevice*)&Device_BlockStorage, &iMXRT_BlockStorageInterface, &Device_BlockStorageConfig, false);
}
