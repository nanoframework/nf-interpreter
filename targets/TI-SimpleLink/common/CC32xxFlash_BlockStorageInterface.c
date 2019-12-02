//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_CC32xxFlashDriver.h>

// map here the Block Storage Interface to the CC32xx driver
IBlockStorageDevice CC32xxFlash_BlockStorageInterface =
{
    &CC32xxFlashDriver_InitializeDevice,
    &CC32xxFlashDriver_UninitializeDevice,
    &CC32xxFlashDriver_GetDeviceInfo,
    &CC32xxFlashDriver_Read,
    &CC32xxFlashDriver_Write,
    NULL,
    &CC32xxFlashDriver_IsBlockErased,
    &CC32xxFlashDriver_EraseBlock,
    NULL,
    NULL
};
