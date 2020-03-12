//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_COMMANDS_H_
#define _WIREPROTOCOL_COMMANDS_H_

#include "WireProtocol.h"
#include "WireProtocol_Message.h"
#include <nanoPackStruct.h>

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

typedef enum MemoryMap_Options
{
    Monitor_MemoryMap_c_RAM     = 0x00000001,
    Monitor_MemoryMap_c_FLASH   = 0x00000002,

}MemoryMap_Options;


//////////////////////////////////////////
// typedefs

// structure for Monitor Ping Reply
// backwards compatible with .NETMF
typedef struct Monitor_Ping_Reply
{
    uint32_t m_source;
    uint32_t m_dbg_flags;

}Monitor_Ping_Reply;

// structure for command Monitor Ping
// backwards compatible with .NETMF
typedef struct Monitor_Ping_Command
{
    uint32_t m_source;
    uint32_t m_dbg_flags;

}Monitor_Ping_Command;


// structure with reply for OEM information command
typedef struct Monitor_OemInfo_Reply
{
    ReleaseInfo   m_releaseInfo;

}Monitor_OemInfo_Reply;

typedef struct CLR_DBG_Commands_Monitor_ReadMemory
{
    uint32_t    address;
    uint32_t    length;
    unsigned char   data[1];

}CLR_DBG_Commands_Monitor_ReadMemory;

typedef struct CLR_DBG_Commands_Monitor_WriteMemory
{
    uint32_t    address;
    uint32_t    length;
    unsigned char   data[1];

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

typedef struct CLR_DBG_Commands_Monitor_CheckMemory
{
    uint32_t address;
    uint32_t length;

}CLR_DBG_Commands_Monitor_CheckMemory;

typedef struct CLR_DBG_Commands_Monitor_CheckMemory_Reply
{
    uint32_t crc;
    
}CLR_DBG_Commands_Monitor_CheckMemory_Reply;

typedef struct CLR_DBG_Commands_Monitor_EraseMemory_Reply
{
    uint32_t ErrorCode;

}CLR_DBG_Commands_Monitor_EraseMemory_Reply;

typedef struct CLR_DBG_Commands_Monitor_WriteMemory_Reply
{
    uint32_t ErrorCode;

}CLR_DBG_Commands_Monitor_WriteMemory_Reply;

typedef struct MemoryMap_Range
{
    uint32_t m_address;
    uint32_t m_length;
    uint32_t m_flags;

}MemoryMap_Range;


typedef struct CLR_DBG_Commands_Monitor_MemoryMap
{
    MemoryMap_Range m_map[1];

}CLR_DBG_Commands_Monitor_MemoryMap;

typedef struct Monitor_QueryConfiguration_Command
{
    uint32_t Configuration;
    uint32_t BlockIndex;

}Monitor_QueryConfiguration_Command;

typedef struct __nfpack Monitor_UpdateConfiguration_Command
{
    uint32_t Configuration;
    uint32_t BlockIndex;
    uint32_t Length;
    uint32_t Offset;
    uint32_t Done;
    uint8_t Data[1];

}Monitor_UpdateConfiguration_Command;

typedef struct Monitor_UpdateConfiguration_Reply
{
    uint32_t ErrorCode;

}Monitor_UpdateConfiguration_Reply;

//////////////////////////////////////////
// function declarations (commands)

#ifdef __cplusplus
extern "C" {
#endif

int Monitor_Ping(WP_Message* message);
int Monitor_OemInfo(WP_Message* message);
int Monitor_WriteMemory(WP_Message* message);
int Monitor_ReadMemory(WP_Message* message);
int Monitor_Reboot(WP_Message* message);
int Monitor_EraseMemory(WP_Message* message);
int Monitor_QueryConfiguration(WP_Message* message);
int Monitor_UpdateConfiguration(WP_Message* message);
int Monitor_CheckMemory(WP_Message* message);
int Monitor_MemoryMap(WP_Message* message);
int Monitor_FlashSectorMap(WP_Message* message);

#ifdef __cplusplus
}
#endif

#endif //_WIREPROTOCOL_COMMANDS_H_
