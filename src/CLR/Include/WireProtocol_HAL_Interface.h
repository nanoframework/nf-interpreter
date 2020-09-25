//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_HAL_INTERFACE_H_
#define _WIREPROTOCOL_HAL_INTERFACE_H_

#include "WireProtocol.h"

//////////////////////////////////////////
int WP_ReceiveBytes(uint8_t* ptr, unsigned short* size);
int WP_TransmitMessage(WP_Message* message);
void WP_CheckAvailableIncomingData();

#endif // _WIREPROTOCOL_HAL_INTERFACE_H_

