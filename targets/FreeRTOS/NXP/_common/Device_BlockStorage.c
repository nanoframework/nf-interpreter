//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

const BlockRange BlockRange0[] = // 4KB blocks
{
    { BlockRange_BLOCKTYPE_BOOTSTRAP,   0x000,  0x00F },     // 0x60000000 nanoBooter  
    { BlockRange_BLOCKTYPE_CONFIG,      0x010,  0x01F },     // 0x60010000 configuration block
    { BlockRange_BLOCKTYPE_CODE,        0x020,  0x1FF },     // 0x60020000 nanoCLR  
    { BlockRange_BLOCKTYPE_DEPLOYMENT,  0x200,  0x2FF }      // 0x60200000 deployment  
};


const BlockRegionInfo BlockRegions[] = 
{
    {
        (0),                            // no attributes for this region
        0x60000000,                     // start address for block region
        0x300,                          // total number of blocks in this region
        0x1000,                         // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange0),
        BlockRange0,
    }
};

const DeviceBlockInfo Device_BlockInfo =
{
    (MediaAttribute_SupportsXIP),           // iMXRT flash memory is XIP
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

        (DeviceBlockInfo*)&Device_BlockInfo              // BlockDeviceinfo
    },

    { // CPU_MEMORY_CONFIG
        0,                          // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        true,                       // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        0,                          // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        0,                          // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        16,                         // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        0x60000000,                 // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x00800000,                 // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,                          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                              // UINT32 ChipProtection;
    0,                              // UINT32 ManufacturerCode;
    0,                              // UINT32 DeviceCode;
};

BlockStorageDevice    Device_BlockStorage;