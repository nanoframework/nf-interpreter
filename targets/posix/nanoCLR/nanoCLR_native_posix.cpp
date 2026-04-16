//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// POSIX implementation of the nanoCLR exported API.
// Corresponds to targets/netcore/nanoFramework.nanoCLR/nanoCLR_native.cpp
// with all Windows-specific code replaced by POSIX equivalents.
//
// The functions here are exported from nanoFramework.nanoCLR.dylib / .so
// and called by the managed .NET host via P/Invoke (nanoCLR.cs).

#include "nanoCLR_native.h"
#include <nanoCLR_Application.h>
#include <nanoCLR_Hardware.h>
#include <nanoPAL_BlockStorage.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// ── Forward declarations of impls that live in CLRStartup.cpp ───────────────
HRESULT nanoCLR_LoadAssemblyImpl(const char16_t *name, const uint8_t *data, size_t size);
HRESULT nanoCLR_LoadAssembliesSetImpl(const uint8_t *data, size_t size);
HRESULT nanoCLR_ResolveImpl();
void nanoCLR_SetConfigureCallbackImpl(ConfigureRuntimeCallback cb);

// ── Callback globals ────────────────────────────────────────────────────────

DebugPrintCallback g_DebugPrintCallback = nullptr;
ProfilerMessageCallback g_ProfilerMessageCallback = nullptr;
ProfilerDataCallback g_ProfilerDataCallback = nullptr;
WireTransmitCallback g_WireProtocolTransmitCallback = nullptr;
WireReceiveCallback g_WireProtocolReceiveCallback = nullptr;

// flag requesting stopping of WP processing (written by Close, read by Process — must be atomic)
static std::atomic<bool> g_wireProtocolStopProcess{false};

// ── nanoCLR_Run ─────────────────────────────────────────────────────────────

void nanoCLR_Run(NANO_CLR_SETTINGS nanoClrSettings)
{
#if !defined(BUILD_RTM)
    CLR_Debug::Printf(
        "\r\nLoading nanoCLR v%d.%d.%d.%d\r\n...\r\n",
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_BUILD,
        VERSION_REVISION);
#endif

    nanoHAL_Initialize();

    BlockStorageList_Initialize();
    BlockStorage_AddDevices();
    BlockStorageList_InitializeDevices();

    CLR_SETTINGS clrSettings;
    memset(&clrSettings, 0, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches = nanoClrSettings.MaxContextSwitches;
    clrSettings.WaitForDebugger = nanoClrSettings.WaitForDebugger;
    clrSettings.EnterDebuggerLoopAfterExit = nanoClrSettings.EnterDebuggerLoopAfterExit;
    // PerformGarbageCollection / PerformHeapCompaction are VIRTUAL_DEVICE-only
    // fields in CLR_SETTINGS – not included in the embedded build's struct.
    // They are present in NANO_CLR_SETTINGS for ABI compatibility with the
    // managed host but are intentionally not forwarded here.

    ClrStartup(clrSettings);
}

// ── Assembly-loading API ─────────────────────────────────────────────────────

int nanoCLR_LoadAssembly(const char16_t *name, const uint8_t *data, size_t size)
{
    return (int)nanoCLR_LoadAssemblyImpl(name, data, size);
}

int nanoCLR_LoadAssembliesSet(const uint8_t *data, size_t size)
{
    return (int)nanoCLR_LoadAssembliesSetImpl(data, size);
}

int nanoCLR_Resolve()
{
    return (int)nanoCLR_ResolveImpl();
}

void nanoCLR_SetConfigureCallback(ConfigureRuntimeCallback configureRuntimeCallback)
{
    nanoCLR_SetConfigureCallbackImpl(configureRuntimeCallback);
}

// ── Debug print callback ────────────────────────────────────────────────────

void nanoCLR_SetDebugPrintCallback(DebugPrintCallback debugPrintCallback)
{
    g_DebugPrintCallback = debugPrintCallback;
}

// ── Wire protocol ───────────────────────────────────────────────────────────

void nanoCLR_SetWireProtocolReceiveCallback(WireReceiveCallback receiveCallback)
{
    g_WireProtocolReceiveCallback = receiveCallback;
}

void nanoCLR_SetWireProtocolTransmitCallback(WireTransmitCallback transmitCallback)
{
    g_WireProtocolTransmitCallback = transmitCallback;
}

void nanoCLR_WireProtocolProcess()
{
    while (!g_wireProtocolStopProcess)
    {
        WP_Message_Process();
    }
}

void nanoCLR_WireProtocolOpen()
{
    WP_Message_PrepareReception();
    g_wireProtocolStopProcess = false;
}

void nanoCLR_WireProtocolClose()
{
    g_wireProtocolStopProcess = true;
}

// ── Version ─────────────────────────────────────────────────────────────────

const char *nanoCLR_GetVersion()
{
    // The P/Invoke caller marshals the return as LPStr and calls CoTaskMemFree (= free on POSIX)
    // on the pointer after it copies the string.  We must therefore return a heap-allocated copy.
    char buffer[64];
    snprintf(
        buffer,
        sizeof(buffer),
        "%d.%d.%d.%d",
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_BUILD,
        VERSION_REVISION);
    return strdup(buffer); // caller frees via CoTaskMemFree / free()
}

// ── Native assembly information ──────────────────────────────────────────────

uint16_t nanoCLR_GetNativeAssemblyCount()
{
    return (uint16_t)g_CLR_InteropAssembliesCount;
}

bool nanoCLR_GetNativeAssemblyInformation(const uint8_t *data, size_t size)
{
    if (data == nullptr)
        return false;

    // Per-assembly layout: uint32_t checksum + 4 × uint16_t version fields + 128-byte name.
    const size_t requiredSize = g_CLR_InteropAssembliesCount * (sizeof(uint32_t) + 4 * sizeof(uint16_t) + 128);
    if (size < requiredSize)
        return false;

    memset((void *)data, 0, size);

    for (uint32_t i = 0; i < g_CLR_InteropAssembliesCount; i++)
    {
        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_checkSum, sizeof(uint32_t));
        data += sizeof(uint32_t);

        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_Version.iMajorVersion, sizeof(uint16_t));
        data += sizeof(uint16_t);

        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_Version.iMinorVersion, sizeof(uint16_t));
        data += sizeof(uint16_t);

        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_Version.iBuildNumber, sizeof(uint16_t));
        data += sizeof(uint16_t);

        memcpy(
            (void *)data,
            &g_CLR_InteropAssembliesNativeData[i]->m_Version.iRevisionNumber,
            sizeof(uint16_t));
        data += sizeof(uint16_t);

        hal_strcpy_s((char *)data, 128, g_CLR_InteropAssembliesNativeData[i]->m_szAssemblyName);
        data += 128;
    }

    return true;
}

// ── Profiler callbacks ───────────────────────────────────────────────────────

void nanoCLR_SetProfilerMessageCallback(ProfilerMessageCallback profilerMessageCallback)
{
    g_ProfilerMessageCallback = profilerMessageCallback;
}

void nanoCLR_SetProfilerDataCallback(ProfilerDataCallback profilerDataCallback)
{
    g_ProfilerDataCallback = profilerDataCallback;
}
