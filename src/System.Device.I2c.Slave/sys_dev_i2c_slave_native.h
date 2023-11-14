//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_I2C_SLAVE_NATIVE_H
#define SYS_DEV_I2C_SLAVE_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

struct Library_sys_dev_i2c_slave_native_System_Device_I2c_I2cSlaveDevice
{
    static const int FIELD___busId = 1;
    static const int FIELD___deviceAddress = 2;
    static const int FIELD___disposed = 3;
    static const int FIELD___syncLock = 4;
    static const int FIELD___buffer = 5;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeTransmit___I4__SystemSpanByte__SystemSpanByte__I4);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c_Slave;

#endif // SYS_DEV_I2C_SLAVE_NATIVE_H
