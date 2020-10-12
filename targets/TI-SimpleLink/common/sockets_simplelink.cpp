//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Texas Instruments Incorporated.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <nanoHAL.h>
#include "simplelink_sockets.h"

#include <ti/drivers/net/wifi/slnetifwifi.h>
#include <ti/drivers/net/wifi/errors.h>
#include <ti/posix/gcc/errno.h>

//--//

// this is to store the return value of the calls to sockets APIs
int socketErrorCode;

static int MARSHAL_SOCK_FDSET_TO_SL_SDSET(SOCK_fd_set* sf, SlNetSock_SdSet_t* f)
{
    if(f != NULL && sf != NULL)
    {
        SlNetSock_sdsClrAll(f);

        for(unsigned int i=0; i < sf->fd_count; i++)
        {
            SlNetSock_sdsSet(sf->fd_array[i], f);
        }

        return sf->fd_count;
    }

    return 0;
}

static void MARSHAL_SL_SDSET_TO_SOCK_FDSET(SOCK_fd_set* sf, SlNetSock_SdSet_t* f)
{
    if(sf != NULL && f != NULL)
    {
        int cnt = sf->fd_count;
        sf->fd_count = 0;

        for(int i=0; i<cnt; i++)
        {
            if(SlNetSock_sdsIsSet(sf->fd_array[i], f))
            {
                sf->fd_array[sf->fd_count] = sf->fd_array[i];
                sf->fd_count++;
            }
        }
    }
}

int GetNativeTcpOption (int optname)
{
    int nativeOptionName = 0;

    switch(optname)
    {
        case SOCK_TCP_NODELAY:
            nativeOptionName = SLNETSOCK_TCP_NODELAY;
            break;

        // case SOCK_SOCKO_KEEPALIVE:
        //     nativeOptionName = TCP_KEEPALIVE;
        //     break;

        // allow the C# user to specify LWIP options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
    }
    return nativeOptionName;
}

int GetNativeSockOption (int optname)
{
    int nativeOptionName = 0;

    switch(optname)
    {
        case SOCK_SOCKO_DONTLINGER:
        case SOCK_SOCKO_LINGER:    
            nativeOptionName = SLNETSOCK_OPSOCK_LINGER;
            break;
        // case SOCK_SOCKO_SENDTIMEOUT:          
        //     nativeOptionName = SO_SNDTIMEO;
        //     break;
        case SOCK_SOCKO_RECEIVETIMEOUT:       
            nativeOptionName = SLNETSOCK_OPSOCK_RCV_TIMEO;
            break;
        // case SOCK_SOCKO_EXCLUSIVEADDRESSUSE: 
        // case SOCK_SOCKO_REUSEADDRESS:         
        //     nativeOptionName = SO_REUSEADDR;
        //     break;
        case SOCK_SOCKO_KEEPALIVE:  
            nativeOptionName = SLNETSOCK_OPSOCK_KEEPALIVE;
            break;
        case SOCK_SOCKO_ERROR:                  
            nativeOptionName = SLNETSOCK_OPSOCK_ERROR;
            break;
        case SOCK_SOCKO_BROADCAST:              
            nativeOptionName = SLNETSOCK_OPSOCK_BROADCAST;
            break;
        case SOCK_SOCKO_RECEIVEBUFFER:
            nativeOptionName =  SLNETSOCK_OPSOCK_RCV_BUF;
            break;
        // case SOCK_SOCKO_SENDBUFFER:
        //     nativeOptionName = SO_SNDBUF;
        //     break;
        // case SOCK_SOCKO_ACCEPTCONNECTION:
        //     nativeOptionName = SO_ACCEPTCONN;
        //     break;
        // case SOCK_SOCKO_USELOOPBACK:
        //     nativeOptionName = SO_USELOOPBACK;
        //     break;
        // case SOCK_SOCKO_DONTROUTE:  
        //     nativeOptionName = SO_DONTROUTE;
        //     break;
        // case SOCK_SOCKO_OUTOFBANDINLINE:
        //     nativeOptionName = SO_OOBINLINE;
        //     break;

        // case SOCK_SOCKO_DEBUG:
        //     nativeOptionName = SO_DEBUG;
        //     break;
            
        // case SOCK_SOCKO_SENDLOWWATER:
        //     nativeOptionName = SO_SNDLOWAT;
        //     break;
            
        // case SOCK_SOCKO_RECEIVELOWWATER:
        //     nativeOptionName = SO_RCVLOWAT;
        //     break;
            
 //        case SOCK_SOCKO_MAXCONNECTIONS:         //don't support
        // case SOCK_SOCKO_UPDATE_ACCEPT_CTX:
        // case SOCK_SOCKO_UPDATE_CONNECT_CTX:
        //     nativeOptionName = 0;
        //     break;
            
        // allow the C# user to specify LWIP options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
            
    }

    return nativeOptionName;
}

int GetNativeIPOption (int optname)
{
    int nativeOptionName = 0;

    switch(optname)
    {
        // case SOCK_IPO_TTL:           
        //     nativeOptionName = IP_TTL;
        //     break;
        // case SOCK_IPO_TOS:    
        //     nativeOptionName = IP_TOS;
        //     break;

        // case SOCK_IPO_MULTICAST_IF:
        //     nativeOptionName = IP_MULTICAST_IF;
        //     break;
        // case SOCK_IPO_MULTICAST_TTL:  
        //     nativeOptionName = SLNETSOCK_OPIP_MULTICAST_TTL;
        //     break;
        // case SOCK_IPO_MULTICAST_LOOP: 
        //     nativeOptionName = IP_MULTICAST_LOOP;
        //     break;
        case SOCK_IPO_ADD_MEMBERSHIP:
            nativeOptionName = SLNETSOCK_OPIP_ADD_MEMBERSHIP;
            break;
        case SOCK_IPO_DROP_MEMBERSHIP:
            nativeOptionName = SLNETSOCK_OPIP_DROP_MEMBERSHIP;
            break;

        // case SOCK_IPO_ADD_SOURCE_MEMBERSHIP:
        // case SOCK_IPO_DROP_SOURCE_MEMBERSHIP:
        // case SOCK_IPO_OPTIONS:
        // case SOCK_IPO_HDRINCL:
        // case SOCK_IPO_IP_DONTFRAGMENT:
        // case SOCK_IPO_BLOCK_SOURCE:
        // case SOCK_IPO_UBLOCK_SOURCE:
        // case SOCK_IPO_PACKET_INFO: 
        //     nativeOptionName = 0;
        //     break;

        // allow the C# user to specify LWIP options that our managed enum
        // doesn't support
        default:
            nativeOptionName = optname;
            break;
    }
    
    return nativeOptionName;
}

SOCK_SOCKET SOCK_socket( int family, int type, int protocol ) 
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_create(family, type, protocol, 0, 0);

    return socketErrorCode;
}

int SOCK_bind( SOCK_SOCKET socket, const struct SOCK_sockaddr* address, int addressLen  ) 
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_bind( socket, (const SlNetSock_Addr_t*)address, addressLen  );

    return socketErrorCode;
}

int SOCK_connect(SOCK_SOCKET socket, const struct SOCK_sockaddr* address, int addressLen) 
{ 
    NATIVE_PROFILE_PAL_COM();

    // Simple Link connect API requires it to be called over and over until connection is established (or fails!)
    // See chapter 6.7.1 of CC3X20 Programmer's Guide (doc SWRU455G).
    // Our current sockets implementation handles this as blocking connect, so we have to deal with it here
    // it's not optimal because it prevents the execution engine from performing task switching
    // this should be addressed at the caller level be implementing a timeout execution within the connect code.

    socketErrorCode = -1;

    // loop until socket is connected (or error is returned)
    while(socketErrorCode < 0)
    {
        socketErrorCode = SlNetSock_connect(socket, (const SlNetSock_Addr_t*)address, addressLen);
        
        if( socketErrorCode == SL_ERROR_BSD_EALREADY )
        {
            ClockP_usleep(10*1000);
            continue;
        }
        else if(socketErrorCode < 0)
        {
            sl_Close(socket);
        }
        break;
    }

    return socketErrorCode;
}

int SOCK_send(SOCK_SOCKET socket, const char* buf, int len, int flags) 
{ 
    NATIVE_PROFILE_PAL_COM();    

    socketErrorCode = SlNetSock_send(socket, buf, len, flags);

    return socketErrorCode;    
}

int SOCK_recv(SOCK_SOCKET socket, char* buf, int len, int flags)
{ 
    NATIVE_PROFILE_PAL_COM();
    
    // check for SOCKET_READ_PEEK_OPTION because Simple Link doesn't support this flag
    if(flags == SOCKET_READ_PEEK_OPTION)
    {
        socketErrorCode = SLNETERR_BSD_EINVAL;
    }
    else
    {
        socketErrorCode = SlNetSock_recv(socket, buf, len, flags);
    }

    return socketErrorCode;
}

int SOCK_close(SOCK_SOCKET socket)
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_close(socket);

    return socketErrorCode;
}

int SOCK_listen( SOCK_SOCKET socket, int backlog )
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_listen( socket, backlog );

    return socketErrorCode;
}

SOCK_SOCKET SOCK_accept( SOCK_SOCKET socket, struct SOCK_sockaddr* address, int* addressLen )
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_accept( socket, (SlNetSock_Addr_t*)address, (SlNetSocklen_t*)addressLen );

    return socketErrorCode;
}

int SOCK_shutdown( SOCK_SOCKET socket, int how )
{ 
    NATIVE_PROFILE_PAL_COM();
    
    socketErrorCode = SlNetSock_shutdown( socket, how );

    return socketErrorCode;
}

int SOCK_getaddrinfo( const char* nodename, char* servname, const struct SOCK_addrinfo* hints, struct SOCK_addrinfo** res )
{ 
    NATIVE_PROFILE_PAL_COM();

    SOCK_addrinfo *ai;
    SOCK_sockaddr_in *sa = NULL;
    int total_size = sizeof(SOCK_addrinfo) + sizeof(SOCK_sockaddr_in);
    struct SlNetUtil_addrInfo_t* addrInfo = NULL;

    if(res == NULL) return SOCK_SOCKET_ERROR;

    *res = NULL;

    // if the nodename == "" then return the IP address of this device
    if(nodename[0] == 0 && servname == NULL)
    {
        uint16_t configOptions;
        uint16_t ipLen = sizeof(SlNetCfgIpV4Args_t);
        SlNetCfgIpV4Args_t ipV4 = {0};
        
        if (sl_NetCfgGet(SL_NETCFG_IPV4_STA_ADDR_MODE, &configOptions, &ipLen, (uint8_t *)&ipV4) >= 0)
        {
            ai = (SOCK_addrinfo*)platform_malloc(total_size);

            if (ai != NULL) 
            {
                memset(ai, 0, total_size);
                sa = (SOCK_sockaddr_in*)((uint8_t*)ai + sizeof(SOCK_addrinfo));

                /* set up sockaddr */
            #if LWIP_IPV6
                // TODO
                //sa->sin_addr.S_un.S_addr = networkInterface->ip_addr.u_addr.ip4.addr;
            #else
                sa->sin_addr.S_un.S_addr = sl_Ntohl(ipV4.Ip);
            #endif

                sa->sin_family = SL_AF_INET;
                sa->sin_port = 0;
                
                // set up addrinfo
                ai->ai_family = SL_AF_INET;
                if (hints != NULL) 
                {
                    // copy socktype & protocol from hints if specified
                    ai->ai_socktype = hints->ai_socktype;
                    ai->ai_protocol = hints->ai_protocol;
                }

                ai->ai_addrlen = sizeof(SOCK_sockaddr_in);
                ai->ai_addr = (SOCK_sockaddr*)sa;

                *res = ai;

                return 0;      
            }      
        }
    }
    else
    {
        int err = SlNetUtil_getAddrInfo(0,  nodename, servname, (SlNetUtil_addrInfo_t*)hints, &addrInfo );
        socketErrorCode = err;

        if(err >= 0)
        {
            ///
            /// Marshal addrinfo data
            ///
            SlNetSock_AddrIn_t* sockaddr_in;
            
            ai = (SOCK_addrinfo*)platform_malloc(total_size);
            if (ai == NULL) 
            {
                SlNetUtil_freeAddrInfo(addrInfo);
            }
            else
            {
                memset(ai, 0, total_size);
                
                sockaddr_in = (SlNetSock_AddrIn_t*)addrInfo->ai_addr;

                sa = (SOCK_sockaddr_in*)((uint8_t*)ai + sizeof(SOCK_addrinfo));
                /* set up sockaddr */
                sa->sin_addr.S_un.S_addr = sockaddr_in->sin_addr.s_addr;
                sa->sin_family = sockaddr_in->sin_family;
                sa->sin_port = sockaddr_in->sin_port;
                
                /* set up addrinfo */
                ai->ai_family = addrInfo->ai_family;
                if (hints != NULL) 
                {
                    /* copy socktype & protocol from hints if specified */
                    ai->ai_socktype = hints->ai_socktype;
                    ai->ai_protocol = hints->ai_protocol;
                }
                
                ai->ai_addrlen = sizeof(SOCK_sockaddr_in);
                ai->ai_addr = (SOCK_sockaddr*)sa;
                
                *res = ai;

                // free marshalled addrinfo
                SlNetUtil_freeAddrInfo(addrInfo);

                return 0;
            }
        }
    }

    return SOCK_SOCKET_ERROR;
}

void SOCK_freeaddrinfo( struct SOCK_addrinfo* ai )
{ 
    NATIVE_PROFILE_PAL_COM();
    SlNetUtil_freeAddrInfo( (SlNetUtil_addrInfo_t*)ai );
}

int SOCK_ioctl( SOCK_SOCKET socket, int cmd, int* data )
{ 
    NATIVE_PROFILE_PAL_COM();

    if(cmd == SOCK_FIONBIO)
    {
        // adjust setting for socket blocking/non-blocking
        SlNetSock_Nonblocking_t enableOption;
        enableOption.nonBlockingEnabled = *data;

        socketErrorCode = SlNetSock_setOpt(socket, SLNETSOCK_LVL_SOCKET, SLNETSOCK_OPSOCK_NON_BLOCKING, (uint8_t *)&enableOption, sizeof(enableOption));

        return socketErrorCode;
    }
    else if(cmd == SOCK_IPPROTO_TCP)
    {
        // TODO
        HAL_AssertEx();
    }
    else if(cmd == SOCK_SOL_SOCKET)
    {
        // TODO
        HAL_AssertEx();
    }

    // unknown command 
    return -1;
}

int SOCK_getlasterror()
{ 
    NATIVE_PROFILE_PAL_COM();
    // FIXME
    return 0;//SlNetSock_getlasterror();
}

int SOCK_getsocklasterror(SOCK_SOCKET socket)
{ 
    NATIVE_PROFILE_PAL_COM();

    int errorNo;

    if(socketErrorCode >= 0)
    {
        return socketErrorCode;
    }

    /*  This switch case block is necessary for translating the SlNetSock error codes
     *  to BSD ones. The #ifdef in each case are made in order to reduce code footprint:
     *  This cases are compiled if and only if there's a discrepancy between the BSD error
     *  number and the error code returned by the SlNetSock layer.
     */
    switch(socketErrorCode)
    {
#if EBADF != SLNETERR_BSD_EBADF
    case SLNETERR_BSD_EBADF:
        errorNo = EBADF;
        break;
#endif
#if ENFILE != SLNETERR_BSD_ENSOCK
    case SLNETERR_BSD_ENSOCK:
        errorNo = ENFILE;
        break;
#endif
#if EAGAIN != SLNETERR_BSD_EAGAIN
    case SLNETERR_BSD_EAGAIN:
        errorNo = EAGAIN;
        break;
#endif
#if ENOMEM != SLNETERR_BSD_ENOMEM
    case SLNETERR_BSD_ENOMEM:
        errorNo = ENOMEM;
        break;
#endif
#if EACCES != SLNETERR_BSD_EACCES
    case SLNETERR_BSD_EACCES:
        errorNo = EACCES;
        break;
#endif
#if EFAULT != SLNETERR_BSD_EFAULT
    case SLNETERR_BSD_EFAULT:
        errorNo = EFAULT;
        break;
#endif
#if EINVAL != SLNETERR_BSD_EINVAL
    case SLNETERR_BSD_EINVAL:
        errorNo = EINVAL;
        break;
#endif
#if EDESTADDRREQ != SLNETERR_BSD_EDESTADDRREQ
    case SLNETERR_BSD_EDESTADDRREQ:
        errorNo = EDESTADDRREQ;
        break;
#endif
#if EPROTOTYPE != SLNETERR_BSD_EPROTOTYPE
    case SLNETERR_BSD_EPROTOTYPE:
        errorNo = EPROTOTYPE;
        break;
#endif
#if ENOPROTOOPT != SLNETERR_BSD_ENOPROTOOPT
    case SLNETERR_BSD_ENOPROTOOPT:
        errorNo = ENOPROTOOPT;
        break;
#endif
#if EPROTONOSUPPORT != SLNETERR_BSD_EPROTONOSUPPORT
    case SLNETERR_BSD_EPROTONOSUPPORT:
        errorNo = EPROTONOSUPPORT;
        break;
#endif
#if EOPNOTSUPP != SLNETERR_BSD_EOPNOTSUPP
    case SLNETERR_BSD_EOPNOTSUPP:
        errorNo = EOPNOTSUPP;
        break;
#endif
#if EAFNOSUPPORT != SLNETERR_BSD_EAFNOSUPPORT
    case SLNETERR_BSD_EAFNOSUPPORT:
        errorNo = EAFNOSUPPORT;
        break;
#endif
#if EADDRINUSE != SLNETERR_BSD_EADDRINUSE
    case SLNETERR_BSD_EADDRINUSE:
        errorNo = EADDRINUSE;
        break;
#endif
#if EADDRNOTAVAIL != SLNETERR_BSD_EADDRNOTAVAIL
    case SLNETERR_BSD_EADDRNOTAVAIL:
        errorNo = EADDRNOTAVAIL;
        break;
#endif
#if ENETDOWN != SLNETERR_BSD_ENETDOWN
    case SLNETERR_BSD_ENETDOWN:
        errorNo = ENETDOWN;
        break;
#endif
#if ENETUNREACH != SLNETERR_BSD_ENETUNREACH
    case SLNETERR_BSD_ENETUNREACH:
        errorNo = ENETUNREACH;
        break;
#endif
#if ENOBUFS != SLNETERR_BSD_ENOBUFS
    case SLNETERR_BSD_ENOBUFS:
        errorNo = ENOBUFS;
        break;
#endif
#if EISCONN != SLNETERR_BSD_EISCONN
    case SLNETERR_BSD_EISCONN:
        errorNo = EISCONN;
        break;
#endif
#if ENOTCONN != SLNETERR_BSD_ENOTCONN
    case SLNETERR_BSD_ENOTCONN:
        errorNo = ENOTCONN;
        break;
#endif
#if ETIMEDOUT != SLNETERR_BSD_ETIMEDOUT
    case SLNETERR_BSD_ETIMEDOUT:
        errorNo = ETIMEDOUT;
        break;
#endif
#if ECONNREFUSED != SLNETERR_BSD_ECONNREFUSED
    case SLNETERR_BSD_ECONNREFUSED:
        errorNo = ECONNREFUSED;
        break;
#endif
    case SL_ERROR_BSD_EALREADY:
        errorNo = SOCK_EWOULDBLOCK;
        break;

    /* The cases below are propriety driver errors, which can
     * be returned by the SimpleLink Driver, in various cases of failure.
     * Each is mapped to the corresponding BSD error.
     */
    case SLNETERR_POOL_IS_EMPTY:
    case SLNETERR_RET_CODE_NO_FREE_SPACE:
    case SLNETERR_RET_CODE_MUTEX_CREATION_FAILED:
    case SLNETERR_RET_CODE_MALLOC_ERROR:
        errorNo = ENOMEM;
        break;
    case SLNETERR_RET_CODE_INVALID_INPUT:
    case SLNETERR_EZEROLEN:
    case SLNETERR_ESMALLBUF:
    case SLNETERR_INVALPARAM:
        errorNo = EINVAL;
        break;
    case SLNETERR_RET_CODE_DOESNT_SUPPORT_NON_MANDATORY_FXN:
        errorNo = EOPNOTSUPP;
        break;
    case SLNETERR_RET_CODE_COULDNT_FIND_RESOURCE:
        errorNo = EBADF;
        break;
    default:
     /* Do nothing ..
      * If no case is true, that means that the BSD error
      * code and the code returned by the NWP are either identical,
      * or no propriety error has occurred.
      */
        break;
    }
    
    return errorNo;
}

int SOCK_select( int nfds, SOCK_fd_set* readfds, SOCK_fd_set* writefds, SOCK_fd_set* exceptfds, const struct SOCK_timeval* timeout )
{ 
    NATIVE_PROFILE_PAL_COM();

    int ret = 0;
    uint32_t networkInterfaceID;

    // If the network goes down then we should alert any pending socket actions
    if(exceptfds != NULL && exceptfds->fd_count > 0)
    {
        // find the network interface for this socket
        // the socket handle is "burried" inside the exceptfds struct (see the caller code in Helper__SelectSocket)
        networkInterfaceID = SlNetSock_getIfID(exceptfds->fd_array[0]);
        if ( networkInterfaceID == SLNETERR_RET_CODE_INVALID_INPUT )
        {
            socketErrorCode = ENOTSOCK;

            return SOCK_SOCKET_ERROR;
        }

        if ( SlNetIf_getConnectionStatus(networkInterfaceID) == SLNETIF_STATUS_DISCONNECTED )
        {
            if(readfds  != NULL)
            {
                readfds->fd_count = 0;
            }
            if(writefds != NULL)
            {
                writefds->fd_count = 0;
            }

            socketErrorCode = ENETDOWN;

            return exceptfds->fd_count;
        }
    }

    // developer note:
    // The original code, being lwIP based, uses the convention that 0 is infinite timeout
    // Because SimpleLink infinite timeout is negative or NULL we need to translate it.
    SlNetSock_Timeval_t timeoutCopy;
    bool isInfiniteTimeout = false;

    if(timeout->tv_sec > 0 || timeout->tv_usec > 0)
    {
        timeoutCopy.tv_sec = timeout->tv_sec;
        timeoutCopy.tv_usec = timeout->tv_usec;
    }
    else
    {
        isInfiniteTimeout = true;
    }

    ret = SlNetSock_select( 
        SLNETSOCK_MAX_CONCURRENT_SOCKETS, 
        (SlNetSock_SdSet_t*)readfds, 
        (SlNetSock_SdSet_t*)writefds, 
        (SlNetSock_SdSet_t*)exceptfds, 
        isInfiniteTimeout ? NULL : &timeoutCopy );

    socketErrorCode = ret;

    // developer notes:
    // SlNetSock_select() call doesn't touch the exception SDSET.
    // In SimpleLink socket exceptions are handled differently.
    // We need to clear that one here because the way the caller is implemented it will think there is an exception 
    // because it has forced it.
    SlNetSock_sdsClrAll((SlNetSock_SdSet_t*)exceptfds);

    return ret;
}

int SOCK_setsockopt( SOCK_SOCKET socket, int level, int optname, const char* optval, int  optlen )
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_setOpt( socket, level, optname, (void *) optval, optlen );

    return socketErrorCode;
}

int SOCK_getsockopt( SOCK_SOCKET socket, int level, int optname, char* optval, int* optlen )
{ 
    NATIVE_PROFILE_PAL_COM();

    int         nativeLevel;
    int         nativeOptionName;
    char*       pNativeOptval = optval;
    int         ret;
    
    switch(level)
    {
        case SOCK_IPPROTO_IP:
            nativeLevel         = SLNETSOCK_LVL_PHY;
            nativeOptionName    = GetNativeIPOption(optname);
            break;
        case SOCK_IPPROTO_TCP:    
            nativeLevel         = SLNETSOCK_PROTO_TCP;
            nativeOptionName    = GetNativeTcpOption(optname);
            break;
        case SOCK_IPPROTO_UDP: 
        case SOCK_IPPROTO_ICMP:
        case SOCK_IPPROTO_IGMP:
        case SOCK_IPPROTO_IPV4:
        case SOCK_SOL_SOCKET:
            nativeLevel         = SLNETSOCK_LVL_SOCKET;
            nativeOptionName    = GetNativeSockOption(optname);
            break;
        default:
            nativeLevel         = level;
            nativeOptionName    = optname;
            break;
    }

    socketErrorCode = SlNetSock_getOpt( socket, nativeLevel, nativeOptionName, (void *)pNativeOptval, (SlNetSocklen_t *)optlen );

    if(socketErrorCode == 0)
    {
        switch(level)
        {
            case SOCK_SOL_SOCKET:
                switch(optname)
                {       
                    case SOCK_SOCKO_EXCLUSIVEADDRESSUSE:
                    case SOCK_SOCKO_DONTLINGER:
                        *optval = !(*(int*)optval != 0);
                        break;
                        
                    case SOCK_SOCKO_ACCEPTCONNECTION:
                    case SOCK_SOCKO_BROADCAST:
                    case SOCK_SOCKO_KEEPALIVE:
                        *optval = (*(int*)optval != 0);
                        break;
                }
                break;
        }
    }

    return socketErrorCode;
}

int SOCK_getpeername( SOCK_SOCKET socket, struct SOCK_sockaddr* name, int* namelen )
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_getPeerName( socket, (SlNetSock_Addr_t*)name, (SlNetSocklen_t*)namelen );

    return socketErrorCode;
}

int SOCK_getsockname( SOCK_SOCKET socket, struct SOCK_sockaddr* name, int* namelen )
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_getSockName( socket, (SlNetSock_Addr_t*)name, (SlNetSocklen_t*)namelen );

    return socketErrorCode;
}

int SOCK_recvfrom( SOCK_SOCKET s, char* buf, int len, int flags, struct SOCK_sockaddr* from, int* fromlen )
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_recvFrom( s, buf, len, flags, (SlNetSock_Addr_t*)from, (SlSocklen_t*)fromlen );

    return socketErrorCode;
}

int SOCK_sendto( SOCK_SOCKET s, const char* buf, int len, int flags, const struct SOCK_sockaddr* to, int tolen )
{ 
    NATIVE_PROFILE_PAL_COM();

    socketErrorCode = SlNetSock_sendTo( s, buf, len, flags, (SlNetSock_Addr_t*)to, (SlSocklen_t)tolen );

    return socketErrorCode;
}

bool Network_Initialize()
{
    NATIVE_PROFILE_PAL_COM();

    // call our equivalent to lwIP driver
    return SimpleLink_SOCKETS_Initialize();

    // network initialization is taken care of by SimpleLink
    return true;
}

bool Network_Uninitialize()
{
    NATIVE_PROFILE_PAL_COM();

    // not required SimpleLink
    return true;
}

void SOCKETS_CloseConnections()
{
    NATIVE_PROFILE_PAL_COM();
    // FIXME
    //Sockets_SimpleLink_Driver::CloseConnections(FALSE);
}

HRESULT SOCK_CONFIGURATION_LoadAdapterConfiguration(HAL_Configuration_NetworkInterface* config, uint32_t interfaceIndex)
{
    NATIVE_PROFILE_PAL_COM();
    return HAL_SOCK_CONFIGURATION_LoadAdapterConfiguration(config, interfaceIndex);
}

HRESULT SOCK_CONFIGURATION_UpdateAdapterConfiguration(HAL_Configuration_NetworkInterface* config, uint32_t interfaceIndex, uint32_t updateFlags)
{
    NATIVE_PROFILE_PAL_COM();
    HRESULT hr = S_OK;
    bool success = FALSE;

    const uint32_t c_reInitFlag =   NetworkInterface_UpdateOperation_Dhcp      | 
                                    NetworkInterface_UpdateOperation_DhcpRenew | 
                                    NetworkInterface_UpdateOperation_Mac;

    const uint32_t c_uninitFlag = c_reInitFlag | NetworkInterface_UpdateOperation_DhcpRelease;

    if(0 != (updateFlags & c_uninitFlag))
    {
        success = SOCKETS_DbgUninitialize(COM_SOCKET_DBG);
    }

    hr = HAL_SOCK_CONFIGURATION_UpdateAdapterConfiguration(config, interfaceIndex, updateFlags);

    if(!SUCCEEDED(hr))
    {
        return S_FALSE;
    }

    if(0 != (updateFlags & c_reInitFlag))
    {
        if(success) SOCKETS_DbgInitialize(COM_SOCKET_DBG);
    }

    return hr;
}

HRESULT SOCK_CONFIGURATION_LoadConfiguration(HAL_Configuration_NetworkInterface* config, uint32_t interfaceIndex)
{
    NATIVE_PROFILE_PAL_COM();
    HRESULT hr = S_OK;

    // load current DCHP settings
    hr = SOCK_CONFIGURATION_LoadAdapterConfiguration(config, interfaceIndex);

    return hr;
}

bool  SOCKETS_DbgInitialize( int ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();

    return true;
}

bool  SOCKETS_DbgUninitialize( int ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();

    return true;
}
