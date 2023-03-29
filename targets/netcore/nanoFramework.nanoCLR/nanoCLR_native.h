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
} NANO_CLR_SETTINGS;

typedef HRESULT(__stdcall *ConfigureRuntimeCallback)();
typedef void(__stdcall *DebugPrintCallback)(const char *szText);
typedef int(__stdcall *WireTransmitCallback)(const CLR_UINT8 *data, size_t size);
typedef int(__stdcall *WireReceiveCallback)(const CLR_UINT8 *data, size_t size);

extern DebugPrintCallback gDebugPrintCallback;
extern WireTransmitCallback WireProtocolTransmitCallback;
extern WireReceiveCallback WireProtocolReceiveCallback;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The following functions are exposed in the DLL and
// meant to be called by the C# host application.
// Keep their names in sync with the managed code declaration @ nanoFramework.nanoCLR.Host\Interop\Native.cs
// and the code @ nanoCLR_native.c
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

extern "C" NANOCLRNATIVE_API void nanoCLR_WireProtocolProcess();

extern "C" NANOCLRNATIVE_API const char *nanoCLR_GetVersion();
