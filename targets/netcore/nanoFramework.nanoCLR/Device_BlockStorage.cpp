//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

const UINT32 RAM_BLOCK_COUNT = 128;
const UINT32 RAM_BYTES_PER_BLOCK = 0x1000;
const UINT32 RAM_BYTES_PER_SECTOR = 1;

const BlockRange BlockRange1[] = {
    {BlockRange_BLOCKTYPE_DEPLOYMENT, 0, 125}, 
    {BlockRange_BLOCKTYPE_CONFIG, 126, 127}
};

const BlockRegionInfo BlockRegions[] = {
    {
        // no attributes for this region
        BlockRegionAttribute(0),
        // start address for block region
        0,
        // total number of blocks in this region
        RAM_BLOCK_COUNT,
        // total number of bytes per block
        RAM_BYTES_PER_BLOCK,
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },
};

const DeviceBlockInfo Device_BlockInfo = {
    // memory mapped can perform as XIP
    MediaAttribute(0),
    // UINT32 BytesPerSector
    RAM_BYTES_PER_SECTOR,
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
            // BOOL                 ActiveState;
            false, 
        },

        // BlockDeviceinfo
        (DeviceBlockInfo *)&Device_BlockInfo, 
    },

    {
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
        0x00000000,
        // UINT32 CPU_MEMORY_CONFIG::SizeInBytes;
        RAM_BLOCK_COUNT * RAM_BYTES_PER_BLOCK,
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
