//
// Copyright (c) 2017 The nano Framework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_COMMANDS_H_
#define _WIREPROTOCOL_COMMANDS_H_

#include "WireProtocol.h"
#include "WireProtocol_Message.h"

//////////////////////////////////////////
// typedefs

// structure for Monitor Ping Reply
// backwards compatible with .NETMF
typedef struct Monitor_Ping_Reply
{
    uint32_t m_source;
    uint32_t m_dbg_flags;

}Monitor_Ping_Reply;

// structure with reply for OEM information command
typedef struct Monitor_OemInfo_Reply
{
    ReleaseInfo   m_releaseInfo;

}Monitor_OemInfo_Reply;

//////////////////////////////////////////
// function declarations (commands)

bool Monitor_Ping(WP_Message* message);
bool Monitor_OemInfo(WP_Message* message);

#endif //_WIREPROTOCOL_COMMANDS_H_
