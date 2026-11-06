//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#pragma once

#ifdef NANOCLRNATIVE_EXPORTS
#define NANOCLRNATIVE_API __declspec(dllexport)
#else
#define NANOCLRNATIVE_API __declspec(dllimport)
#endif

typedef struct NANO_CLR_SETTINGS
{
    // this is the maximum number of context switches that execution engine thread scheduler can handle
    // higher number: more threads and timers can be handled
    unsigned short MaxContextSwitches;

    // set this to TRUE if the default behaviour is for the execution engine to wait for a debugger to be connected
    // when building is set for RTM this configuration is ignored
    BOOL WaitForDebugger;

    // set this to TRUE if a connection from a debugger is to be awaited after the execution engine terminates
    // this is required for launching a debug session in Visual Studio
    // when building is set for RTM this configuration is ignored
    BOOL EnterDebuggerLoopAfterExit;

    // set this to TRUE if execution engine is to performa GC before each allocation
    BOOL PerformGarbageCollection;

    // set this to TRUE if execution engine is to performa heap compaction after each GC run
    BOOL PerformHeapCompaction;

} NANO_CLR_SETTINGS;

typedef HRESULT(__stdcall *ConfigureRuntimeCallback)();
typedef void(__stdcall *DebugPrintCallback)(const char *szText);
typedef void(__stdcall *ProfilerMessageCallback)(const char *szText);
typedef void(__stdcall *ProfilerDataCallback)(const CLR_UINT8 *data, size_t size);
typedef int(__stdcall *WireTransmitCallback)(const CLR_UINT8 *data, size_t size);
typedef int(__stdcall *WireReceiveCallback)(const CLR_UINT8 *data, size_t size);
typedef void(__stdcall *WireDropClientCallback)();

extern DebugPrintCallback gDebugPrintCallback;
extern WireTransmitCallback WireProtocolTransmitCallback;
extern WireReceiveCallback WireProtocolReceiveCallback;
extern WireDropClientCallback WireProtocolDropClientCallback;
extern ProfilerMessageCallback g_ProfilerMessageCallback;
extern ProfilerDataCallback g_ProfilerDataCallback;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following functions are exposed in the DLL and                                                         //
// meant to be called by the C# host application.                                                             //
// Keep their names in sync with the managed code declaration @ nanoFramework.nanoCLR.Host\Interop\nanoCLR.cs //
// and the code @ nanoCLR_native.cpp                                                                          //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" NANOCLRNATIVE_API void nanoCLR_Run(NANO_CLR_SETTINGS nanoClrSettings);
extern "C" NANOCLRNATIVE_API HRESULT nanoCLR_LoadAssembly(const wchar_t *name, const CLR_UINT8 *data, size_t size);
extern "C" NANOCLRNATIVE_API HRESULT nanoCLR_LoadAssembliesSet(const CLR_UINT8 *data, size_t size);
extern "C" NANOCLRNATIVE_API HRESULT nanoCLR_Resolve();

extern "C" NANOCLRNATIVE_API void nanoCLR_SetConfigureCallback(ConfigureRuntimeCallback configureRuntimeCallback);
extern "C" NANOCLRNATIVE_API void nanoCLR_SetDebugPrintCallback(DebugPrintCallback debugPrintCallback);

extern "C" NANOCLRNATIVE_API void nanoCLR_WireProtocolOpen();
extern "C" NANOCLRNATIVE_API void nanoCLR_WireProtocolClose();

extern "C" NANOCLRNATIVE_API void nanoCLR_SetWireProtocolReceiveCallback(WireReceiveCallback receiveCallback);
extern "C" NANOCLRNATIVE_API void nanoCLR_SetWireProtocolTransmitCallback(WireTransmitCallback transmitCallback);

// Sets the callback to drop the connection of the debugger client (called after failed authentication attempts).
extern "C" NANOCLRNATIVE_API void nanoCLR_SetWireProtocolDropClientCallback(WireDropClientCallback dropClientCallback);

// Sets the state of the Wire Protocol session.
// isNetwork: the session is over the network, so it requires authentication when there is a token.
// token: the token the debugger has to present. NULL leaves it unchanged, empty means no authentication.
// Call it when a network client connects and again when it disconnects (isNetwork false), so the session is no longer
// authenticated.
extern "C" NANOCLRNATIVE_API void nanoCLR_SetWireProtocolChannelInfo(bool isNetwork, const char *token);

extern "C" NANOCLRNATIVE_API void nanoCLR_SetProfilerMessageCallback(ProfilerMessageCallback profilerMessageCallback);
extern "C" NANOCLRNATIVE_API void nanoCLR_SetProfilerDataCallback(ProfilerDataCallback profilerDataCallback);

extern "C" NANOCLRNATIVE_API void nanoCLR_WireProtocolProcess();

extern "C" NANOCLRNATIVE_API const char *nanoCLR_GetVersion();

extern "C" NANOCLRNATIVE_API uint16_t nanoCLR_GetNativeAssemblyCount();
extern "C" NANOCLRNATIVE_API bool nanoCLR_GetNativeAssemblyInformation(const CLR_UINT8 *data, size_t size);
