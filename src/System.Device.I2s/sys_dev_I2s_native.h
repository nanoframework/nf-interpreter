//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SYS_DEV_I2S_NATIVE_H_
#define _SYS_DEV_I2S_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

struct Library_sys_dev_I2s_native_System_Device_I2s_I2sDevice
{
    static const int FIELD___connectionSettings = 1;
    static const int FIELD___disposed = 2;

    NANOCLR_NATIVE_DECLARE(Read___VOID__SystemSpanByte);
    NANOCLR_NATIVE_DECLARE(Read___VOID__SZARRAY_U2);
    NANOCLR_NATIVE_DECLARE(Write___VOID__SZARRAY_U2);
    NANOCLR_NATIVE_DECLARE(Write___VOID__SystemSpanByte);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2s;

#endif  //_SYS_DEV_I2S_NATIVE_H_
