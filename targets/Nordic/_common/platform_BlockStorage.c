//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_ZephyrFlashDriver.h>

// map here the Block Storage Interface to the Zephyr driver
IBlockStorageDevice ZephyrFlash_BlockStorageInterface = {
    &ZephyrFlashDriver_InitializeDevice,
    &ZephyrFlashDriver_UninitializeDevice,
    &ZephyrFlashDriver_GetDeviceInfo,
    &ZephyrFlashDriver_Read,
    &ZephyrFlashDriver_Write,
    NULL,
    &ZephyrFlashDriver_IsBlockErased,
    &ZephyrFlashDriver_EraseBlock,
    NULL,
    NULL};
