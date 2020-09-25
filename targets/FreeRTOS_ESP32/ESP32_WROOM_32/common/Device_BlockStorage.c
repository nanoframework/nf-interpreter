//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL_Types.h>
#include <nanoPAL_BlockStorage.h>

#include <esp32_os.h>
#include <esp_partition.h>

// the block ranges mirror the partition table
// spliting each partition on it's own block range is required to use memory mapping


const BlockRange BlockRange1[] =
{
    // factory
    { BlockRange_BLOCKTYPE_CODE      ,   0, 0 },             // 0x010000   nanoCLR
};

const BlockRange BlockRange2[] =
{
    // deploy
    { BlockRange_BLOCKTYPE_DEPLOYMENT,   0, 0 },             // 0x110000   deployment  
};

const BlockRange BlockRange3[] =
{
    // config
    { BlockRange_BLOCKTYPE_CONFIG    ,   0, 0 }              // 0x2D0000   config
};

BlockRegionInfo BlockRegions[] = 
{
	// nanoCLR
    {
        (0),                                // no attributes for this region
        0x010000,                           // start address for block region
        1,                                  // total number of blocks in this region
        0x100000,                           // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange1),
        BlockRange1,
    },

	// Deployment area
    {
        (BlockRegionAttribute_MemoryMapped),    // this region is memory mapped
        0x110000,                               // start address for block region
        1,                                      // total number of blocks in this region
        0x70000,                                // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange2),
        BlockRange2,
    },

	// Config, SPIFS partition
    {
        (0),                                // no attributes for this region
        0x0,                                // start address for block region
        1,                                  // total number of blocks in this region
        0x10000,                            // total number of bytes per block
        ARRAYSIZE_CONST_EXPR(BlockRange3),
        BlockRange3,
    }
};


const DeviceBlockInfo Device_BlockInfo =
{
    (0),                                    // ESP32 storage doesn't supports XIP, instead it provides memory mapped read access to SPI storage
    1,                                      // UINT32 BytesPerSector
    ARRAYSIZE_CONST_EXPR(BlockRegions),     // UINT32 NumRegions;
    (BlockRegionInfo*)BlockRegions          // const BlockRegionInfo* pRegions;
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



//
//  Fix up Block region info based on current partion layout
//
void FixUpBlockRegionInfo()
{
	// nanoCLR
	const esp_partition_t * part_nanoClr = esp_partition_find_first(ESP_PARTITION_TYPE_APP, 0, 0);
	if (part_nanoClr)
	{
		BlockRegions[0].Start = part_nanoClr->address;
		BlockRegions[0].BytesPerBlock = part_nanoClr->size; 
	}
	// Deployment
	const esp_partition_t * part_deploy = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, 0x84, 0);
	if (part_deploy)
	{
		BlockRegions[1].Start = part_deploy->address;
		BlockRegions[1].BytesPerBlock = part_deploy->size;
	}
	
	// Config
	const esp_partition_t * part_config = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, 0);
	if (part_config)
	{
		BlockRegions[2].Start = part_config->address;
		BlockRegions[2].BytesPerBlock = part_config->size;
	}
	
}
