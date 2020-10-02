//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sockets_lwip.h"
#include <lwIP_Sockets.h>

HAL_COMPLETION Sockets_LWIP_Driver::s_DebuggerTimeoutCompletion;

static HAL_CONTINUATION MulticastResponseContinuation;

int g_DebuggerPort_SslCtx_Handle = -1;

//
//  Methods used by the Network Debugger 
//

bool  SOCKETS_DbgInitialize( int ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();

    return Sockets_LWIP_Driver::InitializeDbgListener( ComPortNum );
}

bool  SOCKETS_DbgUninitialize( int ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_LWIP_Driver::UninitializeDbgListener( ComPortNum );
}

int32_t SOCKETS_Write( int32_t ComPortNum, const char* Data, size_t size )
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_LWIP_Driver::Write( ComPortNum, Data, size );
}

int32_t SOCKETS_Read( int32_t ComPortNum, char* Data, size_t size )
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_LWIP_Driver::Read( ComPortNum, Data, size );
}

bool SOCKETS_Flush( int32_t ComPortNum )
{
    (void)ComPortNum;

    NATIVE_PROFILE_PAL_COM();
    //Events_WaitForEvents(0, 2);
    return TRUE;
}

bool SOCKETS_UpgradeToSsl( int32_t ComPortNum, const int8_t* pCACert, uint32_t caCertLen, const int8_t* pDeviceCert, uint32_t deviceCertLen, LPCSTR szTargetHost )
{
     NATIVE_PROFILE_PAL_COM();
     return Sockets_LWIP_Driver::UpgradeToSsl( ComPortNum, pCACert, caCertLen, pDeviceCert, deviceCertLen, szTargetHost );
}

bool SOCKETS_IsUsingSsl( int32_t ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();
    return Sockets_LWIP_Driver::IsUsingSsl( ComPortNum );
}

void Sockets_LWIP_Driver::Debugger_Initialize()
{
    if(!s_initializedDbg)
    {
        memset(&MulticastResponseContinuation, 0, sizeof(MulticastResponseContinuation));
        memset(&s_DebuggerTimeoutCompletion, 0, sizeof(s_DebuggerTimeoutCompletion));

        g_Sockets_LWIP_Driver.m_multicastSocket     = SOCK_SOCKET_ERROR;
        g_Sockets_LWIP_Driver.m_SocketDebugStream   = SOCK_SOCKET_ERROR;
        g_Sockets_LWIP_Driver.m_SocketDebugListener = SOCK_SOCKET_ERROR;

        g_Sockets_LWIP_Driver.m_stateDebugSocket = DbgSock_Uninitialized;

        g_Sockets_LWIP_Driver.m_usingSSL = FALSE;

        s_initializedDbg = TRUE;
    }
}


void Sockets_LWIP_Driver::Debugger_Uninitialize()
{
    g_Sockets_LWIP_Driver.m_multicastSocket     = SOCK_SOCKET_ERROR;
    g_Sockets_LWIP_Driver.m_SocketDebugStream   = SOCK_SOCKET_ERROR;
    g_Sockets_LWIP_Driver.m_SocketDebugListener = SOCK_SOCKET_ERROR;
    s_initializedDbg = FALSE;
}

void Sockets_LWIP_Driver::Debugger_Abort()
{
    MulticastResponseContinuation.Abort();
    s_DebuggerTimeoutCompletion.Abort();
        
    g_Sockets_LWIP_Driver.m_stateDebugSocket = DbgSock_Uninitialized;
}


bool Sockets_LWIP_Driver::InitializeDbgListener( int ComPortNum )
{   
    NATIVE_PROFILE_PAL_COM();
    SOCK_sockaddr_in sockAddr;
    int nonblocking = 1;
    int32_t optval = 1;
    int32_t optLinger = 0;
    void *dummyPtr;
    SOCKET_CHECK_ENTER();

    Debugger_Initialize();

    if(ComPortNum != ConvertCOM_SockPort(COM_SOCKET_DBG)) return FALSE;

    if(g_Sockets_LWIP_Driver.m_SocketDebugListener != SOCK_SOCKET_ERROR) return TRUE;

    s_DebuggerTimeoutCompletion.InitializeForUserMode(OnDebuggerTimeout);

    //-- debug api socket --//

    g_Sockets_LWIP_Driver.m_SocketDebugListener = Socket( SOCK_AF_INET, SOCK_SOCK_STREAM, SOCK_IPPROTO_TCP, TRUE );
    SOCKET_CHECK_RESULT( g_Sockets_LWIP_Driver.m_SocketDebugListener );

    memset( &sockAddr, 0, sizeof(sockAddr) );
    sockAddr.sin_family             = SOCK_AF_INET;
    sockAddr.sin_port               = SOCK_htons(DEBUG_SOCKET_PORT);
    sockAddr.sin_addr.S_un.S_addr   = SOCK_htonl(SOCK_INADDR_ANY);

    SOCKET_CHECK_RESULT( SOCK_ioctl(g_Sockets_LWIP_Driver.m_SocketDebugListener, SOCK_FIONBIO, &nonblocking) );

    SOCKET_CHECK_RESULT( HAL_SOCK_setsockopt( g_Sockets_LWIP_Driver.m_SocketDebugListener, SOCK_IPPROTO_TCP, SOCK_TCP_NODELAY, (char*)&optval, sizeof(optval) ) );

    SOCKET_CHECK_RESULT( HAL_SOCK_setsockopt(g_Sockets_LWIP_Driver.m_SocketDebugListener, SOCK_SOL_SOCKET, SOCK_SOCKO_LINGER, (const char*)&optLinger, sizeof(int32_t)) );

    // need this to keep the compiler happy about the cast to SOCK_sockaddr
    // which is intended and perfectly safe
    dummyPtr = &sockAddr;

    SOCKET_CHECK_RESULT( 
        SOCK_bind( 
            g_Sockets_LWIP_Driver.m_SocketDebugListener,
            (const SOCK_sockaddr*)dummyPtr,
            sizeof(sockAddr)) 
    );

    SOCKET_CHECK_RESULT( SOCK_listen( g_Sockets_LWIP_Driver.m_SocketDebugListener, 1 ) );

    g_Sockets_LWIP_Driver.m_stateDebugSocket = DbgSock_Listening;

    InitializeMulticastDiscovery();
	MulticastDiscoverySchedule();

    SOCKET_CLEANUP();

    if(g_Sockets_LWIP_Driver.m_SocketDebugListener != SOCK_SOCKET_ERROR)
    {
        SOCK_close(g_Sockets_LWIP_Driver.m_SocketDebugListener);
        g_Sockets_LWIP_Driver.m_SocketDebugListener = SOCK_SOCKET_ERROR;

        debug_printf("DBGLISTENER SOCKET_ERROR: %d\r\n", HAL_SOCK_getlasterror() );
    }

    SOCKET_CHECK_EXIT_bool_CLEANUP();
}

bool Sockets_LWIP_Driver::UninitializeDbgListener( int ComPortNum )
{
    NATIVE_PROFILE_PAL_COM();

    if(ComPortNum != ConvertCOM_SockPort(COM_SOCKET_DBG)) return FALSE;

    g_Sockets_LWIP_Driver.m_stateDebugSocket = DbgSock_Uninitialized;

    CloseDebuggerSocket();

    if(g_Sockets_LWIP_Driver.m_SocketDebugListener != SOCK_SOCKET_ERROR)
    {
        SOCK_close( g_Sockets_LWIP_Driver.m_SocketDebugListener );
        
        g_Sockets_LWIP_Driver.m_SocketDebugListener = SOCK_SOCKET_ERROR;
    }

    return TRUE;
}

//-----------------------------------------------------------------------------
//
//  Write - The Write method will write data to the debugger stream socket (if a connection is active).
//  In addition if the write fails for a reason other than EWOULDBLOCK then we should shutdown
//  the debugger stream socket and change to the listening state
//
//-----------------------------------------------------------------------------
int Sockets_LWIP_Driver::Write( int ComPortNum, const char* Data, size_t size )
{
    NATIVE_PROFILE_PAL_COM();
    int32_t ret;

    if(ComPortNum != ConvertCOM_SockPort(COM_SOCKET_DBG)) return SOCK_SOCKET_ERROR;

    if((g_Sockets_LWIP_Driver.m_stateDebugSocket  != DbgSock_Connected) ||
       (g_Sockets_LWIP_Driver.m_SocketDebugStream == SOCK_SOCKET_ERROR)) 
    {
        return SOCK_SOCKET_ERROR;
    }

    if(g_Sockets_LWIP_Driver.m_usingSSL)
    {
        ret = SSL_Write(g_Sockets_LWIP_Driver.m_SocketDebugStream, Data, size);
    }
    else
    {
        ret = SOCK_send( g_Sockets_LWIP_Driver.m_SocketDebugStream, Data, size, 0 );
    }

    if(ret < 0) 
    {
        int32_t err = SOCK_getlasterror();

        // debugger stream is no longer active, change to listening state
        if(err != SOCK_EWOULDBLOCK)
        {
            CloseDebuggerSocket();
        }
        else
        {   
            ret = 0;
        }
    }

    return ret;
}

//-----------------------------------------------------------------------------
//
//  Read - the Read method performs two duties: first to read data from the debug stream; and 
//  second to manage the debugger connection state.  The initial state is to be listening for debug
//  connections, and therefore each read performs an accept to see if there is a pending connection.
//  Once a connection is made the state changes to connected and the debugger stream socket 
//  is read for each Read call.  During the Connected state an Accept is still called so that no other 
//  connections will be handled.  If the debugger is still receiving data on the debugger stream, then
//  new connections will be closed immediately.  If the debugger stream socket has been closed then
//  the state return to the listening state (unless there is an pending connection in which case the 
//  pending connection becomes the new debugger stream).
//
//-----------------------------------------------------------------------------
int Sockets_LWIP_Driver::Read( int ComPortNum, char* Data, size_t size )
{
    NATIVE_PROFILE_PAL_COM();
    SOCK_SOCKET sock;
    SOCK_sockaddr addr;
    int len;
    int32_t ret = 0;
    SOCK_timeval timeout;
    SOCK_fd_set  readSet;

    if(ComPortNum != ConvertCOM_SockPort(COM_SOCKET_DBG)) return 0;

    if(g_Sockets_LWIP_Driver.m_stateDebugSocket == DbgSock_Uninitialized) return 0;

    memset(&addr, 0, sizeof(addr));

    timeout.tv_sec  = 0;
    timeout.tv_usec = 0;

    readSet.fd_array[0] = g_Sockets_LWIP_Driver.m_SocketDebugListener;
    readSet.fd_count = 1;

    // if we are connected, then read from the debug stream
    if(g_Sockets_LWIP_Driver.m_stateDebugSocket == DbgSock_Connected)
    {
        if(g_Sockets_LWIP_Driver.m_usingSSL)
        {
            ret = SSL_Read(g_Sockets_LWIP_Driver.m_SocketDebugStream, Data, size);
        }
        else
        {
            ret = SOCK_recv(g_Sockets_LWIP_Driver.m_SocketDebugStream, Data, size, 0);
        }

        // return value of zero indicates a shutdown of the socket.  Also we shutdown for any error except
        // ewouldblock.  If either of these happens, then we go back to the listening state
        if((ret == 0) || (ret == SOCK_SOCKET_ERROR && SOCK_getlasterror() != SOCK_EWOULDBLOCK))
        {
            CloseDebuggerSocket();
        }
        else if(ret != SOCK_SOCKET_ERROR && s_DebuggerTimeoutCompletion.IsLinked())
        {
            // we either got data or the socket was closed, so kill the timeout
            s_DebuggerTimeoutCompletion.Abort();
        }
    }

    if(SOCK_SOCKET_ERROR != HAL_SOCK_select( SOCK_FD_SETSIZE, &readSet, NULL, NULL, &timeout))
    {
        // we always perform an accept so that we handle pending connections
        // if we already are connected and the debug stream socket is still active, then we immediately close
        // the pending connection
        sock = Accept( g_Sockets_LWIP_Driver.m_SocketDebugListener, &addr, &len, TRUE );

        if(SOCK_SOCKET_ERROR != sock)
        {
            int nonblocking = 1;
            bool  optval      = 1;
            int32_t optLinger   = 0;

            // if we are already in the connected state, then verify that the debugger stream socket is still active
            if(DbgSock_Connected == g_Sockets_LWIP_Driver.m_stateDebugSocket)
            {
                // the debugger stream is still active, so shutdown the pending connection
                HAL_SOCK_setsockopt( sock, SOCK_SOL_SOCKET, SOCK_SOCKO_LINGER, (const char*)&optLinger, sizeof(int32_t) );
                SOCK_close(sock);

                // set timeout since another connection is trying to use us.
                if(!s_DebuggerTimeoutCompletion.IsLinked())
                {
                    s_DebuggerTimeoutCompletion.EnqueueDelta(5000000); // 5 seconds
                }
            }
            else // we are in the listening state, so accept the pending connection and update the state
            {
                g_Sockets_LWIP_Driver.m_SocketDebugStream = sock;
                SOCKET_CHECK_RESULT( SOCK_ioctl(g_Sockets_LWIP_Driver.m_SocketDebugStream, SOCK_FIONBIO, &nonblocking) );
                SOCKET_CHECK_RESULT( HAL_SOCK_setsockopt( g_Sockets_LWIP_Driver.m_SocketDebugStream, SOCK_IPPROTO_TCP, SOCK_TCP_NODELAY, (char*)&optval, sizeof(optval) ) );
                SOCKET_CHECK_RESULT( HAL_SOCK_setsockopt( g_Sockets_LWIP_Driver.m_SocketDebugStream, SOCK_SOL_SOCKET, SOCK_SOCKO_LINGER, (const char*)&optLinger, sizeof(int32_t) ) );

                g_Sockets_LWIP_Driver.m_stateDebugSocket = DbgSock_Connected;

                SOCKET_CLEANUP()

                if(g_Sockets_LWIP_Driver.m_SocketDebugStream != SOCK_SOCKET_ERROR)
                {
                    SOCK_close(g_Sockets_LWIP_Driver.m_SocketDebugStream);
                    g_Sockets_LWIP_Driver.m_SocketDebugStream = SOCK_SOCKET_ERROR;

                    debug_printf("DBGSTREAM SOCKET_ERROR: %d\r\n", HAL_SOCK_getlasterror() );
                }

                SOCKET_CHECK_EXIT_NORETURN();
            }
            
            HAL_SOCK_EventsSet( SOCKET_EVENT_FLAG_SOCKET );
        }
    }

    if(ret < 0)
    {  
        ret = 0;
    }

    return ret;
}

void Sockets_LWIP_Driver::CloseDebuggerSocket()
{
    if(g_Sockets_LWIP_Driver.m_SocketDebugStream != SOCK_SOCKET_ERROR)
    {
        if(g_Sockets_LWIP_Driver.m_usingSSL)
        {
            SSL_CloseSocket( g_Sockets_LWIP_Driver.m_SocketDebugStream );
        }
        else
        {
            SOCK_close( g_Sockets_LWIP_Driver.m_SocketDebugStream );
        }

        g_Sockets_LWIP_Driver.m_usingSSL = FALSE;
        
        g_Sockets_LWIP_Driver.m_SocketDebugStream = SOCK_SOCKET_ERROR;

        g_Sockets_LWIP_Driver.m_stateDebugSocket = DbgSock_Listening;

        s_DebuggerTimeoutCompletion.Abort();
    }
}

void Sockets_LWIP_Driver::OnDebuggerTimeout(void* arg)
{
    (void)arg;

    CloseDebuggerSocket();
}

bool Sockets_LWIP_Driver::IsUsingSsl( int ComPortNum )
{
    if(ComPortNum != ConvertCOM_SockPort(COM_SOCKET_DBG)) return FALSE;
    if(g_Sockets_LWIP_Driver.m_stateDebugSocket == DbgSock_Uninitialized) return FALSE;

    if(g_Sockets_LWIP_Driver.m_stateDebugSocket == DbgSock_Connected)
    {
        return g_Sockets_LWIP_Driver.m_usingSSL;
    }

    return FALSE;
}

bool Sockets_LWIP_Driver::UpgradeToSsl( int ComPortNum, const int8_t* pCACert, uint32_t caCertLen, const int8_t* pDeviceCert, uint32_t deviceCertLen, const char* szTargetHost  )
{
    if(ComPortNum != ConvertCOM_SockPort(COM_SOCKET_DBG)) return 0;
    if(g_Sockets_LWIP_Driver.m_stateDebugSocket == DbgSock_Uninitialized) return 0;
    
    if(g_Sockets_LWIP_Driver.m_stateDebugSocket == DbgSock_Connected)
    {
        if(g_Sockets_LWIP_Driver.m_usingSSL) return TRUE;

        // TLS only and Verify=Required --> only verify the server
        if(SSL_ClientInit( 0x10, 0x04, (const char*)pDeviceCert, deviceCertLen, NULL, 0 , NULL, 0, g_DebuggerPort_SslCtx_Handle ))
        {
            int32_t ret;
    
            SSL_AddCertificateAuthority( g_DebuggerPort_SslCtx_Handle, (const char*)pCACert, caCertLen, NULL);
    
            do
            {
                ret = SSL_Connect( g_Sockets_LWIP_Driver.m_SocketDebugStream, szTargetHost, g_DebuggerPort_SslCtx_Handle );
            }
            while(ret == SOCK_EWOULDBLOCK || ret == SOCK_TRY_AGAIN);
    
    
            if(ret != 0)
            {
                SSL_CloseSocket(g_Sockets_LWIP_Driver.m_SocketDebugStream);
                SSL_ExitContext(g_DebuggerPort_SslCtx_Handle);
            }
            else
            {
                g_Sockets_LWIP_Driver.m_usingSSL = TRUE;
            }
    
            return ret == 0;
        }
    }

    return FALSE;
}

//-----------------------------------------------------------------------------
//
//  InitializeMulticastDiscovery - Initialize the NETMF discovery service for
//  sockets.  We use multicast sockets to create the discovery mechanism.   
//
//-----------------------------------------------------------------------------
bool Sockets_LWIP_Driver::InitializeMulticastDiscovery()
{
    NATIVE_PROFILE_PAL_COM();
    SOCKET_CHECK_ENTER(); 
    SOCK_sockaddr_in sockAddr;
    void *dummyPtr;
    int nonblocking = 1;

    if(g_Sockets_LWIP_Driver.s_discoveryInitialized) return TRUE;

    MulticastResponseContinuation.InitializeCallback(MulticastDiscoveryRespond, NULL);

    // set up discovery socket to list to defined discovery port for any ip address
    memset( &sockAddr, 0, sizeof(sockAddr) );
    sockAddr.sin_family           = SOCK_AF_INET;
    sockAddr.sin_port             = SOCK_htons(SOCK_DISCOVERY_MULTICAST_PORT);
    sockAddr.sin_addr.S_un.S_addr = SOCK_htonl(SOCK_INADDR_ANY);

    // UDP socket is easier in our scenario because it isn't session based
    g_Sockets_LWIP_Driver.m_multicastSocket = Socket( SOCK_AF_INET, SOCK_SOCK_DGRAM, SOCK_IPPROTO_UDP, FALSE );
    SOCKET_CHECK_RESULT( g_Sockets_LWIP_Driver.m_multicastSocket );

    // set sock option for multicast
    SOCK_ip_mreq multicast;
    multicast.imr_multiaddr.S_un.S_addr = SOCK_htonl(SOCK_DISCOVERY_MULTICAST_IPADDR);
    multicast.imr_interface.S_un.S_addr = SOCK_htonl(SOCK_INADDR_ANY);

    SOCKET_CHECK_RESULT( SOCK_ioctl(g_Sockets_LWIP_Driver.m_multicastSocket, SOCK_FIONBIO, &nonblocking) );

    SOCKET_CHECK_RESULT( SOCK_setsockopt( g_Sockets_LWIP_Driver.m_multicastSocket, SOCK_IPPROTO_IP, SOCK_IPO_ADD_MEMBERSHIP, (const char*)&multicast, sizeof(multicast) ) );
    
    // need this to keep the compiler happy about the cast to SOCK_sockaddr
    // which is intended and perfectly safe
    dummyPtr = &sockAddr;

    SOCKET_CHECK_RESULT(
        SOCK_bind(
            g_Sockets_LWIP_Driver.m_multicastSocket,
            (const SOCK_sockaddr*)&dummyPtr,
            sizeof(sockAddr))
    );

    g_Sockets_LWIP_Driver.s_discoveryInitialized = TRUE;


    SOCKET_CLEANUP()

    if(g_Sockets_LWIP_Driver.m_multicastSocket != SOCK_SOCKET_ERROR)
    {
        SOCK_close(g_Sockets_LWIP_Driver.m_multicastSocket);
        g_Sockets_LWIP_Driver.m_multicastSocket = SOCK_SOCKET_ERROR;

        debug_printf("MULTICAST SOCKET_ERROR: %d\r\n", HAL_SOCK_getlasterror() );
    }   

    SOCKET_CHECK_EXIT_bool_CLEANUP();
}

// TODO. Schedule the Discovery Multicast Response

//-----------------------------------------------------------------------------
//
//  MulticastDiscoveryRespond - Respond to a NETMF multicast message.  This is
//  a simple discovery mechanism that uses multicast sockets over UDP.  The
//  client will send a message to the ip address defined by
//     SOCK_DISCOVERY_MULTICAST_IPADDR 
//  over port
//     SOCK_DISCOVERY_MULTICAST_PORT
//  If the message contains the string "NETMF", then a response is sent with the
//  the data structure SOCK_discoveryinfo (which includes the ip address and mac
//  address of the device).
//
//-----------------------------------------------------------------------------

void Sockets_LWIP_Driver::MulticastDiscoverySchedule()
{
	if (g_Sockets_LWIP_Driver.m_multicastSocket != SOCK_SOCKET_ERROR)
	{
		SOCK_fd_set  privRead;
		SOCK_timeval to;

		to.tv_sec = 0;
		to.tv_usec = 0;

		privRead.fd_array[0] = g_Sockets_LWIP_Driver.m_multicastSocket;
		privRead.fd_count = 1;

		if (1 == HAL_SOCK_select(1, &privRead, NULL, NULL, &to))
		{
			if (!MulticastResponseContinuation.IsLinked())
			{
				MulticastResponseContinuation.Enqueue();
			}
		}
	}
}

void Sockets_LWIP_Driver::MulticastDiscoveryRespond(void* arg)
{
    (void)arg;

    NATIVE_PROFILE_PAL_COM();
    SOCK_sockaddr from;
    void *dummyPtr;

    char data[64];
    int fromlen = sizeof(from);

    // intentionally call the HAL recvfrom so as not to invoke the signalnotifythread
    int len = HAL_SOCK_recvfrom( g_Sockets_LWIP_Driver.m_multicastSocket, data, sizeof(data), 0, &from, &fromlen );

    if(len > 0)
    {
        int32_t idx = 0;
        bool fFound = FALSE;
        const char* c_Signature = SOCK_DISCOVERY_MULTICAST_TOKEN;
        int32_t sigLen = hal_strlen_s(c_Signature);

        // search for discovery token
        while(idx <= (len-sigLen))
        {
            if(0 == memcmp( &data[idx], c_Signature, sigLen))
            {
                fFound = TRUE;
                break;
            }
            idx++;
        }
        
        if(fFound)
        {
            // HAL_Configuration_NetworkInterface current;
            // SOCK_discoveryinfo info;
            SOCK_sockaddr_in sockAddr;
            SOCK_sockaddr_in sockAddrMulticast;
            int32_t opt = 64;
            SOCK_SOCKET sock;
            int nonblocking = 1;
            SOCKET_CHECK_ENTER();

            // // Load is required here because the g_NetworkConfig contains only the static ip address (not DHCP)
            // HAL_SOCK_CONFIGURATION_LoadAdapterConfiguration(0, &current);

            // info.ipaddr        = current.ipaddr;
            // info.macAddressLen = current.macAddressLen;
            // memcpy( &info.macAddressBuffer[0], &current.macAddressBuffer[0], current.macAddressLen );

             sock = Socket(SOCK_AF_INET, SOCK_SOCK_DGRAM, SOCK_IPPROTO_UDP, FALSE);
             SOCKET_CHECK_RESULT(sock);

            memset( &sockAddr, 0, sizeof(sockAddr) );
            sockAddr.sin_family           = SOCK_AF_INET;
            sockAddr.sin_port             = SOCK_htons(0);
            //  sockAddr.sin_addr.S_un.S_addr = info.ipaddr;

            memset( &sockAddrMulticast, 0, sizeof(sockAddrMulticast) );
            sockAddrMulticast.sin_family           = SOCK_AF_INET;
            sockAddrMulticast.sin_port             = SOCK_htons(SOCK_DISCOVERY_MULTICAST_PORT);
            sockAddrMulticast.sin_addr.S_un.S_addr = SOCK_htonl(SOCK_DISCOVERY_MULTICAST_IPADDR_SND);

            SOCKET_CHECK_RESULT(HAL_SOCK_ioctl(sock, SOCK_FIONBIO, &nonblocking));
            SOCKET_CHECK_RESULT(HAL_SOCK_setsockopt(sock, SOCK_IPPROTO_IP, SOCK_IPO_MULTICAST_TTL, (const char *) &opt, sizeof(opt)));

            // need this to keep the compiler happy about the cast to SOCK_sockaddr
            // which is intended and perfectly safe
            dummyPtr = &sockAddr;

            SOCKET_CHECK_RESULT(
                HAL_SOCK_bind(
                    sock,
                    (const SOCK_sockaddr*)&dummyPtr,
                    sizeof(sockAddr))
            );
            
            // send a multicast socket back to the caller
            //   SOCKET_CHECK_RESULT(SendTo(sock, (const char*)&info, sizeof(info), 0, (SOCK_sockaddr*)&sockAddrMulticast, sizeof(sockAddrMulticast)));

            SOCK_close(sock);

            SOCKET_CLEANUP();

            if(sock != SOCK_SOCKET_ERROR)
            {
                SOCK_close(sock);

                debug_printf("MULTICAST RESP SOCKET_ERROR: %d\r\n", HAL_SOCK_getlasterror() );
            }

            SOCKET_CHECK_EXIT_NORETURN();            
        }
    }

	MulticastDiscoverySchedule();
}


bool Sockets_LWIP_Driver::s_initializedDbg=FALSE;