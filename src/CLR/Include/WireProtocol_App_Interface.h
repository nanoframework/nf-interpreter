//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _WIREPROTOCOL_APP_INTERFACE_H_
#define _WIREPROTOCOL_APP_INTERFACE_H_

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

#endif // _WIREPROTOCOL_APP_INTERFACE_H_
