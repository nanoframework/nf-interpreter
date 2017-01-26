//
// Copyright (c) 2017 The nano Framework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <WireProtocol.h>
#include <WireProtocol_MonitorCommands.h>

//////////////////////////////////////////////////////////////////////
// helper functions

bool NanoBooter_GetReleaseInfo(ReleaseInfo* releaseInfo)
{
    releaseInfo->version.usMajor = 0;
    releaseInfo->version.usMinor = 0;
    releaseInfo->version.usBuild = 12345;
    releaseInfo->version.usRevision = 1;

    // TODO replace this with string from (possibly...) main config file
    memcpy(&releaseInfo->infoString, ">>>> nanoFramework RULES (nanoBooter here) <<<<", sizeof(releaseInfo->infoString));

    return true;
}


////////////////////////////////////////////////////

bool Monitor_Ping(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_Ping_Reply cmdReply;
        cmdReply.m_source = Monitor_Ping_c_Ping_Source_NanoBooter;

        ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

bool Monitor_OemInfo(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_OemInfo_Reply cmdReply;     
        
        bool fOK = NanoBooter_GetReleaseInfo(&cmdReply.m_releaseInfo) == true;
        
        ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}
