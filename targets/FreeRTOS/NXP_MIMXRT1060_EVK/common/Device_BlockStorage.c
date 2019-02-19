//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

const BlockRange BlockRange0[] = // 4KB blocks
{
    { BlockRange_BLOCKTYPE_BOOTSTRAP,   0, 0x1 }                    // 0x60000000 nanoCLR  
};


const BlockRange BlockRange1[] = // 4KB blocks
{
    { BlockRange_BLOCKTYPE_CODE,   0, 0xFD }                    // 0x60002000 nanoCLR  
};

const BlockRange BlockRange2[] = // 4KB blocks
{
    { BlockRange_BLOCKTYPE_DEPLOYMENT,   0, 0xFF }             // 0x60100000 deployment  
};

const BlockRegionInfo BlockRegions[] = 
{
    {
        (0),                            // no attributes for this region
        // Allocated in nanoBooter partition 8K
        0x60000000,                     // start address for block region
        0x2,                            // total number of blocks in this region
        0x1000,   //  4K                // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange0),
        BlockRange0,
    },
    {
        (0),                            // no attributes for this region
        // Allocated in nanoCLR partition 1M
        0x60002000,                     // start address for block region
        0xFE,                           // total number of blocks in this region
        0x1000,   //  4K                // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },

    {
        (0),                                // no attributes for this region
        // Allocated in Deployment partition 1M
        0x60100000,                         // start address for block region
        0x100,                              // total number of blocks in this region
        0x1000,   //  4K                    // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange2),
        BlockRange2,
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
        0x60002000,                 // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x001FE000,                 // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,                          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead 
        0,                          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0,                              // UINT32 ChipProtection;
    0,                              // UINT32 ManufacturerCode;
    0,                              // UINT32 DeviceCode;
};

BlockStorageDevice    Device_BlockStorage;