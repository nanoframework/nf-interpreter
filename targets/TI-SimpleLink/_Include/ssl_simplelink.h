//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef _SSL_SIMPLELINK_H_
#define _SSL_SIMPLELINK_H_ 1

#include <ti/net/slnetutils.h>

struct SlSSL_Context
{
    SlNetSockSecAttrib_t* SecurityAttributes;
    uint32_t SocketFd;
    bool IsServer;
};

#endif //_SSL_SIMPLELINK_H_
