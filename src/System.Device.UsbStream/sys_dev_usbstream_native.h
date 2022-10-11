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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// moved to targets\AzureRTOS\SiliconLabs\_common\autogen\sl_usbd_class_vendor_instances.c for convenience //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// typedef enum __nfpack UsbEventType
// {
//     UsbEventType_Invalid = 0,
//     UsbEventType_DeviceConnected = 1,
//     UsbEventType_DeviceDisconnected = 2,
// } UsbEventType;

struct Library_sys_dev_usbstream_native_System_Device_Usb_DeviceConnectionEventArgs
{
    static const int FIELD___isConnected = 1;

    //--//
};

struct Library_sys_dev_usbstream_native_System_Device_Usb_UsbClient
{
    static const int FIELD_STATIC__UsbDeviceConnectionChanged = 0;

    NANOCLR_NATIVE_DECLARE(get_IsConnected___BOOLEAN);

    //--//
};

struct Library_sys_dev_usbstream_native_System_Device_Usb_UsbDeviceEvent
{
    static const int FIELD___eventType = 3;
    static const int FIELD___eventData = 4;
    static const int FIELD___interfaceIndex = 5;

    //--//
};

struct Library_sys_dev_usbstream_native_System_Device_Usb_UsbStream
{
    static const int FIELD___streamIndex = 1;
    static const int FIELD___disposed = 2;
    static const int FIELD___writeTimeout = 3;
    static const int FIELD___readTimeout = 4;

    NANOCLR_NATIVE_DECLARE(Flush___VOID);
    NANOCLR_NATIVE_DECLARE(NativeClose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeOpen___I4__STRING__STRING);
    NANOCLR_NATIVE_DECLARE(NativeWrite___VOID__SZARRAY_U1__I4__I4);
    NANOCLR_NATIVE_DECLARE(NativeRead___I4__SZARRAY_U1__I4__I4);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_UsbStream;

#endif // SYS_DEV_USBSTREAM_NATIVE_H
