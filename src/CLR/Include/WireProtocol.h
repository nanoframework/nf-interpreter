//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

// clang-format off

#ifndef WIREPROTOCOL_H
#define WIREPROTOCOL_H

#include <nanoCLR_Headers.h>
#include <nanoSupport.h>
#include "nanoVersion.h"

#if !defined(_WIN32)
#include <target_common.h>
#endif

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

#define MARKER_DEBUGGER_V1 "NFDBGV1" // Used to identify the debugger at boot time.
#define MARKER_PACKET_V1   "NFPKTV1" // Used to identify the start of a packet.

#ifndef WP_PACKET_SIZE
// no Wire Protocol defined at target_common, set to default as 1024
#define WP_PACKET_SIZE      1024U
#endif

// enum with Wire Protocol flags
// backwards compatible with .NETMF
typedef enum WP_Flags
{
    WP_Flags_c_NonCritical = 0x0001, // This doesn't need an acknowledge.
    WP_Flags_c_Reply       = 0x0002, // This is the result of a command.
    WP_Flags_c_BadHeader   = 0x0004,
    WP_Flags_c_BadPayload  = 0x0008,
    WP_Flags_c_Spare0010   = 0x0010,
    WP_Flags_c_Spare0020   = 0x0020,
    WP_Flags_c_Spare0040   = 0x0040,
    WP_Flags_c_Spare0080   = 0x0080,
    WP_Flags_c_Spare0100   = 0x0100,
    WP_Flags_c_Spare0200   = 0x0200,
    WP_Flags_c_Spare0400   = 0x0400,
    WP_Flags_c_Spare0800   = 0x0800,
    WP_Flags_c_Spare1000   = 0x1000,
    WP_Flags_c_NoCaching   = 0x2000,
    WP_Flags_c_NACK        = 0x4000,
    WP_Flags_c_ACK         = 0x8000
}WP_Flags;

// enum with machine states for Wire Procotol receiver
typedef enum ReceiveState
{
    ReceiveState_Idle             = (uint8_t)1,
    ReceiveState_Initialize       = (uint8_t)2,
    ReceiveState_WaitingForHeader = (uint8_t)3,
    ReceiveState_ReadingHeader    = (uint8_t)4,
    ReceiveState_CompleteHeader   = (uint8_t)5,
    ReceiveState_ReadingPayload   = (uint8_t)6,
    ReceiveState_CompletePayload  = (uint8_t)7,
}ReceiveState;

// enum with CLR monitor commands
// backwards compatible with .NETMF
typedef enum CLR_DBG_Commands_Monitor
{
    CLR_DBG_Commands_c_Monitor_Ping                = 0x00000000, // The payload is empty, this command is used to let the other side know we are here...
    CLR_DBG_Commands_c_Monitor_Message             = 0x00000001, // The payload is composed of the string characters, no zero at the end.
    CLR_DBG_Commands_c_Monitor_ReadMemory          = 0x00000002,
    CLR_DBG_Commands_c_Monitor_WriteMemory         = 0x00000003,
    CLR_DBG_Commands_c_Monitor_CheckMemory         = 0x00000004,
    CLR_DBG_Commands_c_Monitor_EraseMemory         = 0x00000005,
    CLR_DBG_Commands_c_Monitor_Execute             = 0x00000006,
    CLR_DBG_Commands_c_Monitor_Reboot              = 0x00000007,
    CLR_DBG_Commands_c_Monitor_MemoryMap           = 0x00000008,
    CLR_DBG_Commands_c_Monitor_ProgramExit         = 0x00000009, // The payload is empty, this command is used to tell the PC of a program termination
    CLR_DBG_Commands_c_Monitor_CheckSignature      = 0x0000000A,
    CLR_DBG_Commands_c_Monitor_DeploymentMap       = 0x0000000B,
    CLR_DBG_Commands_c_Monitor_FlashSectorMap      = 0x0000000C,
    CLR_DBG_Commands_c_Monitor_OemInfo             = 0x0000000E,
    CLR_DBG_Commands_c_Monitor_QueryConfiguration  = 0x0000000F,
    CLR_DBG_Commands_c_Monitor_UpdateConfiguration = 0x00000010,
    CLR_DBG_Commands_c_Monitor_TargetInfo          = 0x00000020,
}CLR_DBG_Commands_Monitor;

// structure for Wire Protocol packet
// backwards compatible with .NETMF
typedef struct WP_Packet
{
    uint8_t  m_signature[8];
    uint32_t m_crcHeader;
    uint32_t m_crcData;

    uint32_t m_cmd;
    uint16_t m_seq;
    uint16_t m_seqReply;
    uint32_t m_flags;
    uint32_t m_size;
}WP_Packet;

typedef struct WP_Controller
{
    uint8_t* m_szMarker;

    uint16_t m_lastOutboundMessage;

    bool (*AdvanceState)(void);
    void (*Initialize)(void* szMarker, void* phy, void* app, void* state );
    //bool (*SendProtocolMessage)( const void* msg );
    bool (*SendProtocolMessage)( uint32_t cmd, uint32_t flags, uint32_t payloadSize, uint8_t* payload);

}WP_Controller;

// structure for Wire Protocol message
// backwards compatible with .NETMF equivalent in names and types to help code reuse
typedef struct WP_Message
{
    WP_Packet      m_header;
    uint8_t*       m_payload;
}WP_Message;


// enum with flags for Monitor ping source and debugger flags
///////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Monitor_Ping (in debugger library code) !!! //
///////////////////////////////////////////////////////////////////////
typedef enum Monitor_Ping_Source_Flags
{
    ///////////////////////////////////////////////////////

    Monitor_Ping_c_Ping_Source_NanoCLR =        0x00010000,
    Monitor_Ping_c_Ping_Source_NanoBooter =     0x00010001,
    Monitor_Ping_c_Ping_Source_Host =           0x00010002,

    Monitor_Ping_c_Ping_DbgFlag_Stop =          0x00000001,
    Monitor_Ping_c_Ping_DbgFlag_AppExit =       0x00000004,
    
    ///////////////////////////////////////////////////////
    // flags specific to Wire Protocol capabilities

    Monitor_Ping_c_Ping_WPFlag_SupportsCRC32 =  0x00000010,
    
    // Wire Protocol packet size (3rd position)
    Monitor_Ping_c_PacketSize_1024 =            0x00000100,
    Monitor_Ping_c_PacketSize_0512 =            0x00000200,
    Monitor_Ping_c_PacketSize_0256 =            0x00000300,
    Monitor_Ping_c_PacketSize_0128 =            0x00000400,

    ///////////////////////////////////////////////////////
    // flags related with device capabilities

    // This flag indicates that the device has a proprietary bootloader.
    Monitor_Ping_c_HasProprietaryBooter =       0x00010000,

    // This flag indicates that the target device is IFU capable.
    Monitor_Ping_c_IFUCapable =                 0x00020000,

    // This flag indicates that the device requires that the configuration block to be erased before updating it.
    Monitor_Ping_c_ConfigBlockRequiresErase =   0x00040000,

    // This flag indicates that the device has nanoBooter.
    Monitor_Ping_c_HasNanoBooter =              0x00080000,

}Monitor_Ping_Source_Flags;

// structure to hold nanoFramework release information
// equivalent with .NETMF MfReleaseInfo
typedef struct ReleaseInfo
{

    VersionInfo version;
    uint8_t InfoString[128];
    uint8_t TargetName[32];
    uint8_t PlatformName[32];
    uint8_t PlatformInfoString[128];

}ReleaseInfo;

// structure to hold nanoFramework Target information
typedef struct TargetInfo
{

    VersionInfo BooterVersion;
    VersionInfo ClrVersion;
    uint8_t InfoString[128];
    uint8_t TargetName[32];
    uint8_t PlatformName[32];
    uint8_t PlatformInfoString[128];

}TargetInfo;

// structure for Wire Protocol command handler lookup
typedef struct CommandHandlerLookup
{
    // command code goes here
    uint32_t command;
    
    // pointer to handler function 
    void* handler;

}CommandHandlerLookup;

// This structure is never used, its purpose is to generate a compiler error in case the size of any structure changes.
struct WP_CompileCheck
{
    char buf1[ sizeof(WP_Packet) == 8 * 4 ? 1 : -1 ];
};

#endif // WIREPROTOCOL_H

// clang-format on
