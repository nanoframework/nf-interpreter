//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "stdafx.h"

#include "nanoCLR_native.h"
#include <nanoCLR_Application.h>
#include <target_common.h>
#include <iostream>
#include <format>

//
// UNDONE: Feature configuration
//

#define NF_CLR_FEATURE_ALL

#if defined(NF_CLR_FEATURE_ALL)

#pragma comment(lib, "WireProtocol.lib")

#pragma comment(lib, "Debugger.lib")
#pragma comment(lib, "Diagnostics.lib")
#pragma comment(lib, "Hardware.lib")
#pragma comment(lib, "InterruptHandler.lib")
#pragma comment(lib, "Messaging.lib")
#pragma comment(lib, "RPC.lib")
#pragma comment(lib, "Serialization.lib")

#else

#pragma comment(lib, "WireProtocol.lib")

#pragma comment(lib, "Debugger_stub.lib")
#pragma comment(lib, "Diagnostics_stub.lib")
#pragma comment(lib, "Hardware_stub.lib")
#pragma comment(lib, "InterruptHandler_stub.lib")
#pragma comment(lib, "IOPort_stub.lib")
#pragma comment(lib, "Messaging_stub.lib")
#pragma comment(lib, "RPC_stub.lib")
#pragma comment(lib, "Serialization_stub.lib")

#endif

DebugPrintCallback gDebugPrintCallback = NULL;

#if defined(VIRTUAL_DEVICE)
ProfilerMessageCallback g_ProfilerMessageCallback = NULL;
ProfilerDataCallback g_ProfilerDataCallback = NULL;
#endif

WireTransmitCallback WireProtocolTransmitCallback = NULL;
WireReceiveCallback WireProtocolReceiveCallback = NULL;

// flag requesting stopping of WP processing
bool _wireProtocolStopProcess;

/////////////////////////////////////////////////////////////////////////////

// All solutions are expected to provide an implementation of this
bool Target_GetReleaseInfo(NFReleaseInfo &releaseInfo)
{
    NFReleaseInfo::Init(
        releaseInfo,
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_BUILD,
        VERSION_REVISION,
        OEMSYSTEMINFOSTRING,
        hal_strlen_s(OEMSYSTEMINFOSTRING),
        TARGETNAMESTRING,
        hal_strlen_s(TARGETNAMESTRING),
        PLATFORMNAMESTRING,
        hal_strlen_s(PLATFORMNAMESTRING));

    return true; // alternatively, return false if you didn't initialize the releaseInfo structure.
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following functions are exposed in the DLL and
// meant to be called by the C# host application.
// Keep their names in sync with the managed code declaration @ nanoFramework.nanoCLR.Host\Interop\Native.cs
// and the declarations @ nanoCLR_native.h
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void nanoCLR_Run(NANO_CLR_SETTINGS nanoClrSettings)
{

#if _DEBUG
    // only show this in debug build
    DWORD pid = GetCurrentProcessId();
    CLR_Debug::Printf("Process ID: %d\r\n", pid);
#endif

    CLR_Debug::Printf(
        "\r\nLoading nanoCLR v%d.%d.%d.%d\r\n...",
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_BUILD,
        VERSION_REVISION);

    // initialize nanoHAL
    nanoHAL_Initialize();

    // take care of block storage here as we are RAM based
    BlockStorageList_Initialize();
    BlockStorage_AddDevices();
    BlockStorageList_InitializeDevices();

    CLR_SETTINGS clrSettings;
    ZeroMemory(&clrSettings, sizeof(CLR_SETTINGS));

    clrSettings.MaxContextSwitches = nanoClrSettings.MaxContextSwitches;
    clrSettings.WaitForDebugger = nanoClrSettings.WaitForDebugger;
    clrSettings.EnterDebuggerLoopAfterExit = nanoClrSettings.EnterDebuggerLoopAfterExit;
    clrSettings.PerformGarbageCollection = nanoClrSettings.PerformGarbageCollection;
    clrSettings.PerformHeapCompaction = nanoClrSettings.PerformHeapCompaction;

    ClrStartup(clrSettings);

#if !defined(BUILD_RTM)
    CLR_Debug::Printf("Exiting.\r\n");
#endif
}

void nanoCLR_SetDebugPrintCallback(DebugPrintCallback debugPrintCallback)
{
    gDebugPrintCallback = debugPrintCallback;
}

void nanoCLR_SetWireProtocolReceiveCallback(WireReceiveCallback receiveCallback)
{
    WireProtocolReceiveCallback = receiveCallback;
}

void nanoCLR_SetWireProtocolTransmitCallback(WireTransmitCallback transmitCallback)
{
    WireProtocolTransmitCallback = transmitCallback;
}

void nanoCLR_WireProtocolProcess()
{
    while (!_wireProtocolStopProcess)
    {
        WP_Message_Process();
    }
}

void nanoCLR_WireProtocolOpen()
{
    WP_Message_PrepareReception();

    _wireProtocolStopProcess = false;
}

void nanoCLR_WireProtocolClose()
{
    // request to stop
    _wireProtocolStopProcess = true;
}

const char *nanoCLR_GetVersion()
{
    char buffer[128];

    char *pszVersion = nullptr;
    pszVersion = (char *)CoTaskMemAlloc(std::size(buffer));

    if (pszVersion != nullptr)
    {
        const auto result = std::format_to_n(
            buffer,
            std::size(buffer) - 1,
            "{}.{}.{}.{}",
            VERSION_MAJOR,
            VERSION_MINOR,
            VERSION_BUILD,
            VERSION_REVISION);
        *result.out = '\0';

        const std::string_view str{buffer, result.out};

        std::memcpy(pszVersion, buffer, result.size + 1);
    }

    return pszVersion;
}

uint16_t nanoCLR_GetNativeAssemblyCount()
{
    return g_CLR_InteropAssembliesCount;
}

bool nanoCLR_GetNativeAssemblyInformation(const CLR_UINT8 *data, size_t size)
{
    if (data == nullptr)
    {
        return false;
    }

    const size_t requiredSize = g_CLR_InteropAssembliesCount * (5 * sizeof(CLR_UINT16) + 128);
    if (size < requiredSize)
    {
        // Buffer too small
        return false;
    }

    // clear buffer memory
    memset((void *)data, 0, size);

    // fill the array
    for (uint32_t i = 0; i < g_CLR_InteropAssembliesCount; i++)
    {
        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_checkSum, sizeof(CLR_UINT32));
        data += sizeof(CLR_UINT32);

        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_Version.iMajorVersion, sizeof(CLR_UINT16));
        data += sizeof(CLR_UINT16);

        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_Version.iMinorVersion, sizeof(CLR_UINT16));
        data += sizeof(CLR_UINT16);

        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_Version.iBuildNumber, sizeof(CLR_UINT16));
        data += sizeof(CLR_UINT16);

        memcpy((void *)data, &g_CLR_InteropAssembliesNativeData[i]->m_Version.iRevisionNumber, sizeof(CLR_UINT16));
        data += sizeof(CLR_UINT16);

        hal_strcpy_s((char *)data, 128, g_CLR_InteropAssembliesNativeData[i]->m_szAssemblyName);
        data += 128;
    }

    return true; // Success
}

#if defined(VIRTUAL_DEVICE)

void nanoCLR_SetProfilerMessageCallback(ProfilerMessageCallback profilerMessageCallback)
{
    g_ProfilerMessageCallback = profilerMessageCallback;

    // set profiling conditions
    g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions |= CLR_RT_ExecutionEngine::c_fProfiling_Enabled |
                                                        CLR_RT_ExecutionEngine::c_fProfiling_Allocations |
                                                        CLR_RT_ExecutionEngine::c_fProfiling_Calls;
}

void nanoCLR_SetProfilerDataCallback(ProfilerDataCallback profilerDataCallback)
{
    g_ProfilerDataCallback = profilerDataCallback;

    // set profiling conditions
    g_CLR_RT_ExecutionEngine.m_iProfiling_Conditions |= CLR_RT_ExecutionEngine::c_fProfiling_Enabled |
                                                        CLR_RT_ExecutionEngine::c_fProfiling_Allocations |
                                                        CLR_RT_ExecutionEngine::c_fProfiling_Calls;
}

#endif
