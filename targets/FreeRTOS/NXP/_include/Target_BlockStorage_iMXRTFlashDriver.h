//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _TARGET_IMXRTFLASH_DRIVER_H_
#define _TARGET_IMXRTFLASH_DRIVER_H_ 1

#include <nanoPAL_BlockStorage.h>

#ifdef __cplusplus
extern "C" {
#endif

bool iMXRTFlexSPIDriver_InitializeDevice(void*);
bool iMXRTFlexSPIDriver_UninitializeDevice(void*);
DeviceBlockInfo* iMXRTFlexSPIDriver_GetDeviceInfo(void*);
bool iMXRTFlexSPIDriver_Read(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer);
bool iMXRTFlexSPIDriver_Write(void*, ByteAddress startAddress, unsigned int numBytes, unsigned char* buffer, bool readModifyWrite);
bool iMXRTFlexSPIDriver_IsBlockErased(void*, ByteAddress blockAddress, unsigned int length);
bool iMXRTFlexSPIDriver_EraseBlock(void*, ByteAddress address);
void iMXRTFlexSPIDriver_SetPowerState(void*, unsigned int state);

#ifdef __cplusplus
}
#endif

#endif  //_TARGET_IMXRTFLASH_DRIVER_H_
