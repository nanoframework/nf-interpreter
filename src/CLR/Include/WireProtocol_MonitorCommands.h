//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef WIREPROTOCOL_COMMANDS_H
#define WIREPROTOCOL_COMMANDS_H

#include <nanoHAL_v2.h>
#include "WireProtocol.h"
#include "WireProtocol_Message.h"

// clang-format off

//////////////////////////////////////////
// enums

// structure for Monitor Reboot options
/////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.WireProtocol.RebootOptions (in managed code) !!! //
// CONSTANTS VALUES NEED TO BE 'FLAG' TYPE                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum Monitor_Reboot_Options
{
    Monitor_Reboot_c_NormalReboot           = 0,
    Monitor_Reboot_c_EnterNanoBooter        = 1,
    Monitor_Reboot_c_ClrRebootOnly          = 2,
    Monitor_Reboot_c_ClrStopDebugger        = 4,
    Monitor_Reboot_c_EnterProprietaryBooter = 8
} Monitor_Reboot_Options;

// structure for Access Memory operations
typedef enum AccessMemory_Operations
{
    // compute CRC32 of the of a block of data starting at a given address
    AccessMemory_Check                      = 0x00,

    // read block of data starting at a given address
    AccessMemory_Read                       = 0x01,

    // write block of data starting at a given address
    AccessMemory_Write                      = 0x02,

    // erase sector/block/page at a given address
    AccessMemory_Erase                      = 0x03,

    AccessMemory_Mask                       = 0x0F

} AccessMemory_Operations;

// true if [location, location + length) overlaps [regionStart, regionEnd)
// computed in 64 bits so that the end of the range can't wrap around
static inline bool AccessMemory_OverlapsRegion(uint32_t location, uint32_t length, uint32_t regionStart, uint32_t regionEnd)
{
    return (uint64_t)location < (uint64_t)regionEnd && (uint64_t)location + length > (uint64_t)regionStart;
}

typedef enum MemoryMap_Options
{
    Monitor_MemoryMap_c_RAM     = 0x00000001,
    Monitor_MemoryMap_c_FLASH   = 0x00000002,

} MemoryMap_Options;

// structure for storage operations
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.WireProtocol.Monitor_StorageOperation.StorageOperation (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum StorageOperation_Monitor
{
    StorageOperation_None   = 0,
    StorageOperation_Write  = 1,
    StorageOperation_Delete = 2,
    StorageOperation_Append = 3
} StorageOperation_Monitor;

//////////////////////////////////////////
// typedefs

// structure for Monitor Ping Reply
typedef struct Monitor_Ping_Reply
{
    uint32_t Source;
    uint32_t Flags;

} Monitor_Ping_Reply;

// structure for command Monitor Ping
typedef struct Monitor_Ping_Command
{
    uint32_t Source;
    uint32_t Flags;

} Monitor_Ping_Command;

// structure with reply for OEM information command
typedef struct Monitor_OemInfo_Reply
{
    ReleaseInfo m_releaseInfo;

} Monitor_OemInfo_Reply;

// structure with reply for Target information command
typedef struct Monitor_TargetInfo_Reply
{
    TargetInfo m_TargetInfo;

} Monitor_TargetInfo_Reply;

typedef struct CLR_DBG_Commands_Monitor_ReadMemory
{
    uint32_t        address;
    uint32_t        length;
    unsigned char   data[1];

} CLR_DBG_Commands_Monitor_ReadMemory;

typedef struct CLR_DBG_Commands_Monitor_WriteMemory
{
    uint32_t        address;
    uint32_t        length;
    unsigned char   data[1];

} CLR_DBG_Commands_Monitor_WriteMemory;

typedef struct Monitor_Reboot_Command
{
    uint32_t m_flags;

} Monitor_Reboot_Command;

typedef struct CLR_DBG_Commands_Monitor_EraseMemory
{
    uint32_t address;
    uint32_t length;

} CLR_DBG_Commands_Monitor_EraseMemory;

typedef struct CLR_DBG_Commands_Monitor_CheckMemory
{
    uint32_t address;
    uint32_t length;

} CLR_DBG_Commands_Monitor_CheckMemory;

typedef uint32_t CLR_DBG_Commands_Monitor_CheckMemory_Reply;

typedef uint32_t CLR_DBG_Commands_Monitor_EraseMemory_Reply;

typedef uint32_t CLR_DBG_Commands_Monitor_WriteMemory_Reply;

typedef struct MemoryMap_Range
{
    uint32_t m_address;
    uint32_t m_length;
    uint32_t m_flags;

} MemoryMap_Range;

typedef struct CLR_DBG_Commands_Monitor_MemoryMap
{
    MemoryMap_Range m_map[1];

} CLR_DBG_Commands_Monitor_MemoryMap;

typedef struct Monitor_QueryConfiguration_Command
{
    uint32_t    Configuration;
    uint32_t    BlockIndex;

} Monitor_QueryConfiguration_Command;

typedef struct Monitor_StorageOperation_Command
{
    uint32_t    Operation;
    uint32_t    DataLength;
    uint32_t    Offset;
    uint8_t     Data[1];

} Monitor_StorageOperation_Command;

typedef struct Monitor_StorageOperation_Reply
{
    uint32_t ErrorCode;

} Monitor_StorageOperation_Reply;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.WireProtocol.Commands.Monitor_ConfigurationInfo (in managed code) !!! //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// information about a configuration block type
typedef struct __nfpack Monitor_ConfigurationInfo_BlockInfo
{
    // DeviceConfigurationOption
    uint8_t     Configuration;

    // count of blocks stored
    uint8_t     Count;

    // maximum count of blocks, 0 if this type isn't supported
    uint8_t     MaxCount;

    // ConfigurationBlockFlags
    uint8_t     Flags;

    // size of the block, or the maximum size for blocks of variable size (certificates)
    uint32_t    MaxBlockSize;

} Monitor_ConfigurationInfo_BlockInfo;

// count of configuration block types reported
#define MONITOR_CONFIGURATION_INFO_BLOCK_TYPES  6

typedef struct __nfpack Monitor_ConfigurationInfo_Reply
{
    // CONFIGURATION_STORAGE_API_VERSION
    uint32_t    Version;

    // ConfigurationStorageFlags
    uint32_t    Flags;

    // size of the configuration storage, 0 if unknown
    uint32_t    TotalSize;

    // free space in the configuration storage, 0xFFFFFFFF if unknown
    uint32_t    FreeSize;

    // count of entries in Blocks
    uint32_t    BlockInfoCount;

    Monitor_ConfigurationInfo_BlockInfo Blocks[MONITOR_CONFIGURATION_INFO_BLOCK_TYPES];

} Monitor_ConfigurationInfo_Reply;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH nanoFramework.Tools.Debugger.WireProtocol.Commands.Monitor_ConfigurationOperation (in managed code) !!! //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct __nfpack Monitor_ConfigurationOperation_Command
{
    // ConfigurationOperation
    uint32_t    Operation;

    // DeviceConfigurationOption
    uint32_t    Configuration;

    // index of the block to replace or delete, ignored for add
    uint32_t    BlockIndex;

    // ConfigurationOperationFlags
    uint32_t    Flags;

    // size of the whole block, 0 for delete
    uint32_t    TotalLength;

    // offset of this chunk in the block
    uint32_t    Offset;

    // size of this chunk
    uint32_t    Length;

    uint8_t     Data[1];

} Monitor_ConfigurationOperation_Command;

typedef struct __nfpack Monitor_ConfigurationOperation_Reply
{
    // ConfigurationOperationError
    uint32_t    ErrorCode;

    // index of the block after the operation (add, replace), 0xFFFFFFFF otherwise
    uint32_t    BlockIndex;

    // count of blocks of this type after the operation
    uint32_t    Count;

    // Id of the wireless profile after the operation (add, replace), 0xFFFFFFFF otherwise
    uint32_t    Id;

} Monitor_ConfigurationOperation_Reply;

typedef struct __nfpack Flash_BlockRegionInfo
{
    uint32_t StartAddress;
    uint32_t NumBlocks;
    uint32_t BytesPerBlock;
    uint32_t Flags;
} Flash_BlockRegionInfo;

#define BlockRegionAttributes_MASK  ((uint32_t)0x0000FF00)

// clang-format on

//////////////////////////////////////////
// function declarations (commands)

#ifdef __cplusplus
extern "C"
{
#endif

    int Monitor_Ping(WP_Message *message);
    int Monitor_OemInfo(WP_Message *message);
    int Monitor_WriteMemory(WP_Message *message);
    int Monitor_ReadMemory(WP_Message *message);
    int Monitor_Reboot(WP_Message *message);
    int Monitor_EraseMemory(WP_Message *message);
    int Monitor_QueryConfiguration(WP_Message *message);
    int Monitor_StorageOperation(WP_Message *message);
    int Monitor_CheckMemory(WP_Message *message);
    int Monitor_MemoryMap(WP_Message *message);
    int Monitor_FlashSectorMap(WP_Message *message);
    int Monitor_TargetInfo(WP_Message *message);

    // helper functions
    int AccessMemory(uint32_t location, uint32_t lengthInBytes, uint8_t *buffer, int32_t mode, uint32_t *errorCode);
    int nanoBooter_GetTargetInfo(TargetInfo *targetInfo);
    int NanoBooter_GetReleaseInfo(ReleaseInfo *releaseInfo);

#ifdef __cplusplus
}
#endif

#endif // WIREPROTOCOL_COMMANDS_H
