//
// Copyright (c) 2017 The nano Framework project contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_HAL_INTERFACE_H_
#define _WIREPROTOCOL_HAL_INTERFACE_H_

#include "WireProtocol.h"

//////////////////////////////////////////
bool WP_ReceiveBytes(uint8_t* ptr, uint16_t* size);
bool WP_TransmitMessage(WP_Message* message);
void WP_CheckAvailableIncomingData();

#endif // _WIREPROTOCOL_HAL_INTERFACE_H_
