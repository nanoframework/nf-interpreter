//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _NF_DEVICES_CAN_NATIVE_H_
#define _NF_DEVICES_CAN_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>

////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Devices.Can.CanEvent (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////

enum CanEvent
{
    CanEvent_MessageReceived = 0,
    CanEvent_ErrorOccurred,
};

////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Devices.Can.CanMessageIdType (in managed code) !!! //
////////////////////////////////////////////////////////////////////////////////////////////

enum CanMessageIdType
{
    CanMessageIdType_SID = 0,
    CanMessageIdType_EID,
};

///////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Devices.Can.CanMessageFrameType (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////////////

enum CanMessageFrameType
{
    CanMessageFrameType_Data = 0,
    CanMessageFrameType_RemoteRequest,
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanSettings
{
    static const int FIELD___baudRatePrescaler = 1;
    static const int FIELD___phaseSegment1 = 2;
    static const int FIELD___phaseSegment2 = 3;
    static const int FIELD___syncJumpWidth = 4;

    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanController
{
    static const int FIELD_STATIC__s_eventListener = 0;

    static const int FIELD___disposed = 1;
    static const int FIELD___callbacks = 2;
    static const int FIELD___syncLock = 3;
    static const int FIELD___controllerId = 4;
    static const int FIELD___settings = 5;
    static const int FIELD___message = 6;
    static const int FIELD__ControllerId = 7;

    NANOCLR_NATIVE_DECLARE(WriteMessage___VOID__nanoFrameworkDevicesCanCanMessage);
    NANOCLR_NATIVE_DECLARE(GetMessage___nanoFrameworkDevicesCanCanMessage);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeUpdateCallbacks___VOID);
    NANOCLR_NATIVE_DECLARE(GetDeviceSelector___STATIC__STRING);

    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanControllerEventListener
{
    static const int FIELD___canControllersMap = 1;

    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanControllerManager
{
    static const int FIELD_STATIC___syncLock = 1;
    static const int FIELD_STATIC__s_controllersCollection = 2;

    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessage
{
    static const int FIELD___id = 1;
    static const int FIELD___identifierType = 2;
    static const int FIELD___frameType = 3;
    static const int FIELD___message = 4;

    //--//

};

struct Library_nf_devices_can_native_nanoFramework_Devices_Can_CanMessageEvent
{
    static const int FIELD__ControllerIndex = 1;
    static const int FIELD__Event = 2;

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Devices_Can;

#endif  //_NF_DEVICES_CAN_NATIVE_H_
