//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

// 16kB blocks
const BlockRange BlockRange1[] = 
{
    { BlockRange_BLOCKTYPE_BOOTSTRAP ,   0, 1 },            // 0x08000000 nanoBooter
    { BlockRange_BLOCKTYPE_CODE      ,   2, 3 }             // 0x08008000 nanoCLR
};

// 64kB blocks
const BlockRange BlockRange2[] = 
{
    { BlockRange_BLOCKTYPE_CODE      ,   0, 0 }             // 0x08010000 nanoCLR
};

// 128kB blocks
const BlockRange BlockRange3[] =
{
    { BlockRange_BLOCKTYPE_CODE      ,   0, 3 },            // 0x08020000 nanoCLR
    { BlockRange_BLOCKTYPE_DEPLOYMENT,   4, 6 },            // 0x080A0000 deployment
};

const BlockRegionInfo BlockRegions[] = 
{
    {
        0x08000000,                         // start address for block region
        4,                                  // total number of blocks in this region
        0x4000,                             // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },

    {
        0x08010000,                         // start address for block region
        1,                                  // total number of blocks in this region
        0x10000,                            // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange2),
        BlockRange2,
    },

    {
        0x08020000,                         // start address for block region
        7,                                  // total number of blocks in this region
        0x20000,                            // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange3),
        BlockRange3,
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
        0x00200000,                 // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,                          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                              // UINT32 ChipProtection;
    0,                              // UINT32 ManufacturerCode;
    0,                              // UINT32 DeviceCode;
};

BlockStorageDevice    Device_BlockStorage;
