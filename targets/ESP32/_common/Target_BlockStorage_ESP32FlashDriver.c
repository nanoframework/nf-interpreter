//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_Esp32FlashDriver.h>

extern int ets_printf(const char *fmt, ...);

const DRAM_ATTR esp_partition_t *g_pFlashDriver_partition;
const void *esp32_flash_start_ptr;
spi_flash_mmap_handle_t g_esp32_flash_out_handle;

// storing the CODE block start and size for quick access
static ByteAddress CODE_Region_StartAddress;
static uint32_t CODE_Region_Size;

// storing the DEPLOYMENT block start and size for quick access
static ByteAddress DEPLOYMENT_Region_StartAddress;
static uint32_t DEPLOYMENT_Region_Size;

static const char *TAG = "FlashDriver";

// developer notes:
// 1. before the code calls this driver it's expected that the requested operation has been validated before
// 2. with ESP32 platform we are only allowed to write to the DEPLOYMENT regions, therefore this implementation it's
// based on that assumption
// 3. no other validations or sanity checks are performed
// 4. this driver only accesses CODE and DEPLOYMENT regions
// 5. there is only a single block for CODE and a single block for DEPLOYMENT

//--//
// helper functions

bool AddressIn_CODE_Region(uint32_t address)
{
    return ((address >= CODE_Region_StartAddress) && (address < CODE_Region_StartAddress + CODE_Region_Size));
}

bool AddressIn_DEPLOYMENT_Region(uint32_t address)
{
    return (
        (address >= DEPLOYMENT_Region_StartAddress) &&
        (address < DEPLOYMENT_Region_StartAddress + DEPLOYMENT_Region_Size));
}

//--//

bool Esp32FlashDriver_InitializeDevice(void *context)
{
    (void)context;

    uint32_t rangeCount = 0;
    uint32_t rangeIndex = 0;
    esp32_flash_start_ptr = NULL;

    // Find the application partition and save partition entry
    g_pFlashDriver_partition =
        esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NANOCLR, NULL);
    if (g_pFlashDriver_partition == NULL)
    {
        ESP_LOGE(TAG, "InitializeDevice failed\n");
        return false;
    }

    // Map Partition into memory
    if (esp_partition_mmap(
            g_pFlashDriver_partition,
            0,
            g_pFlashDriver_partition->size,
            SPI_FLASH_MMAP_DATA,
            &esp32_flash_start_ptr,
            &g_esp32_flash_out_handle) != ESP_OK)
    {
        ESP_LOGE(TAG, "Map memory failed\n");
        return false;
    }

    ESP_LOGI(TAG, "PartitionSize %d\n", g_pFlashDriver_partition->size);
    ESP_LOGI(TAG, "Partition start pointer %X\n", (int32_t)esp32_flash_start_ptr);

    // go through the regions in order to grab the details on the block storage regions addresses and size
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;
    DeviceBlockInfo *deviceInfo = config->BlockConfig.BlockDeviceInformation;

    for (int cnt = 0; cnt < 2; cnt++)
    {
        for (unsigned int i = 0; i < deviceInfo->NumRegions; i++)
        {
            const BlockRegionInfo *pRegion = &deviceInfo->Regions[i];

            for (unsigned int j = 0; j < pRegion->NumBlockRanges; j++)
            {
                if (cnt == 0)
                {
                    rangeCount++;
                }
                else
                {
                    if (((pRegion->BlockRanges[j].RangeType & BlockRange_USAGE_MASK) == BlockUsage_CODE))
                    {
                        CODE_Region_StartAddress =
                            BlockRegionInfo_BlockAddress(pRegion, pRegion->BlockRanges[j].StartBlock);
                        CODE_Region_Size = BlockRange_GetBlockCount(pRegion->BlockRanges[j]) * pRegion->BytesPerBlock;
                    }
                    else if (((pRegion->BlockRanges[j].RangeType & BlockRange_USAGE_MASK) == BlockUsage_DEPLOYMENT))
                    {
                        DEPLOYMENT_Region_StartAddress =
                            BlockRegionInfo_BlockAddress(pRegion, pRegion->BlockRanges[j].StartBlock);
                        DEPLOYMENT_Region_Size =
                            BlockRange_GetBlockCount(pRegion->BlockRanges[j]) * pRegion->BytesPerBlock;
                    }

                    rangeIndex++;
                }
            }
        }
    }

    ESP_LOGI(TAG, "CODE_Region_StartAddress %X\n", CODE_Region_StartAddress);
    ESP_LOGI(TAG, "CODE_Region_Size %d\n", CODE_Region_Size);

    ESP_LOGI(TAG, "DEPLOYMENT_Region_StartAddress %X\n", DEPLOYMENT_Region_StartAddress);
    ESP_LOGI(TAG, "DEPLOYMENT_Region_Size %d\n", DEPLOYMENT_Region_Size);

    return true;
}

bool Esp32FlashDriver_UninitializeDevice(void *context)
{
    (void)context;

    // Unmap the flash memory
    if (esp32_flash_start_ptr != NULL)
    {
        spi_flash_munmap(g_esp32_flash_out_handle);
        esp32_flash_start_ptr = NULL;
    }

    return true;
}

DeviceBlockInfo *Esp32FlashDriver_GetDeviceInfo(void *context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = context;

    return config->BlockConfig.BlockDeviceInformation;
}

bool Esp32FlashDriver_Read(void *context, ByteAddress startAddress, unsigned int numBytes, unsigned char *buffer)
{
    (void)context;

    uint8_t *memStartAdr;
    ByteAddress readAddress;

    // setup memory mapped start address
    // need to compute the address offset from the region (block) start
    if (AddressIn_CODE_Region(startAddress))
    {
        memStartAdr = (uint8_t *)(startAddress - CODE_Region_StartAddress);
    }
    else if (AddressIn_DEPLOYMENT_Region(startAddress))
    {
        readAddress = startAddress - DEPLOYMENT_Region_StartAddress;
        memStartAdr = (uint8_t *)esp32_flash_start_ptr + readAddress;
    }
    else
    {
        return false;
    }

    // copy from memory mapped address to buffer
    memcpy(buffer, memStartAdr, numBytes);

    return true;
}

bool Esp32FlashDriver_Write(
    void *context,
    ByteAddress startAddress,
    unsigned int numBytes,
    unsigned char *buffer,
    bool readModifyWrite)
{
    (void)context;
    (void)readModifyWrite;

    // this implementation here assumes that with ESP32 write operations are performed only in the DEPLOYMENT region
    // setup memory mapped start address
    // need to compute the address offset from the region (block) DEPLOYMENT block start address
    ByteAddress offsetAddress = startAddress - DEPLOYMENT_Region_StartAddress;

    ESP_LOGI(TAG, "Writting  %dB @ %d\n", numBytes, offsetAddress);

    // write buffer to partition
    return (
        esp_partition_write(g_pFlashDriver_partition, offsetAddress, (const void *)buffer, (size_t)numBytes) == ESP_OK);
}

bool Esp32FlashDriver_IsBlockErased(void *context, ByteAddress blockAddress, unsigned int length)
{
    (void)context;

    uint8_t *memStartAdr;
    uint8_t *memEndAdr;

    ESP_LOGI(TAG, "IsBlockErased  %dB @ %X\n", length, blockAddress);

    // need to setup the start and end addresses to the region (block) boundaries
    if (AddressIn_CODE_Region(blockAddress))
    {
        // start at 0 because this a memory mapped address
        memStartAdr = 0;
        // end address
        memEndAdr = (uint8_t *)CODE_Region_Size;
    }
    else if (AddressIn_DEPLOYMENT_Region(blockAddress))
    {
        memStartAdr = (uint8_t *)(esp32_flash_start_ptr + blockAddress - DEPLOYMENT_Region_StartAddress);
        // end address
        memEndAdr = (uint8_t *)(esp32_flash_start_ptr + g_pFlashDriver_partition->size);
    }
    else
    {
        ESP_LOGE(TAG, "Address is out of partition area\n");
        return false;
    }

    ESP_LOGI(TAG, "IsBlockErased checking from %X to %X\n", (uint32_t)memStartAdr, (uint32_t)memEndAdr);

    // go through memory and check for content other than 0xFF (flash erased value)
    while (memStartAdr < memEndAdr)
    {
        if (*memStartAdr != 0xff)
        {
            return false;
        }

        memStartAdr++;
    }

    return true;
}

bool Esp32FlashDriver_EraseBlock(void *context, ByteAddress address)
{
    (void)context;

    // this implementation here assumes that with ESP32 erase operations are performed only in the DEPLOYMENT region
    // and for the full block so the offset it's 0 and the size corresponds to the partition size

    return (esp_partition_erase_range(g_pFlashDriver_partition, 0, g_pFlashDriver_partition->size) == ESP_OK);
}

bool Esp32FlashDriver_GetMemoryMappedAddress(
    void *context,
    unsigned int blockRegionIndex,
    unsigned int blockRangeIndex,
    unsigned int *address)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG *config = (MEMORY_MAPPED_NOR_BLOCK_CONFIG *)context;
    DeviceBlockInfo *deviceInfo = config->BlockConfig.BlockDeviceInformation;

#if defined(DEBUG)
    // sanity check for non-existent block region
    if (blockRegionIndex > (deviceInfo->NumRegions + 1))
    {
        return false;
    }
#endif

    switch (BlockRange_GetBlockUsage(deviceInfo->Regions[blockRegionIndex].BlockRanges[blockRangeIndex]))
    {
        case BlockUsage_CODE:
            *address = 0;
            break;

        case BlockUsage_DEPLOYMENT:
            *address = (unsigned int)esp32_flash_start_ptr;
            break;

        default:
            return false;
    }

    return true;
}
