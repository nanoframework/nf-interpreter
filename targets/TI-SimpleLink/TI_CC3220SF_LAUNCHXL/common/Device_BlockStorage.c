//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

// 2k blocks
const BlockRange BlockRange1[] =
{
    // the 1st block is reserved for the flash header 
    // so we don't take it into account for the map
    { BlockRange_BLOCKTYPE_CODE          ,   0  , 104 },           // 0x01000800 nanoCLR
    { BlockRange_BLOCKTYPE_DEPLOYMENT    ,   105, 510 },           // 0x01034800 deployment
};

const BlockRegionInfo BlockRegions[] = 
{
    {
        (0),                                // no attributes for this region
        0x01000800,                         // start address for block region
        511,                                // total number of blocks in this region
        0x800,                              // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    }
};

const DeviceBlockInfo Device_BlockInfo =
{
    (MediaAttribute_SupportsXIP),
    2,                                      // UINT32 BytesPerSector
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
