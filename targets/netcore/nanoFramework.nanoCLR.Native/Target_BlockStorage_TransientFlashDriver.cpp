//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include <Target_BlockStorage_TransientFlashDriver.h>

CLR_RT_Buffer *storage = NULL;


bool TransientFlashDriver_InitializeDevice(void* context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;

    // ChibiOS driver takes care of this, so always true

    storage = new std::vector<byte>(config->Memory.SizeInBytes);
    return true;
}

bool TransientFlashDriver_UninitializeDevice(void* context)
{
    delete storage;

    // ChibiOS driver takes care of this, so always true
    return true;
}

DeviceBlockInfo *TransientFlashDriver_GetDeviceInfo(void *context)
{
    
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;
    
    return config->BlockConfig.BlockDeviceInformation;  
}

bool TransientFlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    if (!storage)
        TransientFlashDriver_InitializeDevice(context);

    auto *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;
    auto offset = startAddress - config->Memory.BaseAddress;
    std::memcpy(buffer, &(*storage)[offset], numBytes);
    return true;
}

bool TransientFlashDriver_Write(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite)
{
    if (!storage)
        TransientFlashDriver_InitializeDevice(context);

    auto *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;
    auto offset = startAddress - config->Memory.BaseAddress;
    std::memcpy(&(*storage)[offset], buffer, numBytes);
    return true;
}

bool TransientFlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;

    return true;
}

bool TransientFlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;

    return true;
}
