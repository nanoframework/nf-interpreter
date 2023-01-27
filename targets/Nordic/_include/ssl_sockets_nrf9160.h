//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef SSL_NRF9160_H
#define SSL_NRF9160_H

struct nrf91ssl_Context
{
    //SlNetSockSecAttrib_t* SecurityAttributes;
    uint32_t securityMethod;
    uint32_t peer_verify;
    int sec_tag;

    uint32_t SocketFd;
    bool IsServer;
};

#endif //SSL_NRF9160_H
