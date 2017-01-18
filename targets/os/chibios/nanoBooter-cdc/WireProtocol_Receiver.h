//
// Copyright (c) 2017 The nano Framework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_RECEIVER_H_
#define _WIREPROTOCOL_RECEIVER_H_

#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include "WireProtocol.h"
#include "WireProtocol_Commands.h"

#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef ARRAYSIZE
#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))
#endif

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

// structure for Wire Protocol command handler lookup
typedef struct CommandHandlerLookup
{
    uint32_t command;
    
    void* handler;

}CommandHandlerLookup;

////////////////////////////////////////////////////
// function declarations (related with WP_Message)

void WP_Message_Initialize(WP_Message* message);
void WP_Message_PrepareReception(WP_Message* message);
void WP_Message_PrepareRequest(WP_Message* message, uint32_t cmd, uint32_t flags, uint32_t payloadSize, uint8_t* payload);
void WP_Message_PrepareReply(WP_Message* message, const WP_Packet* req, uint32_t flags, uint32_t payloadSize,  uint8_t* payload);
void WP_Message_SetPayload(WP_Message* message, uint8_t* payload);
void WP_Message_Release(WP_Message* message);
bool WP_Message_VerifyHeader(WP_Message* message);
bool WP_Message_VerifyPayload(WP_Message* message);
void WP_Message_ReplyBadPacket(uint32_t flags);
bool WP_Message_Process(WP_Message* message);
bool ReceiveBytes(uint8_t* ptr, uint16_t* size);

//////////////////////////////////////////
// helper functions
void ReplyToCommand(WP_Message* message, bool fSuccess, bool fCritical, void* ptr, int size);
bool SendProtocolMessage(WP_Message* message);
bool TransmitMessage(WP_Message* message);
bool ProcessHeader(WP_Message* message);
bool ProcessPayload(WP_Message* message);

uint32_t SUPPORT_ComputeCRC(const void* rgBlock, int nLength, uint32_t crc);

//////////////////////////////////////////
// functions with RTOS stuff
void ReceiverThread(void const * argument);

#endif // _WIREPROTOCOL_RECEIVER_H_
