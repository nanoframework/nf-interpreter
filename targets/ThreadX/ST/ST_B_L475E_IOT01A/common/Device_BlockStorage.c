//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

// 2kB blocks
const BlockRange BlockRange1[] = {

    // 08000000 nanoBooter
    {BlockRange_BLOCKTYPE_BOOTSTRAP, 0, 15},

    // 08008000 nanoCLR
    {BlockRange_BLOCKTYPE_CODE, 16, 158},

    // 0804F800 deployment
    {BlockRange_BLOCKTYPE_DEPLOYMENT, 159, 509},

    ///////////////////////////////////////////////////////////////////////////////////////
    // because this target is using a configuration block need to add the
    // configuration manager files to the CMake and call ConfigurationManager_Initialize()
    // in nanoBooter so the configuration can be managed when in booter mode
    ///////////////////////////////////////////////////////////////////////////////////////
    // 080FF800 configuration block
    {BlockRange_BLOCKTYPE_CONFIG, 510, 511},
    ///////////////////////////////////////////////////////////////////////////////////////
};

const BlockRegionInfo BlockRegions[] = {
    {
        // STM32L4 flash requires 64bits width programming
        (BlockRegionAttribute_ProgramWidthIs64bits),

        // start address for block region
        0x08000000,

        // total number of blocks in this region
        512,

        // total number of bytes per block
        0x800,

        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },
};

const DeviceBlockInfo Device_BlockInfo = {

    // STM32 flash memory is XIP
    (MediaAttribute_SupportsXIP),

    // UINT32 BytesPerSector
    2,

    // UINT32 NumRegions;
    ARRAYSIZE_CONST_EXPR(BlockRegions),

    // const BlockRegionInfo* pRegions;
    (BlockRegionInfo *)BlockRegions,
};

MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig = {
    {
        // BLOCK_CONFIG
        {
            // GPIO_PIN             Pin;
            0,

            // BOOL                 ActiveState

            false,
        },

        // BlockDeviceinfo
        (DeviceBlockInfo *)&Device_BlockInfo,
    },

    {
        // CPU_MEMORY_CONFIG
        // UINT8  CPU_MEMORY_CONFIG::ChipSelect;
        0,

        // UINT8  CPU_MEMORY_CONFIG::ReadOnly;
        true,

        // UINT32 CPU_MEMORY_CONFIG::WaitStates;
        0,

        // UINT32 CPU_MEMORY_CONFIG::ReleaseCounts;
        0,

        // UINT32 CPU_MEMORY_CONFIG::BitWidth;
        16,

        // UINT32 CPU_MEMORY_CONFIG::BaseAddress;
        0x08000000,

        // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        0x00200000,

        // UINT8  CPU_MEMORY_CONFIG::XREADYEnable
        0,

        // UINT8  CPU_MEMORY_CONFIG::ByteSignalsForRead
        0,

        // UINT8  CPU_MEMORY_CONFIG::ExternalBufferEnable
        0,
    },

    // UINT32 ChipProtection;
    0,

    // UINT32 ManufacturerCode;
    0,

    // UINT32 DeviceCode;
    0,
};

BlockStorageDevice Device_BlockStorage;
