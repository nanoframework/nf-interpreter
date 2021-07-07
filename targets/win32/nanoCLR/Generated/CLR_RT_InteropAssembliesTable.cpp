//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_mscorlib;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Math;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Collections;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Text;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Native;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_ResourceManager;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Storage;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Networking_Sntp;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Net;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_Ports;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Gpio;

// nanoFramework class libraries declarations
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs;

const CLR_RT_NativeAssemblyData *g_CLR_InteropAssembliesNativeData[] = {
    &g_CLR_AssemblyNative_mscorlib,
    &g_CLR_AssemblyNative_System_Math,
    &g_CLR_AssemblyNative_nanoFramework_System_Collections,
    &g_CLR_AssemblyNative_nanoFramework_System_Text,
    &g_CLR_AssemblyNative_nanoFramework_ResourceManager,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Native,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Events,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs,
    &g_CLR_AssemblyNative_Windows_Storage,
    &g_CLR_AssemblyNative_nanoFramework_Networking_Sntp,
    &g_CLR_AssemblyNative_System_Net,
    &g_CLR_AssemblyNative_System_IO_Ports,
    &g_CLR_AssemblyNative_System_Device_Gpio,
    NULL};

uint16_t g_CLR_InteropAssembliesCount = ARRAYSIZE(g_CLR_InteropAssembliesNativeData) - 1;
