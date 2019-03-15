//
// Copyright (c) 2018 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include <nanoPAL_COM.h>

__nfweak bool  Network_Initialize()
{ 
    return true;
};

__nfweak bool  Network_Uninitialize()
{
    return true;
};

__nfweak void SOCKETS_CloseConnections()
{
    NATIVE_PROFILE_PAL_COM();
}
