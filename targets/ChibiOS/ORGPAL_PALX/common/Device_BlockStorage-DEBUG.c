//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

// 32kB blocks
const BlockRange BlockRange1[] = {
    {BlockRange_BLOCKTYPE_BOOTSTRAP, 0, 1}, // 0x08000000 MCUboot (sectors 0-1, 64 kB)

    ///////////////////////////////////////////////////////////////////////////////////////
    // because this target is using a configuration block need to add the
    // configuration manager files to the CMake and call ConfigurationManager_Initialize()
    // in nanoBooter so the configuration can be managed when in booter mode
    ///////////////////////////////////////////////////////////////////////////////////////
    {BlockRange_BLOCKTYPE_CONFIG, 2, 2}, // 0x08010000 configuration block
    ///////////////////////////////////////////////////////////////////////////////////////
};

// 128kB block
// Internal sectors 3-4 (0x08018000, 32 kB + 128 kB) are intentionally unused: 0x08018000 is
// not a 256 kB boundary, so they cannot belong to a swap-using-offset primary slot. nanoCLR
// (image 0 primary) starts at the next 256 kB boundary, 0x08040000.
// const BlockRange BlockRange2[] = {
//     {BlockRange_BLOCKTYPE_CODE, 0, 0} // 0x08020000 nanoCLR
// };

// 256kB blocks
const BlockRange BlockRange3[] = {
    {BlockRange_BLOCKTYPE_CODE, 0, 3},      // 0x08040000 nanoCLR    (image 0 primary, 4 x 256 kB = 1024 kB, ends 0x08140000)
    {BlockRange_BLOCKTYPE_DEPLOYMENT, 4, 6} // 0x08140000 deployment (image 1 primary, 3 x 256 kB =  768 kB, ends 0x08200000)
};

const BlockRegionInfo BlockRegions[] = {
    {
        (0),        // no attributes for this region
        0x08000000, // start address for block region
        3,          // total number of blocks in this region (bootloader + config; sector 3 @ 0x08018000 unused)
        0x8000,     // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },

    {
        (0),        // no attributes for this region
        0x08040000, // start address for block region (sectors 3-4 @ 0x08018000-0x08040000 skipped — unused)
        7,          // total number of blocks in this region
        0x40000,    // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange3),
        BlockRange3,
    },

};

const DeviceBlockInfo Device_BlockInfo = {
    (MediaAttribute_SupportsXIP),       // STM32 flash memory is XIP
    2,                                  // UINT32 BytesPerSector
    ARRAYSIZE_CONST_EXPR(BlockRegions), // UINT32 NumRegions;
    (BlockRegionInfo *)BlockRegions,    // const BlockRegionInfo* pRegions;
};

MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig = {
    {
        // BLOCK_CONFIG
        {
            0,     // GPIO_PIN             Pin;
            false, // BOOL                 ActiveState;
        },

        (DeviceBlockInfo *)&Device_BlockInfo, // BlockDeviceinfo
    },

    {
        // CPU_MEMORY_CONFIG
        0,          // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        true,       // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        0,          // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        0,          // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        16,         // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        0x08000000, // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x00200000, // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0,          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable
        0,          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead
        0,          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0, // UINT32 ChipProtection;
    0, // UINT32 ManufacturerCode;
    0, // UINT32 DeviceCode;
};

BlockStorageDevice Device_BlockStorage;
