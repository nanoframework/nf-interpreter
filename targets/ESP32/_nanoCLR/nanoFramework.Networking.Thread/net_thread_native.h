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

typedef enum __nfpack ThreadDeviceType
{
    ThreadDeviceType_EndDevice = 0,
    ThreadDeviceType_SleepyEndDevice = 1,
    ThreadDeviceType_Router = 2,
} ThreadDeviceType;

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadStateChangeEventArgs
{
    static const int FIELD__currentState = 3;

    //--//
};

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadRoleChangeEventArgs
{
    static const int FIELD__previousRole = 3;
    static const int FIELD__currentRole = 4;

    //--//
};

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread
{
    static const int FIELD_STATIC___openThreadEventManager = 0;

    static const int FIELD___currentDeviceRole = 1;
    static const int FIELD___dataset = 2;
    static const int FIELD___started = 3;
    static const int FIELD___radioType = 4;
    static const int FIELD___disposedValue = 5;
    static const int FIELD___threadDeviceType = 6;
    static const int FIELD___port = 7;
    static const int FIELD__OnStatusChanged = 8;
    static const int FIELD__OnRoleChanged = 9;

    NANOCLR_NATIVE_DECLARE(NativeCreateStack___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);

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

struct Library_net_thread_native_nanoFramework_Networking_Thread_OpenThreadEventListener
{
    static const int FIELD___openThreadObject = 1;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Thread;

#endif // NET_THREAD_NATIVE_H
