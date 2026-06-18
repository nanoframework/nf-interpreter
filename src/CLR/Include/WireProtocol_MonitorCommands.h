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

typedef struct __nfpack Monitor_UpdateConfiguration_Command
{
    uint32_t    Configuration;
    uint32_t    BlockIndex;
    uint32_t    Length;
    uint32_t    Offset;
    uint32_t    Done;
    uint8_t     Data[1];

} Monitor_UpdateConfiguration_Command;


typedef struct Monitor_StorageOperation_Command
{
    uint32_t    Operation;
    uint32_t    NameLength;
    uint32_t    DataLength;
    uint32_t    Offset;
    uint8_t     Data[1];

} Monitor_StorageOperation_Command;

typedef struct Monitor_StorageOperation_Reply
{
    uint32_t ErrorCode;

} Monitor_StorageOperation_Reply;

typedef struct Monitor_UpdateConfiguration_Reply
{
    uint32_t ErrorCode;

} Monitor_UpdateConfiguration_Reply;

typedef struct __nfpack Flash_BlockRegionInfo
{
    uint32_t StartAddress;
    uint32_t NumBlocks;
    uint32_t BytesPerBlock;
    uint32_t Flags;
} Flash_BlockRegionInfo;

#define BlockRegionAttributes_MASK  ((uint32_t)0x0000FF00)

#if CONFIG_NF_FEATURE_HAS_MCUBOOT

//////////////////////////////////////////
// IFU (In-Field Update) command structures
//
// These commands manage MCUboot images over the Wire Protocol during nanoCLR
// application time. They are only compiled when CONFIG_NF_FEATURE_HAS_MCUBOOT is set.

// slot indexes used by the IFU commands
typedef enum Monitor_Image_Slot
{
    Monitor_Image_Slot_Primary   = 0, // running slot (dev write path, unverified by the loader)
    Monitor_Image_Slot_Secondary = 1, // staging slot (normal update path, triggers test-swap)
} Monitor_Image_Slot;

// state flags reported per slot in Monitor_ImageInfo
typedef enum Monitor_Image_StateFlags
{
    Monitor_Image_State_Active    = 0x01, // image is the one currently running
    Monitor_Image_State_Confirmed = 0x02, // image is confirmed (permanent - no revert)
    Monitor_Image_State_Pending   = 0x04, // image is scheduled for a one-time test-swap
} Monitor_Image_StateFlags;

// error codes carried in the ErrorCode field of the IFU command replies
typedef enum Monitor_Image_Error
{
    Monitor_Image_Error_Success     = 0, // operation succeeded
    Monitor_Image_Error_BadArgument = 1, // invalid image or slot index
    Monitor_Image_Error_FlashOpen   = 2, // could not open the target flash area
    Monitor_Image_Error_BadMagic    = 3, // first chunk is not a valid MCUboot image
    Monitor_Image_Error_TooLarge    = 4, // image would exceed the target slot capacity
    Monitor_Image_Error_Erase       = 5, // flash erase failed
    Monitor_Image_Error_Write       = 6, // flash write failed
    Monitor_Image_Error_SetPending  = 7, // boot_set_pending failed
    Monitor_Image_Error_Confirm     = 8, // boot_set_confirmed failed
} Monitor_Image_Error;

// per-slot entry returned by Monitor_ImageInfo
typedef struct __nfpack Monitor_ImageInfo_Entry
{
    uint8_t  ImageIndex;    // 0 = nanoCLR, 1 = deployment
    uint8_t  SlotIndex;     // 0 = primary, 1 = secondary
    uint8_t  Flags;         // Monitor_Image_StateFlags bitmask
    uint8_t  Valid;         // non-zero when the slot holds a valid MCUboot image
    uint32_t Version;       // packed MCUboot header version: (major<<24)|(minor<<16)|revision
    uint8_t  Hash[32];      // SHA-256 of the image (from IMAGE_TLV_SHA256)
} Monitor_ImageInfo_Entry;

// reply for the Monitor_ImageInfo command (variable number of entries)
typedef struct __nfpack Monitor_ImageInfo_Reply
{
    uint8_t                 ImageCount;  // number of entries that follow
    Monitor_ImageInfo_Entry Images[1];   // variable length
} Monitor_ImageInfo_Reply;

// command to write a chunk of image data to a target slot
typedef struct __nfpack Monitor_ImageWrite_Command
{
    uint8_t  ImageIndex;    // 0 = CLR firmware, 1 = deployment
    uint8_t  SlotIndex;     // 0 = primary (dev path), 1 = secondary (normal update path)
    uint32_t Offset;        // byte offset within the target slot
    uint32_t TotalSize;     // total image size (only meaningful when Offset == 0)
    uint8_t  Data[1];       // chunk data (variable length)
} Monitor_ImageWrite_Command;

// reply for the Monitor_ImageWrite command
typedef struct __nfpack Monitor_ImageWrite_Reply
{
    uint32_t ErrorCode;     // 0 = success
    uint32_t NextOffset;    // next expected offset for flow control
} Monitor_ImageWrite_Reply;

// command to confirm the currently running image as permanent
typedef struct __nfpack Monitor_ImageConfirm_Command
{
    uint32_t ImageIndex;    // image to confirm (defaults to 0)
} Monitor_ImageConfirm_Command;

// reply for the Monitor_ImageConfirm command
typedef struct __nfpack Monitor_ImageConfirm_Reply
{
    uint32_t ErrorCode;     // 0 = success
} Monitor_ImageConfirm_Reply;

// command to erase a slot (typically the secondary slot)
typedef struct __nfpack Monitor_ImageErase_Command
{
    uint32_t ImageIndex;    // image whose slot is to be erased
    uint32_t SlotIndex;     // slot to erase (typically Monitor_Image_Slot_Secondary)
} Monitor_ImageErase_Command;

// reply for the Monitor_ImageErase command
typedef struct __nfpack Monitor_ImageErase_Reply
{
    uint32_t ErrorCode;     // 0 = success
} Monitor_ImageErase_Reply;

#endif // CONFIG_NF_FEATURE_HAS_MCUBOOT

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
    int Monitor_UpdateConfiguration(WP_Message *message);
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
