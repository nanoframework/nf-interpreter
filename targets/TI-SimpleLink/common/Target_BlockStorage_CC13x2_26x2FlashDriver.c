//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_BlockStorage_CC13x2_26x2FlashDriver.h>

// includes from SimpleLink
// #include "inc/hw_types.h"
#include <driverlib/flash.h>
#include <driverlib/vims.h>

// local defines
#define FLASH_ERASED_WORD        0x0FFFFFFFFU

static uint32_t disableFlashCache(void);
static void restoreFlashCache(uint32_t mode);

bool CC13x2_26x2FlashDriver_InitializeDevice(void* context)
{
	(void)context;

    // SimpleLink driver takes care of this, so always true
	return true;
}

bool CC13x2_26x2FlashDriver_UninitializeDevice(void* context)
{
	(void)context;

    // SimpleLink driver takes care of this, so always true
	return true;
}

DeviceBlockInfo* CC13x2_26x2FlashDriver_GetDeviceInfo(void* context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = context;
    
    return config->BlockConfig.BlockDeviceInformation;  
}

bool CC13x2_26x2FlashDriver_Read(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer)
{
	(void)context;

    // direct read from memory...
    memcpy(buffer, (void *)startAddress, (size_t)numBytes);

    // ... and always return true
    return true;
}

bool CC13x2_26x2FlashDriver_Write(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite)
{
    bool result = false;
    uint32_t mode;

	(void)context;
	(void)readModifyWrite;

    mode = disableFlashCache();

	if(FlashProgram((unsigned long*)buffer, (unsigned long) startAddress, (unsigned long) numBytes) == 0)
	{
		result = true;
	}

    restoreFlashCache(mode);
	
	return result;
}

bool CC13x2_26x2FlashDriver_IsBlockErased(void* context, ByteAddress blockAddress, unsigned int length)
{
	(void)context;

    unsigned long* cursor = (unsigned long *)blockAddress;
    unsigned long* endAddress = (unsigned long *)(blockAddress + length);

    // an erased flash address has to read FLASH_ERASED_WORD
    // OK to check by word (32 bits) because the erase is performed by 'block' whose size is word multiple
    while(cursor < endAddress)
    {
        if(*cursor++ != FLASH_ERASED_WORD)
        {
            // found an address with something other than FLASH_ERASED_WORD!!
            return false;
        }
    }
    
    // reached here so the block must be erased
    return true;
}

bool CC13x2_26x2FlashDriver_EraseBlock(void* context, ByteAddress address)
{
    bool result = false;
    uint32_t mode;

	(void)context;

    mode = disableFlashCache();
    
	if(FlashSectorErase((unsigned long) address) == 0)
	{
		result = true;
	}

    restoreFlashCache(mode);

	return result;
}

/**
 * Disable Flash data caching and instruction pre-fetching.
 *
 * It is necessary to disable the caching and VIMS to ensure the cache has
 * valid data while the program is executing.
 *
 * @return The VIMS state before being disabled.
 */
static uint32_t disableFlashCache(void)
{
    uint32_t mode = VIMSModeGet(VIMS_BASE);

    VIMSLineBufDisable(VIMS_BASE);

    if (mode != VIMS_MODE_DISABLED)
    {
        VIMSModeSet(VIMS_BASE, VIMS_MODE_DISABLED);

        while (VIMSModeGet(VIMS_BASE) != VIMS_MODE_DISABLED)
            ;
    }

    return mode;
}

/**
 * Restore the Flash data caching and instruction pre-fetching.
 *
 * @param [in] mode The VIMS mode returned by @ref disableFlashCache.
 */
static void restoreFlashCache(uint32_t mode)
{
    if (mode != VIMS_MODE_DISABLED)
    {
        VIMSModeSet(VIMS_BASE, mode);
    }

    VIMSLineBufEnable(VIMS_BASE);
}
