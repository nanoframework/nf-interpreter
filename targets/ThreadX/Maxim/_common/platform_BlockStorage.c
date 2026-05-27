//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_BlockStorage.h>
#include <Target_BlockStorage_MXCFlashDriver.h>

// map here the Block Storage Interface to the MXC driver
IBlockStorageDevice MXCFlash_BlockStorageInterface =
{                          
    &MXCFlashDriver_InitializeDevice,
    &MXCFlashDriver_UninitializeDevice,
    &MXCFlashDriver_GetDeviceInfo,
    &MXCFlashDriver_Read,
    &MXCFlashDriver_Write,
    NULL,
    &MXCFlashDriver_IsBlockErased,    
    &MXCFlashDriver_EraseBlock,
    NULL,
    NULL
};
