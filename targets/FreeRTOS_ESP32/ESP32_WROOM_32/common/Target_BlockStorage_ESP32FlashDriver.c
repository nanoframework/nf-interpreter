//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_BlockStorage_Esp32FlashDriver.h>


extern int ets_printf(const char *fmt, ...);

const DRAM_ATTR esp_partition_t * g_pFlashDriver_partition;
const void * g_esp32_flash_start_ptr;
spi_flash_mmap_handle_t  g_esp32_flash_out_handle;

static const char* TAG = "FlashDriver";


/////////////////////////////////////////////////////////
// Description:
//    Initializes a given block device for use
//    Find the first Application partion and map to memory
//
// Input:
//
// Returns:
//   true if successful; false if not
//
// Remarks:
//    No other functions in this interface may be called
//    until after Init returns.
//
bool Esp32FlashDriver_InitializeDevice(void* context)
{
	(void)context;

//	ets_printf("InitializeDevice enter\n");

	g_esp32_flash_start_ptr = NULL;

	// Find the application partition and save partition entry
	g_pFlashDriver_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NANOCLR, NULL);
	if (g_pFlashDriver_partition == NULL)
	{
		ESP_LOGE(TAG, "InitializeDevice failed\n");
		return false;
	}
	
//	ets_printf("InitializeDevice partition found\n");

	// Map Partition into memory
	if (esp_partition_mmap(g_pFlashDriver_partition, 0, g_pFlashDriver_partition->size, SPI_FLASH_MMAP_DATA, &g_esp32_flash_start_ptr, &g_esp32_flash_out_handle) != ESP_OK)
	{
		ESP_LOGE(TAG, "Map memory failed\n");
		return false;
	}	

//	ets_printf("InitializeDevice flash adr %X mapped %X\n", g_pFlashDriver_partition->address, g_esp32_flash_start_ptr);
	
	return true;
}

/////////////////////////////////////////////////////////
// Description:
//    Uninitializes a given block device
// 
// Returns:
//   true if successful; false if not
//
// Remarks:
//   De initializes the device when no longer needed
//
bool Esp32FlashDriver_UninitializeDevice(void* context)
{
	(void)context;

	// Unmap the flash memory
	if (g_esp32_flash_start_ptr != NULL) {
		spi_flash_munmap(g_esp32_flash_out_handle);
		g_esp32_flash_start_ptr = NULL;
	}
	
	return true;
}

/////////////////////////////////////////////////////////
// Description:
//    Gets the information describing the device
//
DeviceBlockInfo* Esp32FlashDriver_GetDeviceInfo(void* context)
{
	(void)context;

    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = context;
    
    return config->BlockConfig.BlockDeviceInformation;  
}

/////////////////////////////////////////////////////////
// Description:
//    Reads data from a set of sectors
//
// Input:
//    startAddress - Starting Sector for the read
//    numBytes     - Number of sectors to read
//    buffer       - pointer to buffer to read the data into.
//                  Must be large enough to hold all of the data
//                  being read.
//
// Returns:
//   true if successful; false if not
//
// Remarks:
//   This function reads the number of sectors specified from the device.
//   
bool Esp32FlashDriver_Read(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer)
{
	(void)context;

//	ets_printf("Flash Read adr %X bytes %X target %X\n", startAddress, numBytes, buffer);

	if (startAddress == 0) return false;

    unsigned char* memStartAdr = (startAddress - g_pFlashDriver_partition->address) + (unsigned char *)g_esp32_flash_start_ptr;
    unsigned char* memEndAdr = memStartAdr + numBytes;

//	ets_printf("Flash Read memStartAdr %X memEndAdr %X Target %X\n", memStartAdr, memEndAdr, buffer);

	while (memStartAdr < memEndAdr) {
		*buffer++ = *memStartAdr++;
	}

    // ... and always return true
    return true;
}

/////////////////////////////////////////////////////////
// Description:
//    Writes data to a set of flash sectors
//
// Input:
//    startAddress    - Starting address for the write
//    numBytes        - Number of sectors to write
//    buffer          - pointer to data to write.
//                      Must be large enough to hold complete sectors
//                      for the number of sectors being written.
//    readModifyWrite - Not used for flash
//
// Returns:
//   true if successful; false if not
//
// Remarks:
//   This function reads the number of bytes specified from the device.
//   
bool Esp32FlashDriver_Write(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite)
{
	(void)context;

	//ets_printf("Flash write adr %X bytes %X  src %X\n", startAddress, numBytes, buffer);

	size_t partitionOffset = (size_t)startAddress;
	size_t size = numBytes;
//	ets_printf("Flash write offset %X bytes %X\n", partitionOffset, size);

	// Check in range of partition
	if (  (startAddress + size) > g_pFlashDriver_partition->size )
	{
		ESP_LOGE(TAG, "Invalid write address\n");
		return false;
	}


	// Read-modify-write is used for FAT filesystems only
	if (readModifyWrite) return false;

	if (esp_partition_write(g_pFlashDriver_partition, partitionOffset, (const void*)buffer, (size_t)size) != ESP_OK)
		return false;
	
	return true;
}

/////////////////////////////////////////////////////////
// Description:
//    Check a block is erased or not.
// 
// Input:
//    blockAddress - Logical start address 
//	  length       - Length of block
//
// Returns:
//   true if it is erased, otherwise false
//
// Remarks:
//    Check the block containing the sector address specified is erased.
//    
bool Esp32FlashDriver_IsBlockErased(void* context, ByteAddress blockAddress, unsigned int length)
{
	(void)context;

//	ets_printf("IsBlockErased %X %X start mem %X\n", blockAddress, length, g_esp32_flash_start_ptr);

	if ( length > g_pFlashDriver_partition->size ) {
		ESP_LOGE(TAG, "Invalid Invalid flash address/length\n");
		return false;
	}

	unsigned char * memStartAdr = (unsigned char *)(g_esp32_flash_start_ptr + blockAddress);
    unsigned char * memEndAdr = memStartAdr + length;

//	ets_printf("IsBlockErased mem start %X end %X\n", memStartAdr, memEndAdr);

	while (memStartAdr < memEndAdr) {
		if (*memStartAdr != 0xff)
		{
//			ets_printf("IsBlockErased exit FALSE\n");
			return false;
		}
		memStartAdr++;
	}
//	ets_printf("IsBlockErased exit true\n");

	return true;
}

/////////////////////////////////////////////////////////
// Description:
//    Erases a block
// 
// Input:
//    address - Logical Sector Address ( in partition )
//
// Returns:
//   true if successful; false if not
//
// Remarks:
//    Erases the block containing the sector address specified.
//    
bool Esp32FlashDriver_EraseBlock(void* context, ByteAddress address)
{
	(void)context;

//	ets_printf("EraseBlock start part adr:%X start mem: %X size:%X erase adr:%X\n", g_pFlashDriver_partition->address, g_esp32_flash_start_ptr, g_pFlashDriver_partition->size, address);

	uint32_t start_addr = address * SPI_FLASH_SEC_SIZE;
	uint32_t size = g_pFlashDriver_partition->size;
//	ets_printf("EraseBlock start addr %X\n", start_addr);

	if (esp_partition_erase_range(g_pFlashDriver_partition, start_addr, size) != ESP_OK)
	{
		ESP_LOGE(TAG, "EraseBlock Fail\n");
		return false;
	}
//	ets_printf("EraseBlock OK\n");
	return true;   
}

void Esp32FlashDriver_SetPowerState(void* context, unsigned int state)
{
	(void)context;
	(void)state;

    // nothing to be done here
}
