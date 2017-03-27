//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_App_Interface.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// The functions bellow are the ones that need to be ported to client applications, 
// tipically nanoBooter and CLR debugger
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// provided as weak to be replaced by actual implementation in the client application
__nfweak bool WP_App_ProcessHeader(WP_Message* message)
{
    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation in the client application
__nfweak bool WP_App_ProcessPayload(WP_Message* message)
{
    // default to false
    return false; 
}

