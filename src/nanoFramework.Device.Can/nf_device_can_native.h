//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _NF_DEVICE_CAN_NATIVE_H_
#define _NF_DEVICE_CAN_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack CanEvent
{
    CanEvent_MessageReceived = 0,
    CanEvent_ErrorOccurred = 1,
} CanEvent;

typedef enum __nfpack CanMessageFrameType
{
    CanMessageFrameType_Data = 0,
    CanMessageFrameType_RemoteRequest = 1,
} CanMessageFrameType;

typedef enum __nfpack CanMessageIdType
{
    CanMessageIdType_SID = 0,
    CanMessageIdType_EID = 1,
} CanMessageIdType;

struct Library_nf_device_can_native_nanoFramework_Device_Can_CanSettings
{
    static const int FIELD___baudRatePrescaler = 1;
    static const int FIELD___phaseSegment1 = 2;
    static const int FIELD___phaseSegment2 = 3;
    static const int FIELD___syncJumpWidth = 4;

    //--//
};

struct Library_nf_device_can_native_nanoFramework_Device_Can_CanController
{
    static const int FIELD_STATIC__s_eventListener = 0;

    static const int FIELD___disposed = 1;
    static const int FIELD___callbacks = 2;
    static const int FIELD___syncLock = 3;
    static const int FIELD___controllerId = 4;
    static const int FIELD___settings = 5;
    static const int FIELD___message = 6;
    static const int FIELD__ControllerId = 7;

    NANOCLR_NATIVE_DECLARE(WriteMessage___VOID__nanoFrameworkDeviceCanCanMessage);
    NANOCLR_NATIVE_DECLARE(GetMessage___nanoFrameworkDeviceCanCanMessage);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeUpdateCallbacks___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//
};

struct Library_nf_device_can_native_nanoFramework_Device_Can_CanControllerEventListener
{
    static const int FIELD___canControllersMap = 1;

    //--//
};

struct Library_nf_device_can_native_nanoFramework_Device_Can_CanControllerManager
{
    static const int FIELD_STATIC___syncLock = 1;
    static const int FIELD_STATIC__s_controllersCollection = 2;

    //--//
};

struct Library_nf_device_can_native_nanoFramework_Device_Can_CanMessage
{
    static const int FIELD___id = 1;
    static const int FIELD___identifierType = 2;
    static const int FIELD___frameType = 3;
    static const int FIELD___message = 4;

    //--//
};

struct Library_nf_device_can_native_nanoFramework_Device_Can_CanMessageEvent
{
    static const int FIELD__ControllerIndex = 3;
    static const int FIELD__Event = 4;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_Can;

#endif //_NF_DEVICE_CAN_NATIVE_H_
