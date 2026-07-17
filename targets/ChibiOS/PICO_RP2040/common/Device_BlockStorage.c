//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

// RP2040 Pico flash layout (2MB external QSPI flash, 4KB sectors)
// 0x10000000 - 0x1000BFFF : nanoBooter   (48KB  = 12 sectors)
// 0x1000C000 - 0x10013FFF : Config block (32KB  = 8 sectors)
// 0x10014000 - 0x100FBFFF : nanoCLR      (928KB = 232 sectors)
// 0x100FC000 - 0x101F7FFF : Deployment   (1008KB = 252 sectors)
// 0x101F8000 - 0x101FFFFF : LittleFS     (32KB  = 8 sectors)

// 4KB sectors
const BlockRange BlockRange1[] = {
    // 0x10000000 nanoBooter (sectors 0-11)
    {BlockRange_BLOCKTYPE_BOOTSTRAP, 0, 11},

    // 0x1000C000 configuration block (sectors 12-19)
    {BlockRange_BLOCKTYPE_CONFIG, 12, 19},

    // 0x10014000 nanoCLR (sectors 20-251)
    {BlockRange_BLOCKTYPE_CODE, 20, 251},

    // 0x10100000 deployment (sectors 252-503)
    {BlockRange_BLOCKTYPE_DEPLOYMENT, 252, 503},

    // 0x101F8000 littlefs (sectors 504-511)
    {BlockRange_BLOCKTYPE_FILESYSTEM, 504, 511}};

const BlockRegionInfo BlockRegions[] = {
    {
        (0),        // no attributes for this region
        0x10000000, // start address for block region (XIP base)
        512,        // total number of blocks in this region (2MB / 4KB)
        0x1000,     // total number of bytes per block (4KB)
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },
};

const DeviceBlockInfo Device_BlockInfo = {
    (MediaAttribute_SupportsXIP), // RP2040 flash is XIP (execute in place)
    2,                            // UINT32 BytesPerSector
    ARRAYSIZE_CONST_EXPR(BlockRegions),
    (BlockRegionInfo *)BlockRegions,
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
        8,          // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        0x10000000, // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x00200000, // UINT32 CPU_MEMORY_CONFIG::SizeInBytes; (2MB)
        0,          // UINT8  CPU_MEMORY_CONFIG::XREADYEnable
        0,          // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead
        0,          // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
    },

    0, // UINT32 ChipProtection;
    0, // UINT32 ManufacturerCode;
    0, // UINT32 DeviceCode;
};

__attribute__((__used__)) BlockStorageDevice Device_BlockStorage;
