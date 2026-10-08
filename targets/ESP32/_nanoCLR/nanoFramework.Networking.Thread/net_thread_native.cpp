//
// Copyright (c) 2020 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "net_thread_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeCreateStack___VOID,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeDispose___VOID,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeGetActiveDataset___VOID,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeGetConsoleOutput___SZARRAY_STRING,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeGetMeshLocalAddress___SZARRAY_U1,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeJoinerStart___VOID__STRING,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeSendConsoleInput___VOID__STRING__BOOLEAN,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeSetActiveDataset___VOID,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeStartThread___VOID,
    Library_net_thread_native_nanoFramework_Networking_Thread_OpenThread::NativeStopThread___VOID,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Thread =
{
    "nanoFramework.Networking.Thread",
    0x7D05D25E,
    method_lookup,
    ARRAYSIZE(method_lookup)
};

// clang-format on
