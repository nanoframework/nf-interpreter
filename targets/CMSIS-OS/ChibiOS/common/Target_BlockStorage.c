//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <targetHAL.h>
#include <Target_BlockStorage_STM32FlashDriver.h>

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
        stream->BaseAddress = &__nanoImage_start__;
        // set Length to the region size 
        stream->Length = &__nanoImage_end__ - &__nanoImage_start__;
    }
    else if(blockUsage == StorageUsage_DEPLOYMENT)
    {
        // set BaseAddress to the start of the region
        stream->BaseAddress = &__deployment_start__;
        // set Length to the region size 
        stream->Length = &__deployment_end__ - &__deployment_start__;
    }

    return true;
}

///////////////////////////////////////////////////
// BlockStorageList 
///////////////////////////////////////////////////

extern struct BlockStorageDevice    Device_BlockStorage;
extern struct MEMORY_MAPPED_NOR_BLOCK_CONFIG   Device_BlockStorageConfig;
BlockStorageList             BlockStorage;

// map here the Block Storage Interface to the STM32 driver
IBlockStorageDevice STM32Flash_BlockStorageInterface =
{                          
    &STM32FlashDriver_InitializeDevice,
    &STM32FlashDriver_UninitializeDevice,
    &STM32FlashDriver_GetDeviceInfo,
    &STM32FlashDriver_Read,
    &STM32FlashDriver_Write,
    NULL,
    &STM32FlashDriver_IsBlockErased,    
    &STM32FlashDriver_EraseBlock,
    &STM32FlashDriver_SetPowerState
};

void BlockStorage_AddDevices()
{
    BlockStorageList_AddDevice(&Device_BlockStorage, &STM32Flash_BlockStorageInterface, &Device_BlockStorageConfig, false);
}

bool BlockStorageList_FindDeviceForPhysicalAddress(BlockStorageDevice** pBSD, unsigned int physicalAddress, ByteAddress* blockAddress)
{
    *pBSD = NULL;
       
    BlockStorageDevice* block = BlockStorageList_GetFirstDevice;

    // this has to add to make metadataprocessor happy
    if(!block) return true;

    DeviceBlockInfo* pDeviceInfo = BlockStorageDevice_GetDeviceInfo(&block);
        
    for(int i=0; i < pDeviceInfo->NumRegions; i++)
    {
        BlockRegionInfo* pRegion = &pDeviceInfo->Regions[i];
        
        if(pRegion->Start <= physicalAddress && physicalAddress < (pRegion->Start + pRegion->NumBlocks * pRegion->BytesPerBlock))
        {
            *pBSD = block; 

            // get block start address 
            *blockAddress = (ByteAddress)((physicalAddress - pRegion->Start) / pRegion->BytesPerBlock);
            *blockAddress *= pRegion->BytesPerBlock;
            *blockAddress += pRegion->Start;

            return true;
        }
    }

    return false;
}

bool BlockStorageList_AddDevice(BlockStorageDevice* pBSD, IBlockStorageDevice* vtable, void* config, bool init)
{
    pBSD->m_BSD     = vtable;
    pBSD->m_context = config;

    BlockStorage.PrimaryDevice = pBSD;

    return true;
}

BlockStorageDevice* BlockStorageList_GetFirstDevice()
{
    return BlockStorage.PrimaryDevice;
}
