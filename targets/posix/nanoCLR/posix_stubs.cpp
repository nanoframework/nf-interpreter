//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// POSIX host stubs for platform-specific symbols that have no meaningful
// implementation on a development host but must be present to link.

#include <cstdint>
#include <cstring>
#include <cstdio>

#include <nanoCLR_Runtime.h>
#include <nanoHAL_ConfigurationManager.h>
#include "nanoCLR_native.h"

// ---------------------------------------------------------------------------
// CLR debug output — override the weak no-op from Diagnostics_stub.cpp so
// debug messages actually appear on stdout during development.
// When a debug-print callback has been registered by the managed host
// (via nanoCLR_SetDebugPrintCallback), forward output to it as well.
// ---------------------------------------------------------------------------

void CLR_Debug::Emit(const char *text, int len)
{
    if (text == nullptr)
        return;

    if (gDebugPrintCallback != nullptr)
    {
        if (len < 0)
        {
            gDebugPrintCallback(text);
        }
        else
        {
            // Null-terminate a temporary copy for the callback.
            char buf[512];
            size_t copyLen = (size_t)len < sizeof(buf) - 1 ? (size_t)len : sizeof(buf) - 1;
            memcpy(buf, text, copyLen);
            buf[copyLen] = '\0';
            gDebugPrintCallback(buf);
        }
        return;
    }

    if (len < 0)
        fputs(text, stdout);
    else
        fwrite(text, 1, (size_t)len, stdout);
    fflush(stdout);
}

int CLR_Debug::PrintfV(const char *format, va_list arg)
{
    char buf[512];
    int n = vsnprintf(buf, sizeof(buf), format, arg);
    CLR_Debug::Emit(buf, -1);
    return n;
}

int CLR_Debug::Printf(const char *format, ...)
{
    va_list arg;
    va_start(arg, format);
    int n = CLR_Debug::PrintfV(format, arg);
    va_end(arg);
    return n;
}

// ---------------------------------------------------------------------------
// Interop assembly table — register all native assemblies built into this dylib.
// ---------------------------------------------------------------------------

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_mscorlib;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Native;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Collections;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_Text;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Math;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Runtime_Serialization;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_ResourceManager;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_System_IO_Hashing;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Gpio;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_I2c;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Spi;
extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_IO_Ports;

const CLR_RT_NativeAssemblyData *g_CLR_InteropAssembliesNativeData[] = {
    &g_CLR_AssemblyNative_mscorlib,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Native,
    &g_CLR_AssemblyNative_nanoFramework_System_Collections,
    &g_CLR_AssemblyNative_nanoFramework_System_Text,
    &g_CLR_AssemblyNative_System_Math,
    &g_CLR_AssemblyNative_System_Runtime_Serialization,
    &g_CLR_AssemblyNative_nanoFramework_ResourceManager,
    &g_CLR_AssemblyNative_nanoFramework_System_IO_Hashing,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Events,
    &g_CLR_AssemblyNative_nanoFramework_Runtime_Events_EventSink_DriverProcs,
    &g_CLR_AssemblyNative_System_Device_Gpio,
    &g_CLR_AssemblyNative_System_Device_I2c,
    &g_CLR_AssemblyNative_System_Device_Spi,
    &g_CLR_AssemblyNative_System_IO_Ports,
    nullptr,
};
const uint16_t g_CLR_InteropAssembliesCount = ARRAYSIZE(g_CLR_InteropAssembliesNativeData) - 1; // exclude nullptr sentinel

// ---------------------------------------------------------------------------
// Configuration Manager stubs.
// ---------------------------------------------------------------------------

extern "C" void ConfigurationManager_GetOemModelSku(char *model, size_t modelSkuSize)
{
    if (model && modelSkuSize > 0)
        hal_strncpy_s(model, modelSkuSize, "POSIX", modelSkuSize - 1);
}

extern "C" void ConfigurationManager_GetModuleSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    if (serialNumber && serialNumberSize > 0)
        hal_strncpy_s(serialNumber, serialNumberSize, "0000000000000000", serialNumberSize - 1);
}

extern "C" void ConfigurationManager_GetSystemSerialNumber(char *serialNumber, size_t serialNumberSize)
{
    if (serialNumber && serialNumberSize > 0)
        hal_strncpy_s(serialNumber, serialNumberSize, "0000000000000000", serialNumberSize - 1);
}

// ---------------------------------------------------------------------------
// Wire Protocol stubs – the POSIX shared library exposes the WireProtocol API
// but does not connect a real transport.  These stubs satisfy the link.
// ---------------------------------------------------------------------------

extern "C" void WP_Message_PrepareReception()
{
    // No-op: no wire protocol transport on POSIX host.
}

extern "C" void WP_Message_Process()
{
    // No-op: no wire protocol transport on POSIX host.
}
