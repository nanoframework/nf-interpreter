//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include "stdafx.h"
#include <target_common.h>
#include "platform_selector.h"
#include <iostream>

//--//

// using namespace System;
// using namespace System::Diagnostics;
// using namespace Microsoft::SPOT::Emulator;

bool HAL_Windows_IsShutdownPending()
{
    return false; // return EmulatorNative::GetEmulatorNative()->IsShuttingDown();
}

void HAL_Windows_AcquireGlobalLock()
{
    // UNDONE: FIXME: EmulatorNative::GetEmulatorNative()->DisableInterrupts();
}

void HAL_Windows_ReleaseGlobalLock()
{
    // UNDONE: FIXME: EmulatorNative::GetEmulatorNative()->EnableInterrupts();
}

bool HAL_Windows_HasGlobalLock()
{
    return false; // UNDONE: FIXME: !EmulatorNative::GetEmulatorNative()->AreInterruptsEnabled();
}

uint64_t HAL_Windows_GetPerformanceTicks()
{
    return 0; // UNDONE: FIXME: return EmulatorNative::GetEmulatorNative()->GetCurrentTicks();
}

// HAL_Configuration_Windows g_HAL_Configuration_Windows;

// unsigned int LOAD_IMAGE_CalcCRC;

// OEM_MODEL_SKU OEM_Model_SKU = { 1, 2, 3 };

HAL_SYSTEM_CONFIG HalSystemConfig = {
    {true}, // HAL_DRIVER_CONFIG_HEADER Header;

    //--//

    // unsigned int      DebuggerPorts[MAX_DEBUGGERS];
    DEBUGGER_PORT,

    DEBUG_TEXT_PORT,
    921600,
    0, // STDIO = COM2 or COM1

    {0, 0}, // { SRAM1_MEMORY_Base, SRAM1_MEMORY_Size },
    {0, 0}, // { FLASH_MEMORY_Base, FLASH_MEMORY_Size },
};

bool ConfigurationManager_GetConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex)
{
    return true;
}

bool ConfigurationManager_StoreConfigurationBlock(
    void *configurationBlock,
    DeviceConfigurationOption configuration,
    uint32_t configurationIndex,
    uint32_t blockSize,
    bool done)
{
    return true;
}

void NFReleaseInfo::Init(
    NFReleaseInfo &NFReleaseInfo,
    unsigned short int major,
    unsigned short int minor,
    unsigned short int build,
    unsigned short int revision,
    const char *info,
    size_t infoLen,
    const char *target,
    size_t targetLen,
    const char *platform,
    size_t platformLen)
{
    size_t len;

    NFVersion::Init(NFReleaseInfo.Version, major, minor, build, revision);

    // better set these to empty strings, in case there is nothing to fill in
    NFReleaseInfo.InfoString[0] = 0;
    NFReleaseInfo.TargetName[0] = 0;
    NFReleaseInfo.PlatformName[0] = 0;

    // fill each one, if it was provided
    if (NULL != info)
    {
        len = MIN(infoLen, sizeof(NFReleaseInfo.InfoString) - 1);
        memcpy(NFReleaseInfo.InfoString, info, len);
        NFReleaseInfo.InfoString[len] = 0;
    }

    if (NULL != target)
    {
        len = MIN(targetLen, sizeof(NFReleaseInfo.TargetName) - 1);
        memcpy(NFReleaseInfo.TargetName, target, len);
        NFReleaseInfo.TargetName[len] = 0;
    }

    if (NULL != platform)
    {
        len = MIN(platformLen, sizeof(NFReleaseInfo.PlatformName) - 1);
        memcpy(NFReleaseInfo.PlatformName, platform, len);
        NFReleaseInfo.PlatformName[len] = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

static ON_SOFT_REBOOT_HANDLER s_rebootHandlers[5] = {NULL, NULL, NULL, NULL, NULL};

void __cdecl HAL_AddSoftRebootHandler(ON_SOFT_REBOOT_HANDLER handler)
{
    for (int i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
    {
        if (s_rebootHandlers[i] == NULL)
        {
            s_rebootHandlers[i] = handler;
            return;
        }
        else if (s_rebootHandlers[i] == handler)
        {
            return;
        }
    }
}

bool g_fDoNotUninitializeDebuggerPort = false;

void nanoHAL_Initialize(void)
{
    HAL_CONTINUATION::InitializeList();
    HAL_COMPLETION::InitializeList();

#ifndef NANOWIN32
    FS_Initialize();
    FileSystemVolumeList::Initialize();
    FS_AddVolumes();
    FileSystemVolumeList::InitializeVolumes();
#endif

    Events_Initialize();
}

void nanoHAL_Uninitialize(bool isPoweringDown)
{
    (void)isPoweringDown;

    // UNDONE: FIXME: CPU_GPIO_Uninitialize();

    for (int i = 0; i < ARRAYSIZE(s_rebootHandlers); i++)
    {
        if (s_rebootHandlers[i] != NULL)
        {
            s_rebootHandlers[i]();
            return;
        }
    }

#ifndef NANOWIN32
    FileSystemVolumeList::UninitializeVolumes();
#endif

    Events_Uninitialize();

    HAL_CONTINUATION::Uninitialize();
    HAL_COMPLETION ::Uninitialize();
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// #if !defined(BUILD_RTM)
//  void __cdecl HARD_Breakpoint()
//{
//     if(::IsDebuggerPresent())
//     {
//         ::DebugBreak();
//     }
// }
// #endif

////////////////////////////////////////////////////////////////////////////////////////////////////

// HAL_Mutex::HAL_Mutex()
//{
//    ::InitializeCriticalSection( &m_data );
//}
//
// HAL_Mutex::~HAL_Mutex()
//{
//    ::DeleteCriticalSection( &m_data );
//}
//
// void HAL_Mutex::Lock()
//{
//    ::EnterCriticalSection( &m_data );
//}
//
// void HAL_Mutex::Unlock()
//{
//    ::LeaveCriticalSection( &m_data );
//}

////////////////////////////////////////////////////////////////////////////////////////////////////

void CPU_ChangePowerLevel(POWER_LEVEL level)
{
    switch (level)
    {
        case POWER_LEVEL__MID_POWER:
            break;

        case POWER_LEVEL__LOW_POWER:
            break;

        case POWER_LEVEL__HIGH_POWER:
        default:
            break;
    }
}

void CPU_Hibernate()
{
    int64_t start = ::HAL_Time_CurrentTime();

    while (true)
    {
        // wait on SYSTEM_EVENT_FLAG_DEBUGGER_ACTIVITY as well??
        unsigned int mask = ::Events_WaitForEvents(
            SLEEP_LEVEL__SLEEP,
            SYSTEM_EVENT_FLAG_COM_IN | SYSTEM_EVENT_HW_INTERRUPT /*| SYSTEM_EVENT_FLAG_BUTTON */,
            1000);

        if (mask)
        {
            break;
        }
    }
}

void CPU_Shutdown()
{
    ClrExit();
}

void CPU_Reset()
{
    ::ExitProcess(0);
}

bool CPU_IsSoftRebootSupported()
{
    return TRUE;
}

void CPU_SetPowerMode(PowerLevel_type powerLevel)
{
    (void)powerLevel;
}

char nanoCLR_Dat_Start[512 * 1024];
char nanoCLR_Dat_End[1];

////////////////////////////////////////////////////////////////////////////////////////////////////

void SecurityKey_Copy(unsigned char KeyCopy[], signed int BytesToCopy)
{
    memset(KeyCopy, 0, BytesToCopy);
}

void SecurityKey_LowLevelCopy(unsigned char KeyCopy[], signed int BytesToCopy)
{
    memset(KeyCopy, 0, BytesToCopy);
}

void SecurityKey_Print()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////

void HAL_Windows_FastSleep(signed __int64 ticks)
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER countStart;
    LARGE_INTEGER countEnd;

    if (ticks > 0)
    {
        ::QueryPerformanceFrequency(&frequency);

        ::QueryPerformanceCounter(&countStart);

        double ratio = (double)TIME_CONVERSION__TO_SECONDS / (double)frequency.QuadPart;

        countEnd.QuadPart = (signed __int64)(((countStart.QuadPart * ratio) + ticks) / ratio);

        while (countStart.QuadPart < countEnd.QuadPart)
        {
            ::QueryPerformanceCounter(&countStart);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma managed(push, off)

int hal_printf(const char *format, ...)
{
    va_list arg_ptr;

    va_start(arg_ptr, format);

    int chars = hal_vprintf(format, arg_ptr);

    va_end(arg_ptr);

    return chars;
}

int hal_vprintf(const char *format, va_list arg)
{
    return vprintf(format, arg);
}

int hal_fprintf(COM_HANDLE stream, const char *format, ...)
{
    va_list arg_ptr;
    int chars;

    va_start(arg_ptr, format);

    chars = hal_vfprintf(stream, format, arg_ptr);

    va_end(arg_ptr);

    return chars;
}

int hal_vfprintf(COM_HANDLE stream, const char *format, va_list arg)
{
    char buffer[512];
    int chars = 0;

    chars = hal_vsnprintf(buffer, sizeof(buffer), format, arg);

    // switch(ExtractTransport(stream))
    //{
    // default:
    DebuggerPort_Write(stream, buffer, chars); // skip null terminator
    //    break;

    // case FLASH_WRITE_TRANSPORT:
    //    _ASSERTE(FALSE);
    //}

    return chars;
}

int hal_snprintf(char *buffer, size_t len, const char *format, ...)
{
    va_list arg_ptr;
    int chars;

    va_start(arg_ptr, format);

    chars = hal_vsnprintf(buffer, len, format, arg_ptr);

    va_end(arg_ptr);

    return chars;
}

int hal_vsnprintf(char *buffer, size_t len, const char *format, va_list arg)
{
    return _vsnprintf_s(buffer, len, len - 1 /* force space for trailing zero*/, format, arg);
}

// Compares 2 ASCII strings case insensitive. Does not take locale into account.
int hal_stricmp(const char *dst, const char *src)
{
    int f = 0, l = 0;

    do
    {
        if (((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z'))
        {
            f -= 'A' - 'a';
        }
        if (((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z'))
        {
            l -= 'A' - 'a';
        }
    } while (f && (f == l));

    return (f - l);
}

#pragma managed(pop)

///////////////////////////////////////////////////////////////

size_t CPU_GetCachableAddress(size_t address)
{
    return address;
}

//--//

size_t CPU_GetUncachableAddress(size_t address)
{
    return address;
}

// CPU sleep is not currently implemented in this target
void CPU_Sleep(SLEEP_LEVEL_type level, uint64_t wakeEvents)
{
    (void)level;
    (void)wakeEvents;
};

///////////////////////////////////////////////////////////////

int nanoBooter_GetTargetInfo(TargetInfo *targetInfo)
{
    targetInfo->BooterVersion.usMajor = 0;
    targetInfo->BooterVersion.usMinor = 0;
    targetInfo->BooterVersion.usBuild = 0;
    targetInfo->BooterVersion.usRevision = 0;

    targetInfo->ClrVersion.usMajor = VERSION_MAJOR;
    targetInfo->ClrVersion.usMinor = VERSION_MINOR;
    targetInfo->ClrVersion.usBuild = VERSION_MINOR;
    targetInfo->ClrVersion.usRevision = VERSION_REVISION;

    memcpy(&targetInfo->InfoString, OEMSYSTEMINFOSTRING, ARRAYSIZE(OEMSYSTEMINFOSTRING));
    memcpy(&targetInfo->TargetName, TARGETNAMESTRING, ARRAYSIZE(TARGETNAMESTRING));
    memcpy(&targetInfo->PlatformName, PLATFORMNAMESTRING, ARRAYSIZE(PLATFORMNAMESTRING));
    memcpy(&targetInfo->PlatformInfoString, TARGETINFOSTRING, ARRAYSIZE(TARGETINFOSTRING));

    return true;
}

///////////////////////////////////////////////////////////////

void WP_Message_PrepareReception_Platform()
{
    // empty on purpose, nothing to configure
}

void RtosYield()
{
}
