//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_BlockStorage_CC13x2_26x2FlashDriver.h>

// includes from SimpleLink
#include <driverlib/flash.h>
#include <driverlib/vims.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Swi.h>

// local defines
#define FLASH_ERASED_WORD        0x0FFFFFFFFU

// need these prototypes here
static uint8_t DisableCache();
static void EnableCache( uint8_t state );

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
	(void)context;
	(void)readModifyWrite;

    // uint_least16_t swiState;
    // uint_least16_t hwiState;
    uint8_t state;
    bool result = false;

    // // enter critical section
    // swiState = Swi_disable();
    // hwiState = Hwi_disable();

    state = DisableCache();

	if(FlashProgram((unsigned long*)buffer, (unsigned long) startAddress, (unsigned long) numBytes) == 0)
	{
		result = true;
	}

    EnableCache(state);
    
    // // exit critical section
    // Hwi_restore(hwiState);
    // Swi_restore(swiState);
	
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
	(void)context;

    // uint_least16_t swiState;
    // uint_least16_t hwiState;
    uint8_t state;
    bool result = false;

    // // enter critical section
    // swiState = Swi_disable();
    // hwiState = Hwi_disable();

    state = DisableCache();
    
	if(FlashSectorErase((unsigned long) address) == 0)
	{
		result = true;
	}

    EnableCache(state);

    // // exit critical section
    // Hwi_restore(hwiState);
    // Swi_restore(swiState);

	return result;
}

// Disable Flash data caching
static void EnableCache( uint8_t state )
{
    if ( state != VIMS_MODE_DISABLED )
    {
        // Enable the Cache.
        VIMSModeSet( VIMS_BASE, VIMS_MODE_ENABLED );
    }
}

// Restore the Flash data caching and instruction pre-fetching.
static uint8_t DisableCache()
{
    uint8_t state = VIMSModeGet( VIMS_BASE );
    
    // Check VIMS state
    if( state != VIMS_MODE_DISABLED )
    {
        // Invalidate cache
        VIMSModeSet( VIMS_BASE, VIMS_MODE_DISABLED );
    
        // Wait for disabling to be complete
        while( VIMSModeGet( VIMS_BASE ) != VIMS_MODE_DISABLED ); 
        
    }
    
    return state;
}
