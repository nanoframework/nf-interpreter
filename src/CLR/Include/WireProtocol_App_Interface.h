//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_APP_INTERFACE_H_
#define _WIREPROTOCOL_APP_INTERFACE_H_

#include "WireProtocol.h"

//////////////////////////////////////////
extern uint8_t receptionBuffer[sizeof(WP_Packet) + WP_PACKET_SIZE];
extern void ReplyToCommand(WP_Message* message, int fSuccess, int fCritical, void* ptr, int size);

//////////////////////////////////////////
int WP_App_ProcessHeader(WP_Message* message);
int WP_App_ProcessPayload(WP_Message* message);

#endif // _WIREPROTOCOL_APP_INTERFACE_H_

