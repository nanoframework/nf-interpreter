//
// Copyright (c) 2017 The nano Framework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_H_
#define _WIREPROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

#define MARKER_DEBUGGER_V1 "MSdbgV1" // Used to identify the debugger at boot time.
#define MARKER_PACKET_V1   "MSpktV1" // Used to identify the start of a packet.

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
    ReceiveState_Idle             = (1 << 0),
    ReceiveState_Initialize       = (2 << 0),
    ReceiveState_WaitingForHeader = (3 << 0),
    ReceiveState_ReadingHeader    = (4 << 0),
    ReceiveState_CompleteHeader   = (5 << 0),
    ReceiveState_ReadingPayload   = (6 << 0),
    ReceiveState_CompletePayload  = (7 << 0),
}ReceiveState;

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

// structure for Wire Protocol message
// backwards compatible with .NETMF equivalent in names and types to help code reuse
typedef struct WP_Message
{
    WP_Packet      m_header;
    uint8_t*       m_payload;

    uint8_t*       m_pos;
    uint16_t       m_size;
    int            m_rxState;
}WP_Message;

// This structure is never used, its purpose is to generate a compiler error in case the size of any structure changes.
struct WP_CompileCheck
{
    char buf1[ sizeof(WP_Packet) == 8 * 4 ? 1 : -1 ];
};

extern uint8_t* marker;
extern uint16_t lastOutboundMessage;
extern uint32_t lastPacketSequence;
extern WP_Message inboundMessage;

#endif // _WIREPROTOCOL_H_
