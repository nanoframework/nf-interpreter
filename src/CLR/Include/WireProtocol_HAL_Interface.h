//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_HAL_INTERFACE_H_
#define _WIREPROTOCOL_HAL_INTERFACE_H_

#include "WireProtocol.h"

#if __cplusplus
extern "C"
{
#endif

//////////////////////////////////////////

///
/// @brief Receives n bytes from the Wire Protocol channel.
///
/// @param ptr Pointer to the buffer that will hold the received bytes.
/// @param size Number of bytes to read. On return it will have the number of bytes actually received.
/// @return bool true if any bytes where received, false otherwise.
///
uint8_t WP_ReceiveBytes(uint8_t *ptr, uint32_t *size);

///
/// @brief Sends a message through the Wire Protocol channel.
///
/// @param message Message to send
/// @return bool true for transmition succesfull, false otherwise.
///
uint8_t WP_TransmitMessage(WP_Message *message);

void WP_CheckAvailableIncomingData();

#if __cplusplus
}
#endif

#endif // _WIREPROTOCOL_HAL_INTERFACE_H_
