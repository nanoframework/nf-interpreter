//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_Esp32FlashDriver.h>

extern int ets_printf(const char *fmt, ...);

const DRAM_ATTR esp_partition_t * g_pFlashDriver_partition;
const void * g_esp32_flash_start_ptr;
spi_flash_mmap_handle_t  g_esp32_flash_out_handle;

static const char* TAG = "FlashDriver";

bool Esp32FlashDriver_InitializeDevice(void* context)
{
	(void)context;

	//ets_printf("InitializeDevice enter\n");

	g_esp32_flash_start_ptr = NULL;

	// Find the application partition and save partition entry
	g_pFlashDriver_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NANOCLR, NULL);
	if (g_pFlashDriver_partition == NULL)
	{
		ESP_LOGE(TAG, "InitializeDevice failed\n");
		return false;
	}
	
	//ets_printf("InitializeDevice partition found\n");

	// Map Partition into memory
	if (esp_partition_mmap(g_pFlashDriver_partition, 0, g_pFlashDriver_partition->size, SPI_FLASH_MMAP_DATA, &g_esp32_flash_start_ptr, &g_esp32_flash_out_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "Map memory failed\n");
		return false;
	}

	//ets_printf("InitializeDevice flash adr %X mapped %X\n", g_pFlashDriver_partition->address, g_esp32_flash_start_ptr);
	
	return true;
}

bool Esp32FlashDriver_UninitializeDevice(void* context)
{
	(void)context;

	// Unmap the flash memory
	if (g_esp32_flash_start_ptr != NULL)
	{
		spi_flash_munmap(g_esp32_flash_out_handle);
		g_esp32_flash_start_ptr = NULL;
	}
	
	return true;
}

DeviceBlockInfo* Esp32FlashDriver_GetDeviceInfo(void* context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = context;
    
    return config->BlockConfig.BlockDeviceInformation;  
}

bool Esp32FlashDriver_Read(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer)
{
	//	ets_printf("Flash Read adr %X bytes %X target %X\n", startAddress, numBytes, buffer);

    uint32_t regionIndex, rangeIndex;
	unsigned char* memStartAdr = NULL;

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    DeviceBlockInfo* deviceInfo = config->BlockConfig.BlockDeviceInformation;

    if(!DeviceBlockInfo_FindRegionFromAddress(deviceInfo, startAddress, &regionIndex, &rangeIndex))
	{
 		return false;
	}

	BlockRegionInfo* region = &deviceInfo->Regions[regionIndex];
	ByteAddress readAddress = BlockRegionInfo_OffsetFromBlock(region, startAddress);

	// setup memory mapped start address
	switch(BlockRange_GetBlockUsage(deviceInfo->Regions[regionIndex].BlockRanges[rangeIndex]))
	{
		case BlockUsage_CODE:
			memStartAdr = 0;
			break;

		case BlockUsage_DEPLOYMENT:
			memStartAdr = (unsigned char *)g_esp32_flash_start_ptr + readAddress;
			break;

		default:
			return false;
	}

	// sanity check for region address overflowing
	if ((readAddress + numBytes) > BlockRegionInfo_Size(region) )
	{
		ESP_LOGE(TAG, "Invalid Invalid flash address/length\n");
		return false;
	}

	// copy from memory mapped address to buffer
	memcpy(buffer, memStartAdr, numBytes);

	//ets_printf("Flash Read memStartAdr %X memEndAdr %X Target %X\n", memStartAdr, memStartAdr + numBytes, buffer);

    return true;
}

bool Esp32FlashDriver_Write(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite)
{
    uint32_t regionIndex, rangeIndex;

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    DeviceBlockInfo* deviceInfo = config->BlockConfig.BlockDeviceInformation;
    if(!DeviceBlockInfo_FindRegionFromAddress(deviceInfo, startAddress, &regionIndex, &rangeIndex))
	{
 		return false;
	}

	BlockRegionInfo* region = &deviceInfo->Regions[regionIndex];
	ByteAddress offsetAddress = BlockRegionInfo_OffsetFromBlock(region, startAddress);

	//ets_printf("Flash write adr %X bytes %X  src %X\n", startAddress, numBytes, buffer);

	// Check in range of partition
	if (numBytes > (BlockRegionInfo_Size(region) - offsetAddress))
	{
		ESP_LOGE(TAG, "Invalid Invalid flash address/length\n");
		return false;
	}

	if (readModifyWrite)
	{
		return false;
	}

	// write buffer to partition
	if (esp_partition_write(g_pFlashDriver_partition, offsetAddress, (const void*)buffer, (size_t)numBytes) != ESP_OK)
	{
		return false;
	}
	
	return true;
}

bool Esp32FlashDriver_IsBlockErased(void* context, ByteAddress blockAddress, unsigned int length)
{
    uint32_t regionIndex, rangeIndex;
	unsigned char* memStartAdr = NULL;
    unsigned char* memEndAdr = NULL;

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    DeviceBlockInfo* deviceInfo = config->BlockConfig.BlockDeviceInformation;
    if(!DeviceBlockInfo_FindRegionFromAddress(deviceInfo, blockAddress, &regionIndex, &rangeIndex))
	{
 		return false;
	}

	BlockRegionInfo* region = &deviceInfo->Regions[regionIndex];

	// get memory mapped start address
	switch(BlockRange_GetBlockUsage(deviceInfo->Regions[regionIndex].BlockRanges[rangeIndex]))
	{
		case BlockUsage_CODE:
			memStartAdr = 0;
			break;

		case BlockUsage_DEPLOYMENT:
			memStartAdr = (unsigned char *)g_esp32_flash_start_ptr;
			break;

		default:
			return false;
	}

	// end address
	memEndAdr = memStartAdr + length;

	//ets_printf("IsBlockErased %X %X start mem %X\n", blockAddress, length, g_esp32_flash_start_ptr);

	if ( length > BlockRegionInfo_Size(region))
	{
		ESP_LOGE(TAG, "Invalid Invalid flash address/length\n");
		return false;
	}

	//ets_printf("IsBlockErased mem start %X end %X\n", memStartAdr, memEndAdr);

	// go through memory anf check for content other than 0xFF (flash erased value)
	while (memStartAdr < memEndAdr) 
	{
		if (*memStartAdr != 0xff)
		{
			//ets_printf("IsBlockErased exit FALSE\n");
			return false;
		}
		memStartAdr++;
	}

	//ets_printf("IsBlockErased exit true\n");

	return true;
}

bool Esp32FlashDriver_EraseBlock(void* context, ByteAddress address)
{
    uint32_t regionIndex, rangeIndex;

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    DeviceBlockInfo* deviceInfo = config->BlockConfig.BlockDeviceInformation;
    if(!DeviceBlockInfo_FindRegionFromAddress(deviceInfo, address, &regionIndex, &rangeIndex))
	{
 		return false;
	}

	BlockRegionInfo* region = &deviceInfo->Regions[regionIndex];
	ByteAddress startAddress = BlockRegionInfo_OffsetFromBlock(region, address);
	uint32_t size = BlockRegionInfo_Size(region);

	//ets_printf("EraseBlock start part adr:%X start mem: %X size:%X erase adr:%X\n", g_pFlashDriver_partition->address, g_esp32_flash_start_ptr, g_pFlashDriver_partition->size, address);
	//ets_printf("EraseBlock start addr %X\n", start_addr);

	if (esp_partition_erase_range(g_pFlashDriver_partition, startAddress, size) != ESP_OK)
	{
		ESP_LOGE(TAG, "EraseBlock Fail\n");
		return false;
	}

	//ets_printf("EraseBlock OK\n");

	return true;   
}

bool Esp32FlashDriver_GetMemoryMappedAddress(void* context, unsigned int blockRegionIndex, unsigned int blockRangeIndex, unsigned int* address)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG*)context;
    DeviceBlockInfo* deviceInfo = config->BlockConfig.BlockDeviceInformation;

  #if defined(DEBUG)
	// sanity check for non-existent block region
	if(blockRegionIndex > (deviceInfo->NumRegions + 1))
	{
		return false;
	}
  #endif

	switch(BlockRange_GetBlockUsage(deviceInfo->Regions[blockRegionIndex].BlockRanges[blockRangeIndex]))
	{
		case BlockUsage_CODE:
			*address = 0;
			break;

		case BlockUsage_DEPLOYMENT:
			*address = (unsigned int)g_esp32_flash_start_ptr;
			break;

		default:
			return false;
	}

	return true;
}
