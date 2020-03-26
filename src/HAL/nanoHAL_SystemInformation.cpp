//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include <targetHAL.h>

bool GetHalSystemInfo(HalSystemInfo& systemInfo)
{
#if defined(HAL_REDUCESIZE)
    // No config update.
    return FALSE;
#else

    // NFReleaseInfo:
    systemInfo.m_releaseInfo.Version.usMajor       = VERSION_MAJOR;
    systemInfo.m_releaseInfo.Version.usMinor       = VERSION_MINOR;
    systemInfo.m_releaseInfo.Version.usBuild       = VERSION_BUILD;
    systemInfo.m_releaseInfo.Version.usRevision    = VERSION_REVISION;
    
    size_t len = sizeof(systemInfo.m_releaseInfo.InfoString);
    hal_strncpy_s ((char*)&systemInfo.m_releaseInfo.InfoString[0], len, OEMSYSTEMINFOSTRING, len-1 );
    
    len = sizeof(systemInfo.m_releaseInfo.PlatformName);
    hal_strncpy_s ((char*)&systemInfo.m_releaseInfo.PlatformName[0], len, TARGETNAMESTRING, len-1 );

    len = sizeof(systemInfo.m_releaseInfo.TargetName);
    hal_strncpy_s ((char*)&systemInfo.m_releaseInfo.TargetName[0], len, PLATFORMNAMESTRING, len-1 );

    // we are not supporting this at this time
    // OEM_MODEL_SKU:
    //memcpy((void*)&(systemInfo.m_OemModelInfo), (void*)&(g_ConfigurationSector.OEM_Model_SKU), sizeof(OEM_MODEL_SKU));

    // we are not supporting this at this time
    // OEM_SERIAL_NUMBERS:
    //memcpy((void*)&(systemInfo.m_OemSerialNumbers), (void*)&(g_ConfigurationSector.OemSerialNumbers), sizeof(OEM_SERIAL_NUMBERS));

    return TRUE;
#endif
}

bool Target_GetReleaseInfo(NFReleaseInfo& releaseInfo)
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
        hal_strlen_s(PLATFORMNAMESTRING)
        );
    return TRUE; // alternatively, return false if you didn't initialize the releaseInfo structure.
}
