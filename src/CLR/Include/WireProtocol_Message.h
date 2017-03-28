//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_MESSAGE_H_
#define _WIREPROTOCOL_MESSAGE_H_

#include "WireProtocol.h"
#include "WireProtocol_App_Interface.h"
#include "WireProtocol_HAL_Interface.h"

////////////////////////////////////////////////////
// function declarations (related with WP_Message)

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////
// helper functions

#ifdef __cplusplus
extern "C" {
#endif

void WP_ReplyToCommand(WP_Message* message, bool fSuccess, bool fCritical, void* ptr, int size);
void WP_SendProtocolMessage(WP_Message *message);
void WP_PrepareAndSendProtocolMessage(uint32_t cmd, uint32_t payloadSize, uint8_t* payload, uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif // _WIREPROTOCOL_MESSAGE_H_
