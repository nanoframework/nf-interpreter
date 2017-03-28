//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "WireProtocol.h"
#include "WireProtocol_MonitorCommands.h"
#include <target_board.h>

//////////////////////////////////////////////////////////////////////
// helper functions

bool NanoCLR_GetReleaseInfo(ReleaseInfo* releaseInfo)
{
    releaseInfo->version.usMajor = VERSION_MAJOR;
    releaseInfo->version.usMinor = VERSION_MINOR;
    releaseInfo->version.usBuild = VERSION_BUILD;
    releaseInfo->version.usRevision = 0;

    memcpy(&releaseInfo->infoString, OEMSYSTEMINFOSTRING, sizeof(releaseInfo->infoString));

    return true;
}


////////////////////////////////////////////////////

bool Monitor_Ping(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_Ping_Reply cmdReply;
        cmdReply.m_source = Monitor_Ping_c_Ping_Source_NanoCLR;

        WP_ReplyToCommand(message, true, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}

bool Monitor_OemInfo(WP_Message* message)
{
    if((message->m_header.m_flags & WP_Flags_c_Reply) == 0)
    {
        Monitor_OemInfo_Reply cmdReply;     
        
        bool fOK = NanoCLR_GetReleaseInfo(&cmdReply.m_releaseInfo) == true;
        
        WP_ReplyToCommand(message, fOK, false, &cmdReply, sizeof(cmdReply));
    }

    return true;
}
