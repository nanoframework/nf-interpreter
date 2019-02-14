//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <targetHAL.h>
#include <Target_BlockStorage_CC32xxFlashDriver.h>

///////////////////////////////////////////////////
// BlockStream 
///////////////////////////////////////////////////
bool BlockStorageStream_Initialize(BlockStorageStream* stream, unsigned int blockUsage)
{
    // 'reset' CurrentIndex
    stream->CurrentIndex = 0;

    stream->BlockLength = 0;

    // set usage according to parameter
    stream->Usage = blockUsage;

    // initialize base address and length from linker symbols
    if(blockUsage == StorageUsage_CLR)
    {
        // set BaseAddress to the start of the region
        stream->BaseAddress = (unsigned int)&__nanoImage_start__;
        // set Length to the region size 
        // need to cast the pointers to make sure the compiler implements the correct math
        stream->Length = ((uint32_t)&__nanoImage_end__) - ((uint32_t)&__nanoImage_start__);
    }
    else if(blockUsage == StorageUsage_DEPLOYMENT)
    {
        // deployment is stored at SPI flash
        // if there is a DEPLOYMENT file, get it's size

        // FIXME TODO

        // if not, return 0 as stream length
        stream->BaseAddress = 0;
        stream->Length = 0;        
    }

    return true;
}

///////////////////////////////////////////////////
// BlockStorageList 
///////////////////////////////////////////////////

extern struct BlockStorageDevice    Device_BlockStorage;
extern struct MEMORY_MAPPED_NOR_BLOCK_CONFIG   Device_BlockStorageConfig;
extern BlockStorageList             g_BlockStorage;

// map here the Block Storage Interface to the CC32xx driver
IBlockStorageDevice CC32xxFlash_BlockStorageInterface =
{
    &CC32xxFlashDriver_InitializeDevice,
    &CC32xxFlashDriver_UninitializeDevice,
    &CC32xxFlashDriver_GetDeviceInfo,
    &CC32xxFlashDriver_Read,
    &CC32xxFlashDriver_Write,
    NULL,
    &CC32xxFlashDriver_IsBlockErased,
    &CC32xxFlashDriver_EraseBlock,
    &CC32xxFlashDriver_SetPowerState
};
