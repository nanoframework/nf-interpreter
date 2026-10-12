//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_usbstream_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::NativeClose___VOID,
    Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::NativeOpen___I4__STRING__STRING,
    Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::Read___I4__SZARRAY_U1__I4__I4,
    Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::Write___VOID__SZARRAY_U1__I4__I4,
    Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream::get_IsConnected___BOOLEAN,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_UsbStream =
{
    "System.Device.UsbStream",
    0x04336DF1,
    method_lookup,
    ARRAYSIZE(method_lookup)
};

// clang-format on
