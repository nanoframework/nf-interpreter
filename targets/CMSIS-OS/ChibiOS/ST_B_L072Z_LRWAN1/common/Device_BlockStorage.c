//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

// 128 bytes block
const BlockRange BlockRange1[] = 
{
    { BlockRange_BLOCKTYPE_BOOTSTRAP ,   0    , 67   },        // 0x08000000 nanoBooter          
    { BlockRange_BLOCKTYPE_CODE      ,   68   , 1110 },        // 0x08002200 nanoCLR
    { BlockRange_BLOCKTYPE_DEPLOYMENT,   1111 , 1535 }         // 0x08022B80 deployment  
};

const BlockRegionInfo BlockRegions[] = 
{
    {
        0x08000000,                         // start address for block region
        1536,                               // total number of blocks in this region
        0x80,                               // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },
};

const DeviceBlockInfo Device_BlockInfo =
{
    (MediaAttribute_SupportsXIP),
    ARRAYSIZE_CONST_EXPR(BlockRegions),     // UINT32 NumRegions;
    (BlockRegionInfo*)BlockRegions,         // const BlockRegionInfo* pRegions;
};

MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig =
{
    { // BLOCK_CONFIG
        {
            0,          // GPIO_PIN             Pin;
            false,      // BOOL                 ActiveState;
        },

        (DeviceBlockInfo*)&Device_BlockInfo,    // BlockDeviceinfo
    },

    { // CPU_MEMORY_CONFIG
        0,                          // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        true,                       // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        0,                          // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        0,                          // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        16,                         // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        0x08000000,                 // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x00040000,                 // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,                          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                              // UINT32 ChipProtection;
    0,                              // UINT32 ManufacturerCode;
    0,                              // UINT32 DeviceCode;
};

__attribute__((__used__))  BlockStorageDevice    Device_BlockStorage;