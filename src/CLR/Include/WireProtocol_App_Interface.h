//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef WIREPROTOCOL_APP_INTERFACE_H
#define WIREPROTOCOL_APP_INTERFACE_H

#include "WireProtocol.h"

//////////////////////////////////////////
extern void ReplyToCommand(WP_Message *message, int fSuccess, int fCritical, void *ptr, int size);

extern const CommandHandlerLookup c_Lookup_Request[];
extern const CommandHandlerLookup c_Lookup_Reply[];

//////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif

    uint8_t WP_App_ProcessHeader(WP_Message *message);
    uint8_t WP_App_ProcessPayload(WP_Message *message);
    uint8_t Messaging_ProcessPayload(WP_Message *message);

    /////////////////////////////////////
    // these two helper functions need to be implemented at platform level (WireProtocol_App_Interface.c)
    uint8_t GetSizeOfLookup_Reply();
    uint8_t GetSizeOfLookup_Request();
    ////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif // WIREPROTOCOL_APP_INTERFACE_H
