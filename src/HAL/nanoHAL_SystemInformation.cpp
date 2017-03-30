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

    // MfReleaseInfo:
    systemInfo.m_releaseInfo.version.usMajor       = VERSION_MAJOR;
    systemInfo.m_releaseInfo.version.usMinor       = VERSION_MINOR;
    systemInfo.m_releaseInfo.version.usBuild       = VERSION_BUILD;
    systemInfo.m_releaseInfo.version.usRevision    = VERSION_REVISION;
    const size_t len = sizeof(systemInfo.m_releaseInfo.infoString);

    hal_strncpy_s ((char*)&systemInfo.m_releaseInfo.infoString[0], len, OEMSYSTEMINFOSTRING, len-1 );

    // we are not supporting this at this time
    // OEM_MODEL_SKU:
    //memcpy((void*)&(systemInfo.m_OemModelInfo), (void*)&(g_ConfigurationSector.OEM_Model_SKU), sizeof(OEM_MODEL_SKU));

    // we are not supporting this at this time
    // OEM_SERIAL_NUMBERS:
    //memcpy((void*)&(systemInfo.m_OemSerialNumbers), (void*)&(g_ConfigurationSector.OemSerialNumbers), sizeof(OEM_SERIAL_NUMBERS));

    return TRUE;
#endif
}
