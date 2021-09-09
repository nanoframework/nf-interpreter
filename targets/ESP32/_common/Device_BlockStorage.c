//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

#include <esp32_idf.h>
#include <esp_partition.h>

#include <Target_BlockStorage_Esp32FlashDriver.h>

// the block ranges mirror the partition table
// spliting each partition on it's own block range is required to use memory mapping

const BlockRange BlockRange1[] = {
    // partition: factory (nanoCLR)
    {BlockRange_BLOCKTYPE_CODE, 0, 0},
};

const BlockRange BlockRange2[] = {
    // partition: deploy
    {BlockRange_BLOCKTYPE_DEPLOYMENT, 0, 0},
};

const BlockRange BlockRange3[] = {
    // partition: config
    {BlockRange_BLOCKTYPE_CONFIG, 0, 0}};

// The block regions are defined with 0 as the start address and 0 as total bytes per block.
// At boot time, these are filled in @ FixUpBlockRegionInfo() from the partition table.
// For simplicity, each region is considered to have a single block.

BlockRegionInfo BlockRegions[] = {
    // nanoCLR
    {
        // no attributes for this region
        (0),
        // start address for block region
        0,
        // total number of blocks in this region
        1,
        // total number of bytes per block
        0,
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },

    // Deployment area
    {
        // this region is memory mapped
        (BlockRegionAttribute_MemoryMapped),
        // start address for block region
        0,
        // total number of blocks in this region
        1,
        // total number of bytes per block
        0,
        ARRAYSIZE_CONST_EXPR(BlockRange2),
        BlockRange2,
    },

    // Config, SPIFS partition
    {
        // no attributes for this region
        (0),
        // start address for block region
        0,
        // total number of blocks in this region
        1,
        // total number of bytes per block
        0,
        ARRAYSIZE_CONST_EXPR(BlockRange3),
        BlockRange3,
    }};

const DeviceBlockInfo Device_BlockInfo = {
    // ESP32 storage doesn't supports XIP, instead it provides memory mapped read access to SPI storage
    (0),
    // UINT32 BytesPerSector
    1,
    // UINT32 NumRegions;
    ARRAYSIZE_CONST_EXPR(BlockRegions),
    // const BlockRegionInfo* pRegions;
    (BlockRegionInfo *)BlockRegions};

MEMORY_MAPPED_NOR_BLOCK_CONFIG Device_BlockStorageConfig = {
    {
        // BLOCK_CONFIG
        {
            // GPIO_PIN Pin;
            0,
            // BOOL ActiveState;
            false,
        },

        (DeviceBlockInfo *)&Device_BlockInfo // BlockDeviceinfo
    },

    // CPU_MEMORY_CONFIG
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

//
//  Fix up Block region info based on current partion layout
//
void FixUpBlockRegionInfo()
{
    // nanoCLR
    const esp_partition_t *part_nanoClr =
        esp_partition_find_first(ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_APP_FACTORY, 0);
    if (part_nanoClr)
    {
        BlockRegions[0].Start = part_nanoClr->address;
        BlockRegions[0].BytesPerBlock = part_nanoClr->size;
    }

    // Deployment
    const esp_partition_t *part_deploy =
        esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NANOCLR, 0);
    if (part_deploy)
    {
        BlockRegions[1].Start = part_deploy->address;
        BlockRegions[1].BytesPerBlock = part_deploy->size;
    }

    // Config
    const esp_partition_t *part_config =
        esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, 0);
    if (part_config)
    {
        BlockRegions[2].Start = part_config->address;
        BlockRegions[2].BytesPerBlock = part_config->size;
    }
}
