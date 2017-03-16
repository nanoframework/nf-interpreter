//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_COMMANDS_H_
#define _WIREPROTOCOL_COMMANDS_H_

#include "WireProtocol_v2.h"
#include "WireProtocol_Message.h"

//////////////////////////////////////////
// enums

// structure for Monitor Reboot options
// backwards compatible with .NETMF
typedef enum Monitor_Reboot_Options
{
    Monitor_Reboot_c_NormalReboot    = 0,
    Monitor_Reboot_c_EnterBootloader = 1,
    Monitor_Reboot_c_ClrRebootOnly   = 2,
    Monitor_Reboot_c_ClrStopDebugger = 4
}Monitor_Reboot_Options;

// structure for Access Memory operations
typedef enum AccessMemory_Operations
{
    // check if memory space is erased
    AccessMemory_Check    = 0x00,

    // read block of data starting at a given address
    AccessMemory_Read     = 0x01,
    
    // write block of data starting at a given address
    AccessMemory_Write    = 0x02,

    // erase sector/block/page at a given address 
    AccessMemory_Erase    = 0x03,


    AccessMemory_Mask     = 0x0F

}AccessMemory_Operations;

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

typedef struct CLR_DBG_Commands_Monitor_WriteMemory
{
    uint32_t address;
    uint32_t length;
    uint8_t  data[1];

}CLR_DBG_Commands_Monitor_WriteMemory;

typedef struct Monitor_Reboot_Command
{
    uint32_t m_flags;

}Monitor_Reboot_Command;

typedef struct CLR_DBG_Commands_Monitor_EraseMemory
{
    uint32_t address;
    uint32_t length;

}CLR_DBG_Commands_Monitor_EraseMemory;

//////////////////////////////////////////
// function declarations (commands)

bool Monitor_Ping(WP_Message* message);
bool Monitor_OemInfo(WP_Message* message);
bool Monitor_WriteMemory(WP_Message* message);
bool Monitor_Reboot(WP_Message* message);
bool Monitor_EraseMemory(WP_Message* message);

#endif //_WIREPROTOCOL_COMMANDS_H_

