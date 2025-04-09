//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_SL_MscFlashDriver.h>

// map here the Block Storage Interface to the SL_Msc driver
IBlockStorageDevice SL_MscFlash_BlockStorageInterface =
{                          
    &SL_MscFlashDriver_InitializeDevice,
    &SL_MscFlashDriver_UninitializeDevice,
    &SL_MscFlashDriver_GetDeviceInfo,
    &SL_MscFlashDriver_Read,
    &SL_MscFlashDriver_Write,
    NULL,
    &SL_MscFlashDriver_IsBlockErased,    
    &SL_MscFlashDriver_EraseBlock,
    NULL,
    NULL
};
