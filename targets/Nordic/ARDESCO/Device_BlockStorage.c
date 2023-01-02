//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

#include <Device_BlockStorage.h>

//
// The Ardesco has a 8MB SPI flash that can be used to hold
// managed code. However, this impacts RAM use as it's not XIP.
//
#ifdef USE_EXTERNAL_FLASH
// NOR Flash
const BlockRange BlockRange0[] = // 4KB blocks
    {
        {BlockRange_BLOCKTYPE_CONFIG, 0x000, 0x00F},    // Config
        {BlockRange_BLOCKTYPE_STORAGE_A, 0x010, 0x1FF}, // Storage
        {BlockRange_BLOCKTYPE_DEPLOYMENT, 0x200, 0x5FF} // 0x00200000 deployment
};

const BlockRegionInfo BlockRegions[] = {
    {
        (0),        // no attributes for this region
        0x00000000, // start address for block region
        0x800,      // total number of blocks in this region
        0x1000,     // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange0),
        BlockRange0,
    },
};
const DeviceBlockInfo Device_BlockInfo = {
    (0),                                // NOR flash memory is not XIP
    4,                                  // UINT32 BytesPerSector
    ARRAYSIZE_CONST_EXPR(BlockRegions), // UINT32 NumRegions;
    (BlockRegionInfo *)BlockRegions,    // const BlockRegionInfo* pRegions;
};

#else
// SOC flash
const BlockRange BlockRange0[] = // 4KB blocks
    {
        {BlockRange_BLOCKTYPE_BOOTSTRAP, 0x00, 0x0F}, // Secure bootloader
        {BlockRange_BLOCKTYPE_CODE, 0x40, 0xAF},      // nanoCLR
        // Using slot1_ns_partition for managed code deployment

        //{ BlockRange_BLOCKTYPE_DEPLOYMENT,  0xB0,  0xEF }      // deployment
        {BlockRange_BLOCKTYPE_CONFIG, 0x80, 0x8F},    // Config
        {BlockRange_BLOCKTYPE_DEPLOYMENT, 0x90, 0xEF} // deployment
};

const BlockRegionInfo BlockRegions[] = {
    {
        (0),        // no attributes for this region
        0x00000000, // start address for block region
        0x100,      // total number of blocks in this region
        0x1000,     // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange0),
        BlockRange0,
    },
};

const DeviceBlockInfo Device_BlockInfo = {
    (MediaAttribute_SupportsXIP),       // SOC flash memory is XIP
    4,                                  // UINT32 BytesPerSector
    ARRAYSIZE_CONST_EXPR(BlockRegions), // UINT32 NumRegions;
    (BlockRegionInfo *)BlockRegions,    // const BlockRegionInfo* pRegions;
};
#endif

MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig = {
    {
        // BLOCK_CONFIG
        {
            0,     // GPIO_PIN             Pin;
            false, // BOOL                 ActiveState;
        },

        (DeviceBlockInfo *)&Device_BlockInfo // BlockDeviceinfo
    },

    {
        // CPU_MEMORY_CONFIG
        0,          // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        true,       // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        0,          // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        0,          // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        8,          // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        0x00000000, // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x00100000, // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable
        0,          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead
        0,          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0, // UINT32 ChipProtection;
    0, // UINT32 ManufacturerCode;
    0, // UINT32 DeviceCode;
};

BlockStorageDevice Device_BlockStorage;