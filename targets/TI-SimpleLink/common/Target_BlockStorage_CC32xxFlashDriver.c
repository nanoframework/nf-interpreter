//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_BlockStorage_CC32xxFlashDriver.h>

bool CC32xxFlashDriver_InitializeDevice(void* context)
{
	(void)context;

    // SimpleLink driver takes care of this, so always true
	return true;
}

bool CC32xxFlashDriver_UninitializeDevice(void* context)
{
	(void)context;

    // SimpleLink driver takes care of this, so always true
	return true;
}

DeviceBlockInfo* CC32xxFlashDriver_GetDeviceInfo(void* context)
{
    MEMORY_MAPPED_NOR_BLOCK_CONFIG* config = context;
    
    return config->BlockConfig.BlockDeviceInformation;  
}

bool CC32xxFlashDriver_Read(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer)
{
	(void)context;

    // ... and always return true
    return true;
}

bool CC32xxFlashDriver_Write(void* context, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite)
{
	(void)context;
	
	return true;
}
   
bool CC32xxFlashDriver_IsBlockErased(void* context, ByteAddress blockAddress, unsigned int length)
{
	(void)context;

	return true;
}

bool CC32xxFlashDriver_EraseBlock(void* context, ByteAddress address)
{
	(void)context;

	return true;   
}
