//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_I2C_NATIVE_H
#define SYS_DEV_I2C_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack I2cBusSpeed
{
    I2cBusSpeed_StandardMode = 0,
    I2cBusSpeed_FastMode = 1,
} I2cBusSpeed;

typedef enum __nfpack I2cTransferStatus
{
    I2cTransferStatus_UnknownError = 0,
    I2cTransferStatus_FullTransfer = 1,
    I2cTransferStatus_ClockStretchTimeout = 2,
    I2cTransferStatus_PartialTransfer = 3,
    I2cTransferStatus_SlaveAddressNotAcknowledged = 4,
} I2cTransferStatus;

struct Library_sys_dev_i2c_native_System_Device_I2c_I2cConnectionSettings
{
    static const int FIELD___deviceAddress = 1;
    static const int FIELD___busSpeed = 2;
    static const int FIELD___busId = 3;

    //--//
};

struct Library_sys_dev_i2c_native_System_Device_I2c_I2cTransferResult
{
    static const int FIELD___bytesTransferred = 1;
    static const int FIELD___status = 2;

    //--//
};

struct Library_sys_dev_i2c_native_System_Device_I2c_I2cDevice
{
    static const int FIELD___syncLock = 1;
    static const int FIELD___connectionSettings = 2;
    static const int FIELD___disposed = 3;
    static const int FIELD___buffer = 4;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransmit___SystemDeviceI2cI2cTransferResult__SystemSpanByte__SystemSpanByte);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c;

#endif //SYS_DEV_I2C_NATIVE_H
