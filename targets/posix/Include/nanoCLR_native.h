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
    //
    // The managed host (nanoFramework.nanoCLR.Host.dll) marshals C# bool as
    // 4-byte BOOL (UnmanagedType.Bool) by default.  The Windows nanoCLR_native.h
    // uses BOOL (= int) for these fields, so we must use int32_t here to match
    // the managed struct layout exactly.  Using C++ bool (1 byte) causes an ABI
    // mismatch: the managed marshaler produces a 20-byte struct (passed by
    // hidden pointer on x86-64), while the native side expects 6 bytes (passed
    // in a register), leading to the struct contents being misinterpreted.
    // ---------------------------------------------------------------------------
    typedef struct NANO_CLR_SETTINGS
    {
        unsigned short MaxContextSwitches;
        int32_t WaitForDebugger;
        int32_t EnterDebuggerLoopAfterExit;
        int32_t PerformGarbageCollection;
        int32_t PerformHeapCompaction;
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
    extern DebugPrintCallback g_DebugPrintCallback;
    extern WireTransmitCallback g_WireProtocolTransmitCallback;
    extern WireReceiveCallback g_WireProtocolReceiveCallback;
    extern ProfilerMessageCallback g_ProfilerMessageCallback;
    extern ProfilerDataCallback g_ProfilerDataCallback;

    // ---------------------------------------------------------------------------
    // Exported API  (keep names in sync with nanoCLR.cs P/Invoke declarations)
    // ---------------------------------------------------------------------------

    NANOCLRNATIVE_API void nanoCLR_Run(NANO_CLR_SETTINGS nanoClrSettings);

    // name is passed as UTF-16 LE (2-byte chars) from the managed host via CharSet.Unicode.
    // char16_t is guaranteed to be 2 bytes on all platforms, matching what the C# marshaller sends.
    NANOCLRNATIVE_API int nanoCLR_LoadAssembly(const char16_t *name, const uint8_t *data, size_t size);
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
