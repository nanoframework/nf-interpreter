//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_APP_INTERFACE_H_
#define _WIREPROTOCOL_APP_INTERFACE_H_

#include "WireProtocol.h"

//////////////////////////////////////////
extern uint8_t receptionBuffer[2048];
extern void ReplyToCommand(WP_Message* message, bool fSuccess, bool fCritical, void* ptr, int size);

//////////////////////////////////////////
bool WP_App_ProcessHeader(WP_Message* message);
bool WP_App_ProcessPayload(WP_Message* message);

#endif // _WIREPROTOCOL_APP_INTERFACE_H_

