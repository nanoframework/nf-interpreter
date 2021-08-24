//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SSL_SIMPLELINK_H
#define SSL_SIMPLELINK_H

#include <ti/net/slnetutils.h>

struct SlSSL_Context
{
    SlNetSockSecAttrib_t* SecurityAttributes;
    uint32_t SocketFd;
    bool IsServer;
};

#endif //SSL_SIMPLELINK_H
