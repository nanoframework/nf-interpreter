//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <strings.h>
#include <thread>

// HAL/PAL headers needed for nanoHAL_Initialize / HAL_CONTINUATION.
#include <nanoHAL.h>
#include <nanoPAL_Events.h>

// Forward declaration — defined in CLRStartup.cpp.
extern "C" void ClrExit();

bool HAL_Windows_IsShutdownPending()
{
    return false;
}

void HAL_Windows_AcquireGlobalLock()
{
    // TODO: This lock is currently a stub and can be inconsistent with HasGlobalLock().
    // Replace with a real host lock when shared runtime threading is wired for macOS.
}

void HAL_Windows_ReleaseGlobalLock()
{
}

bool HAL_Windows_HasGlobalLock()
{
    return false;
}

uint64_t HAL_Windows_GetPerformanceTicks()
{
    const auto now = std::chrono::steady_clock::now().time_since_epoch();
    const auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now).count();
    return static_cast<uint64_t>(nanos / 100);
}

void HAL_Windows_FastSleep(long long ticks)
{
    if (ticks <= 0)
    {
        return;
    }

    std::this_thread::sleep_for(std::chrono::nanoseconds(ticks * 100LL));
}

int hal_vprintf(const char *format, va_list arg)
{
    return vprintf(format, arg);
}

int hal_vfprintf(void *stream, const char *format, va_list arg)
{
    FILE *out = stream == nullptr ? stdout : static_cast<FILE *>(stream);
    return vfprintf(out, format, arg);
}

int hal_vsnprintf(char *buffer, size_t len, const char *format, va_list arg)
{
    return vsnprintf(buffer, len, format, arg);
}

int hal_snprintf(char *buffer, size_t len, const char *format, ...)
{
    va_list arg_ptr;
    va_start(arg_ptr, format);
    const int chars = hal_vsnprintf(buffer, len, format, arg_ptr);
    va_end(arg_ptr);
    return chars;
}

int hal_stricmp(const char *dst, const char *src)
{
    return strcasecmp(dst, src);
}

void CPU_Hibernate()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

void CPU_Shutdown()
{
    // Signal the CLR to exit cleanly via ExitPending, then wake up any
    // Events_WaitForEvents call so the Execute() loop checks the flag promptly.
    // Mirrors the win32 virtual-device behaviour (CPU_Shutdown → ClrExit).
    ClrExit();
    Events_Set(SYSTEM_EVENT_FLAG_ALL);
}

// The following functions are declared extern "C" in nanoHAL_v2.h / nanoHAL_Rtos.h / nanoHAL_Boot.h.
extern "C"
{

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    (void)powerLevel;
}

void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{
    (void)level;
    (void)wakeEvents;
    std::this_thread::yield();
}

void RtosYield()
{
    std::this_thread::yield();
}

void CPU_Reset()
{
    // On POSIX host, reset is equivalent to exit.
    std::exit(0);
}

void HAL_AssertEx()
{
    // On POSIX host: print location then abort so the crash is visible in logs.
    fputs("[POSIX] HAL_AssertEx() — assertion failed\n", stderr);
    std::abort();
}

bool LaunchProprietaryBootloader()
{
    // Not applicable on POSIX host.
    return false;
}

} // extern "C"

// nanoHAL lifecycle — not extern "C" (declared as C++ in nanoHAL.h).
void nanoHAL_Initialize()
{
    HAL_CONTINUATION::InitializeList();
    HAL_COMPLETION::InitializeList();
    Events_Initialize();
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;
    Events_Uninitialize();
    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION::Uninitialize();
}

