//
// Copyright (c) 2017 The nano Framework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_COMMANDS_H_
#define _WIREPROTOCOL_COMMANDS_H_

#include <string.h>
#include "WireProtocol.h"

//////////////////////////////////////////
// enums

// enum with CLR and debugger commands
// backwards compatible with .NETMF
typedef enum CLR_DBG_Commands
{
    CLR_DBG_Commands_c_Monitor_Ping               = 0x00000000, // The payload is empty, this command is used to let the other side know we are here...
    CLR_DBG_Commands_c_Monitor_Message            = 0x00000001, // The payload is composed of the string characters, no zero at the end.
    CLR_DBG_Commands_c_Monitor_ReadMemory         = 0x00000002,
    CLR_DBG_Commands_c_Monitor_WriteMemory        = 0x00000003,
    CLR_DBG_Commands_c_Monitor_CheckMemory        = 0x00000004,
    CLR_DBG_Commands_c_Monitor_EraseMemory        = 0x00000005,
    CLR_DBG_Commands_c_Monitor_Execute            = 0x00000006,
    CLR_DBG_Commands_c_Monitor_Reboot             = 0x00000007,
    CLR_DBG_Commands_c_Monitor_MemoryMap          = 0x00000008,
    CLR_DBG_Commands_c_Monitor_ProgramExit        = 0x00000009, // The payload is empty, this command is used to tell the PC of a program termination
    CLR_DBG_Commands_c_Monitor_CheckSignature     = 0x0000000A,
    CLR_DBG_Commands_c_Monitor_DeploymentMap      = 0x0000000B,
    CLR_DBG_Commands_c_Monitor_FlashSectorMap     = 0x0000000C,
    CLR_DBG_Commands_c_Monitor_SignatureKeyUpdate = 0x0000000D,
    CLR_DBG_Commands_c_Monitor_OemInfo            = 0x0000000E,
}CLR_DBG_Commands;

// enum with flags for Monitor ping source and debugger flags
// backwards compatible with .NETMF in debugger flags only
// adds NEW flags for nanoBooter and nanoCLR
typedef enum Monitor_Ping_Source_Flags
{
    Monitor_Ping_c_Ping_Source_NanoCLR    = 0x00010000,
    Monitor_Ping_c_Ping_Source_NanoBooter = 0x00010001,

    Monitor_Ping_c_Ping_DbgFlag_Stop      = 0x00000001,
    Monitor_Ping_c_Ping_DbgFlag_AppExit   = 0x00000004,
}Monitor_Ping_Source_Flags;

//////////////////////////////////////////
// typedefs

// structure for Monitor Ping Reply
// backwards compatible with .NETMF
typedef struct Monitor_Ping_Reply
{
    uint32_t m_source;
    uint32_t m_dbg_flags;

}Monitor_Ping_Reply;

// structure to hold 'standard' version information
// equivalent with .NETMF MFVersion
typedef struct VersionInfo
{
    uint16_t usMajor;
    uint16_t usMinor;
    uint16_t usBuild;
    uint16_t usRevision;

}VersionInfo;

// structure to hold nano Framework release information
// equivalent with .NETMF MfReleaseInfo
typedef struct ReleaseInfo
{

    VersionInfo version;

    uint8_t infoString[64-sizeof(VersionInfo)];

}ReleaseInfo;

// structure with reply for OEM information command
typedef struct Monitor_OemInfo_Reply
{

    ReleaseInfo   m_releaseInfo;

}Monitor_OemInfo_Reply;

//////////////////////////////////////////
// function declarations (commands)

bool Monitor_Ping(WP_Message* message);
bool Monitor_OemInfo(WP_Message* message);

//////////////////////////////////////////
// helper functions
bool NanoBooter_GetReleaseInfo(ReleaseInfo* releaseInfo);

#endif //_WIREPROTOCOL_COMMANDS_H_
