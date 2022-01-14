//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_CC13xx_26xxFlashDriver.h>

// map here the Block Storage Interface to the CC13xx_26xx driver
IBlockStorageDevice CC13xx_26xxFlash_BlockStorageInterface = {
    &CC13xx_26xxFlashDriver_InitializeDevice,
    &CC13xx_26xxFlashDriver_UninitializeDevice,
    &CC13xx_26xxFlashDriver_GetDeviceInfo,
    &CC13xx_26xxFlashDriver_Read,
    &CC13xx_26xxFlashDriver_Write,
    NULL,
    &CC13xx_26xxFlashDriver_IsBlockErased,
    &CC13xx_26xxFlashDriver_EraseBlock,
    NULL,
    NULL};
