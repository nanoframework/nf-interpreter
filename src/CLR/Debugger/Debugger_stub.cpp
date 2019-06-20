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
    NFVersion::Init( NFReleaseInfo.Version, major, minor, build, revision );
    NFReleaseInfo.InfoString[ 0 ] = 0;
    if ( NULL != info && infoLen > 0 )
    {
        size_t len = MIN(infoLen, sizeof(NFReleaseInfo.InfoString)-1);
        hal_strncpy_s( (char*)&NFReleaseInfo.InfoString[0], sizeof(NFReleaseInfo.InfoString), info, len );
        
        len = MIN(targetLen, sizeof(NFReleaseInfo.TargetName)-1);
        hal_strncpy_s( (char*)&NFReleaseInfo.TargetName[0], sizeof(NFReleaseInfo.TargetName), target, len );
        
        len = MIN(platformLen, sizeof(NFReleaseInfo.PlatformName)-1);
        hal_strncpy_s( (char*)&NFReleaseInfo.PlatformName[0], sizeof(NFReleaseInfo.PlatformName), platform, len );
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
