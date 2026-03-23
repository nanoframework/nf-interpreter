//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_MXCFLASH_DRIVER_H_
#define _TARGET_MXCFLASH_DRIVER_H_

#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C" {
#endif

bool MXCFlashDriver_InitializeDevice(void*);
bool MXCFlashDriver_UninitializeDevice(void*);
DeviceBlockInfo* MXCFlashDriver_GetDeviceInfo(void*);
bool MXCFlashDriver_Read(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer);
bool MXCFlashDriver_Write(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite);
bool MXCFlashDriver_IsBlockErased(void*, ByteAddress blockAddress, unsigned int length);
bool MXCFlashDriver_EraseBlock(void*, ByteAddress address);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_MXCFLASH_DRIVER_H_
