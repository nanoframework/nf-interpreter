//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <targetHAL.h>
#include <nanoCLR_Runtime.h>

extern CLR_RT_ExecutionEngine g_CLR_RT_ExecutionEngine;

bool GetHalSystemInfo(HalSystemInfo &systemInfo)
{
#if defined(HAL_REDUCESIZE)
    // No config update.
    return FALSE;
#else

    // NFReleaseInfo:
    systemInfo.m_releaseInfo.Version.usMajor = VERSION_MAJOR;
    systemInfo.m_releaseInfo.Version.usMinor = VERSION_MINOR;
    systemInfo.m_releaseInfo.Version.usBuild = VERSION_BUILD;
    systemInfo.m_releaseInfo.Version.usRevision = VERSION_REVISION;

    size_t len = sizeof(systemInfo.m_releaseInfo.InfoString);
    hal_strncpy_s((char *)&systemInfo.m_releaseInfo.InfoString[0], len, OEMSYSTEMINFOSTRING, len - 1);

    len = sizeof(systemInfo.m_releaseInfo.PlatformName);
    hal_strncpy_s((char *)&systemInfo.m_releaseInfo.PlatformName[0], len, TARGETNAMESTRING, len - 1);

    len = sizeof(systemInfo.m_releaseInfo.TargetName);
    hal_strncpy_s((char *)&systemInfo.m_releaseInfo.TargetName[0], len, PLATFORMNAMESTRING, len - 1);

    // OEM_MODEL_SKU:
    ConfigurationManager_GetOemModelSku((char *)&systemInfo.m_OemModelInfo, sizeof(systemInfo.m_OemModelInfo));

    // OEM_SERIAL_NUMBERS:
    ConfigurationManager_GetModuleSerialNumber(
        (char *)&systemInfo.m_OemSerialNumbers.module_serial_number,
        sizeof(systemInfo.m_OemSerialNumbers.module_serial_number));

    ConfigurationManager_GetSystemSerialNumber(
        (char *)&systemInfo.m_OemSerialNumbers.system_serial_number,
        sizeof(systemInfo.m_OemSerialNumbers.system_serial_number));

    return TRUE;
#endif
}

bool Target_GetReleaseInfo(NFReleaseInfo &releaseInfo)
{
    NFReleaseInfo::Init(
        releaseInfo,
        VERSION_MAJOR,
        VERSION_MINOR,
        VERSION_BUILD,
        VERSION_REVISION,
        TARGETINFOSTRING,
        hal_strlen_s(TARGETINFOSTRING),
        TARGETNAMESTRING,
        hal_strlen_s(TARGETNAMESTRING),
        PLATFORMNAMESTRING,
        hal_strlen_s(PLATFORMNAMESTRING));
    return TRUE; // alternatively, return false if you didn't initialize the releaseInfo structure.
}

bool DebuggerIsConnected()
{
    return ((g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions & CLR_RT_ExecutionEngine::c_fDebugger_Enabled) != 0);
}

bool DebugSessionIsActive()
{
    return (
        (g_CLR_RT_ExecutionEngine.m_iDebugger_Conditions & CLR_RT_ExecutionEngine::c_fDebugger_SourceLevelDebugging) !=
        0);
}
