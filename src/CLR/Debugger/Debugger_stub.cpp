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

__nfweak void CLR_DBG_Debugger::Debugger_Discovery()
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
    NATIVE_PROFILE_CLR_DEBUGGER();
}

__nfweak void NFReleaseInfo::Init( NFReleaseInfo& NFReleaseInfo, unsigned short int major, unsigned short int minor, unsigned short int build, unsigned short int revision, const char *info, size_t infoLen )
{
    NFVersion::Init( NFReleaseInfo.version, major, minor, build, revision );
    NFReleaseInfo.infoString[ 0 ] = 0;
    if ( NULL != info && infoLen > 0 )
    {
        const size_t len = MIN(infoLen, sizeof(NFReleaseInfo.infoString)-1);
        hal_strncpy_s( (char*)&NFReleaseInfo.infoString[0], sizeof(NFReleaseInfo.infoString), info, len );
    }
}
