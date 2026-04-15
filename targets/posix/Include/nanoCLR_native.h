//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

// POSIX-compatible replacement for the Windows nanoCLR_native.h.
// Uses standard C types instead of Windows-specific BOOL / HRESULT / __declspec.
// The exported symbols use GCC/Clang visibility attributes.

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#ifdef __cplusplus
extern "C"
{
#endif

// ---------------------------------------------------------------------------
// Visibility macro
// ---------------------------------------------------------------------------
#if defined(__GNUC__) || defined(__clang__)
#define NANOCLRNATIVE_API __attribute__((visibility("default")))
#else
#define NANOCLRNATIVE_API
#endif

    // ---------------------------------------------------------------------------
    // Settings struct  (must be ABI-compatible with the managed NANO_CLR_SETTINGS)
    // ---------------------------------------------------------------------------
    typedef struct NANO_CLR_SETTINGS
    {
        unsigned short MaxContextSwitches;
        bool WaitForDebugger;
        bool EnterDebuggerLoopAfterExit;
        bool PerformGarbageCollection;
        bool PerformHeapCompaction;
    } NANO_CLR_SETTINGS;

    // ---------------------------------------------------------------------------
    // Callback typedefs
    // The calling convention on POSIX for cdecl is the default, so no extra attrs.
    // ---------------------------------------------------------------------------
    typedef int (*ConfigureRuntimeCallback)();
    typedef void (*DebugPrintCallback)(const char *szText);
    typedef void (*ProfilerMessageCallback)(const char *szText);
    typedef void (*ProfilerDataCallback)(const uint8_t *data, size_t size);
    typedef int (*WireTransmitCallback)(const uint8_t *data, size_t size);
    typedef int (*WireReceiveCallback)(const uint8_t *data, size_t size);

    // ---------------------------------------------------------------------------
    // Globals (defined in nanoCLR_native_posix.cpp / CLRStartup.cpp)
    // ---------------------------------------------------------------------------
    extern DebugPrintCallback gDebugPrintCallback;
    extern WireTransmitCallback WireProtocolTransmitCallback;
    extern WireReceiveCallback WireProtocolReceiveCallback;
    extern ProfilerMessageCallback g_ProfilerMessageCallback;
    extern ProfilerDataCallback g_ProfilerDataCallback;

    // ---------------------------------------------------------------------------
    // Exported API  (keep names in sync with nanoCLR.cs P/Invoke declarations)
    // ---------------------------------------------------------------------------

    NANOCLRNATIVE_API void nanoCLR_Run(NANO_CLR_SETTINGS nanoClrSettings);

    // name is passed as UTF-16 LE from the managed host (LPWStr marshal).
    // On POSIX wchar_t is 4 bytes; we receive char16_t data – the name is used
    // only as an opaque map key, so the mismatch is harmless in practice.
    NANOCLRNATIVE_API int nanoCLR_LoadAssembly(const wchar_t *name, const uint8_t *data, size_t size);
    NANOCLRNATIVE_API int nanoCLR_LoadAssembliesSet(const uint8_t *data, size_t size);
    NANOCLRNATIVE_API int nanoCLR_Resolve();

    NANOCLRNATIVE_API void nanoCLR_SetConfigureCallback(ConfigureRuntimeCallback configureRuntimeCallback);
    NANOCLRNATIVE_API void nanoCLR_SetDebugPrintCallback(DebugPrintCallback debugPrintCallback);

    NANOCLRNATIVE_API void nanoCLR_WireProtocolOpen();
    NANOCLRNATIVE_API void nanoCLR_WireProtocolClose();

    NANOCLRNATIVE_API void nanoCLR_SetWireProtocolReceiveCallback(WireReceiveCallback receiveCallback);
    NANOCLRNATIVE_API void nanoCLR_SetWireProtocolTransmitCallback(WireTransmitCallback transmitCallback);

    NANOCLRNATIVE_API void nanoCLR_SetProfilerMessageCallback(ProfilerMessageCallback profilerMessageCallback);
    NANOCLRNATIVE_API void nanoCLR_SetProfilerDataCallback(ProfilerDataCallback profilerDataCallback);

    NANOCLRNATIVE_API void nanoCLR_WireProtocolProcess();

    NANOCLRNATIVE_API const char *nanoCLR_GetVersion();

    NANOCLRNATIVE_API uint16_t nanoCLR_GetNativeAssemblyCount();
    NANOCLRNATIVE_API bool nanoCLR_GetNativeAssemblyInformation(const uint8_t *data, size_t size);

#ifdef __cplusplus
}
#endif
