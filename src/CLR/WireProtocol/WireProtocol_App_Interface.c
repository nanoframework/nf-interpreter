//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoWeak.h>
#include "WireProtocol_App_Interface.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// The functions below are the ones that need to be ported to client applications, 
// tipically nanoBooter and CLR debugger
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// provided as weak to be replaced by actual implementation in the client application
__nfweak int WP_App_ProcessHeader(WP_Message* message)
{
    (void)(message);

    // default to false
    return false;
}

// provided as weak to be replaced by actual implementation in the client application
__nfweak int WP_App_ProcessPayload(WP_Message* message)
{
    (void)(message);

    // default to false
    return false; 
}
