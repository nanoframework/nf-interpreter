//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

const BlockRange BlockRange1[] = 
{
    { BlockRange_BLOCKTYPE_BOOTSTRAP ,   0, 7   }           // 08000000 nanoBooter          
};

const BlockRange BlockRange2[] = 
{
    { BlockRange_BLOCKTYPE_CODE      ,   0, 9   }           // 08004000 nanoCLR          
};

const BlockRange BlockRange3[] =
{
    { BlockRange_BLOCKTYPE_CODE      ,   0, 9   },          // 08020000 nanoCLR         
    { BlockRange_BLOCKTYPE_DEPLOYMENT,   10, 26 }           // 08080000 deployment  
};

const BlockRegionInfo BlockRegions[] = 
{
    {
        0x08000000,                         // start address for block region
        8,                                  // total number of blocks in this region
        0x800,                              // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },

    {
        0x08004000,                         // start address for block region
        10,                                 // total number of blocks in this region
        0x800,                              // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange2),
        BlockRange2,
    },

    {
        0x08027000,                         // start address for block region
        26,                                 // total number of blocks in this region
        0x800,                              // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange3),
        BlockRange3,
    }
    
};

const DeviceBlockInfo Device_BlockInfo =
{
    {  
        false,              // BOOL Removable;
        true,               // BOOL SupportsXIP;
        false,              // BOOL WriteProtected;
        false               // BOOL SupportsCopyBack
    },
    ARRAYSIZE_CONST_EXPR(BlockRegions),     // UINT32 NumRegions;
    BlockRegions,                           // const BlockRegionInfo* pRegions;
};

MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig =
{
    { // BLOCK_CONFIG
        {
            0,          // GPIO_PIN             Pin;
            false,      // BOOL                 ActiveState;
        },

        &Device_BlockInfo,             // BlockDeviceinfo
    },

    { // CPU_MEMORY_CONFIG
        0,                          // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        true,                       // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        0,                          // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        0,                          // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        16,                         // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        0x08000000,                 // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x00010000,                 // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,                          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                              // UINT32 ChipProtection;
    0,                              // UINT32 ManufacturerCode;
    0,                              // UINT32 DeviceCode;
};

__attribute__((__used__))  BlockStorageDevice    Device_BlockStorage;