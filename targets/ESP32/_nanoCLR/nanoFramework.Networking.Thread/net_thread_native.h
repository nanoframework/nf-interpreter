//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NET_THREAD_NATIVE_H
#define NET_THREAD_NATIVE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack OpenThreadDataset_componentInUseFlags
{
    OpenThreadDataset_componentInUseFlags_None = 0,
    OpenThreadDataset_componentInUseFlags_channel = 1,
    OpenThreadDataset_componentInUseFlags_panId = 2,
    OpenThreadDataset_componentInUseFlags_networkName = 4,
    OpenThreadDataset_componentInUseFlags_extendedPanId = 8,
    OpenThreadDataset_componentInUseFlags_networkKey = 16,
    OpenThreadDataset_componentInUseFlags_pskc = 32,
} OpenThreadDataset_componentInUseFlags;

typedef enum __nfpack OpenThreadEventType
{
    OpenThreadEventType_StateChanged = 0,
    OpenThreadEventType_RoleChanged = 1,
    OpenThreadEventType_CommandOutputAvailable = 2,
    OpenThreadEventType_JoinerComplete = 3,
} OpenThreadEventType;

typedef enum __nfpack RadioConnection
{
    RadioConnection_Native = 0,
    RadioConnection_Uart = 1,
    RadioConnection_Spi = 2,
} RadioConnection;

typedef enum __nfpack ThreadDeviceRole
{
    ThreadDeviceRole_Disabled = 0,
    ThreadDeviceRole_Detached = 1,
    ThreadDeviceRole_Child = 2,
    ThreadDeviceRole_Router = 3,
    ThreadDeviceRole_Leader = 4,
} ThreadDeviceRole;

typedef enum __nfpack ThreadDeviceState
{
    ThreadDeviceState_Start = 0,
    ThreadDeviceState_Stop = 1,
    ThreadDeviceState_Detached = 2,
    ThreadDeviceState_Attached = 3,
    ThreadDeviceState_InterfaceUp = 5,
    ThreadDeviceState_InterfaceDown = 6,
    ThreadDeviceState_GotIpv6 = 7,
} ThreadDeviceState;

typedef enum __nfpack ThreadDeviceType
{
    ThreadDeviceType_EndDevice = 0,
    ThreadDeviceType_SleepyEndDevice = 1,
    ThreadDeviceType_Router = 2,
} ThreadDeviceType;

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadStateChangeEventArgs
{
    static const int FIELD___currentState = 3;

    //--//
};

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadRoleChangeEventArgs
{
    static const int FIELD___previousRole = 3;
    static const int FIELD___currentRole = 4;

    //--//
};

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadConsoleOutputAvailableArgs
{
    static const int FIELD___consoleLines = 3;

    //--//
};

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread
{
    static const int FIELD_STATIC___openThreadEventManager = 0;

    static const int FIELD___disposedValue = 1;
    static const int FIELD___currentDeviceRole = 2;
    static const int FIELD___dataset = 3;
    static const int FIELD___started = 4;
    static const int FIELD___threadDeviceType = 5;
    static const int FIELD___radioType = 6;
    static const int FIELD___port = 7;
    static const int FIELD___speed = 8;
    static const int FIELD___consoleOutputAvailable = 9;
    static const int FIELD___joinerResultAvailable = 10;
    static const int FIELD___openThreadErrorCode = 11;
    static const int FIELD___consoleCommandResult = 12;
    static const int FIELD___commandInProgress = 13;
    static const int FIELD__OnStatusChanged = 14;
    static const int FIELD__OnRoleChanged = 15;
    static const int FIELD__OnConsoleOutputAvailable = 16;

    NANOCLR_NATIVE_DECLARE(NativeCreateStack___VOID);
    NANOCLR_NATIVE_DECLARE(NativeSetActiveDataset___VOID);
    NANOCLR_NATIVE_DECLARE(NativeGetActiveDataset___VOID);
    NANOCLR_NATIVE_DECLARE(NativeJoinerStart___VOID__STRING);
    NANOCLR_NATIVE_DECLARE(NativeStartThread___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStopThread___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);
    NANOCLR_NATIVE_DECLARE(NativeSendConsoleInput___VOID__STRING__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeGetConsoleOutput___SZARRAY_STRING);
    NANOCLR_NATIVE_DECLARE(NativeGetMeshLocalAddress___SZARRAY_U1);

    //--//
};

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadDataset
{
    static const int FIELD___channel = 1;
    static const int FIELD___panId = 2;
    static const int FIELD___networkName = 3;
    static const int FIELD___extendedPanId = 4;
    static const int FIELD___networkKey = 5;
    static const int FIELD___pskc = 6;
    static const int FIELD___inUseFlags = 7;

    //--//
};

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadJoinerStartCompleteEventArgs
{
    static const int FIELD___error = 3;

    //--//
};

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadEventListener
{
    static const int FIELD___openThreadObject = 1;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Thread;

#endif // NET_THREAD_NATIVE_H
