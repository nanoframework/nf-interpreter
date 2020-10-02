//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#ifndef _NANOHAL_RELEASEINFO_H_
#define _NANOHAL_RELEASEINFO_H_ 1

#include <nanoPackStruct.h>

struct __nfpack NFVersion
{
    unsigned short usMajor;
    unsigned short usMinor;
    unsigned short usBuild;
    unsigned short usRevision;
    
    // Version & NFReleaseInfo participate in a union in the debugging support,
    // and therefore cannot have real constructors, though that would be better
    // style otherwise.
    static void Init(NFVersion& version, unsigned short major=0, unsigned short minor=0, unsigned short build=0, unsigned short revision=0)
    {
        version.usMajor = major;
        version.usMinor = minor;
        version.usBuild = build;
        version.usRevision = revision;
    }
};

struct __nfpack NFReleaseInfo
{
    NFVersion Version;
    unsigned char InfoString[128];
    unsigned char TargetName[32];
    unsigned char PlatformName[32];
    
    static void Init(
        NFReleaseInfo& releaseInfo, 
        unsigned short major=0, 
        unsigned short minor=0, 
        unsigned short build=0, 
        unsigned short revision=0, 
        const char *info=(const char *)NULL, 
        size_t infoLen=0,
        const char *target=(const char *)NULL, 
        size_t targetLen=0,
        const char *platform=(const char *)NULL, 
        size_t platformLen=0
        );
};

struct __nfpack OEM_MODEL_SKU
{
    unsigned char  OEM;
    unsigned char  Model;
    unsigned short SKU;
};

struct __nfpack OEM_SERIAL_NUMBERS
{
    unsigned char module_serial_number[32];
    unsigned char system_serial_number[16];
};

struct __nfpack HalSystemInfo
{
    NFReleaseInfo       m_releaseInfo;
    OEM_MODEL_SKU       m_OemModelInfo;
    OEM_SERIAL_NUMBERS  m_OemSerialNumbers;
};

//--//

// GetHalSystemInfo() is defined in \src\HAL\nanoHAL_SystemInformation.cpp and is provided
// to allow convenient access to configuration and build information to the debugging
// system and to managed code.
bool GetHalSystemInfo(HalSystemInfo& systemInfo);

// Solution-specific function; see nanoBooterEntry.cpp in your solution's nanoBooter directory for implementation.
unsigned int nanoBooter_GetReleaseInfo(NFReleaseInfo& releaseInfo);

// target specific function, provide this to allow access to a vendor-provided
// informative string, platform, target and build-time version information.
bool Target_GetReleaseInfo(NFReleaseInfo&);

//--//


#endif // _NANOHAL_RELEASEINFO_H_

