//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//
#include <nanoCLR_Runtime.h>


////////////////////////////////////////////////////////////////////////////////////////////////////

__nfweak void CLR_DBG_Debugger::Debugger_WaitForCommands()
{
    NATIVE_PROFILE_CLR_DEBUGGER();
}

__nfweak void CLR_DBG_Debugger::PurgeCache()
{
    NATIVE_PROFILE_CLR_DEBUGGER();
}

__nfweak HRESULT CLR_DBG_Debugger::CreateInstance()
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    g_CLR_DBG_Debugger = (CLR_DBG_Debugger*)&g_scratchDebugger[0];
    NANOCLR_SYSTEM_STUB_RETURN();
}

//--//

__nfweak HRESULT CLR_DBG_Debugger::DeleteInstance()
{
    NATIVE_PROFILE_CLR_DEBUGGER();
    NANOCLR_SYSTEM_STUB_RETURN();
}

__nfweak void CLR_DBG_Debugger::BroadcastEvent( unsigned int cmd, unsigned int payloadSize, unsigned char* payload, unsigned int flags )
{
    (void)cmd;
    (void)payloadSize;
    (void)payload;
    (void)flags;

    NATIVE_PROFILE_CLR_DEBUGGER();
}

__nfweak void NFReleaseInfo::Init(
    NFReleaseInfo& NFReleaseInfo, 
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

    NFVersion::Init( NFReleaseInfo.Version, major, minor, build, revision );
    
    // better set these to empty strings, in case there is nothing to fill in
    NFReleaseInfo.InfoString[ 0 ] = 0;
    NFReleaseInfo.TargetName[ 0 ] = 0;
    NFReleaseInfo.PlatformName[ 0 ] = 0;

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

__nfweak bool SOCKETS_DbgInitialize( int ComPortNum )
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    (void)ComPortNum;
    return true;
}

__nfweak bool SOCKETS_DbgUninitialize( int ComPortNum )
{
    NATIVE_PROFILE_CLR_DEBUGGER();

    (void)ComPortNum;
    return true;
}
