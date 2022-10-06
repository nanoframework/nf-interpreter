//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SYS_DEV_USBSTREAM_NATIVE_H
#define SYS_DEV_USBSTREAM_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

struct Library_sys_dev_usbstream_native_System_Device_UsbClient_UsbStream
{
    static const int FIELD___streamIndex = 2;
    static const int FIELD___disposed = 3;

    NANOCLR_NATIVE_DECLARE(Flush___VOID);
    NANOCLR_NATIVE_DECLARE(NativeClose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeOpen___I4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(NativeWrite___VOID__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeRead___I4__SZARRAY_U1__I4__I4);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_UsbStream;

#endif // SYS_DEV_USBSTREAM_NATIVE_H
