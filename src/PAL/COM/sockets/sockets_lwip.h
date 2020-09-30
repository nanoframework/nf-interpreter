//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _SOCKETS_H_
#define _SOCKETS_H_ 1

//--//

#include "nanoHAL.h"

//--//

#define SOCKET_CHECK_ENTER()  \
    int32_t ret=0               \

#define SOCKET_CLEANUP() \
    goto RETURN_OK;      \
    SOCKET_CHECK_RETURN: \

#define SOCKET_CHECK_EXIT_CLEANUP()   \
    RETURN_OK:                        \
    ASSERT(ret != SOCK_SOCKET_ERROR); \
    return ret;                       \

#define SOCKET_CHECK_EXIT()           \
    SOCKET_CLEANUP()                  \
    RETURN_OK:                        \
    ASSERT(ret != SOCK_SOCKET_ERROR); \
    return ret;                       \

#define SOCKET_CHECK_EXIT_bool()      \
    SOCKET_CLEANUP()                  \
    RETURN_OK:                        \
    return ret != SOCK_SOCKET_ERROR;  \

#define SOCKET_CHECK_EXIT_bool_CLEANUP()\
    RETURN_OK:                          \
    return ret != SOCK_SOCKET_ERROR;    \

#define SOCKET_CHECK_RESULT(x)        \
    if((ret = (x)) == SOCK_SOCKET_ERROR)\
    {                                 \
        goto SOCKET_CHECK_RETURN;     \
    }                                 \

#define SOCKET_CHECK_bool(x)          \
    if(!(x))                          \
    {                                 \
        ret = SOCK_SOCKET_ERROR;      \
        goto SOCKET_CHECK_RETURN;     \
    }                                 \

#define SOCKET_CHECK_EXIT_NORETURN() \
    RETURN_OK:                       \

struct Sockets_LWIP_Driver
{
    static SOCK_SOCKET Socket( int32_t family, int32_t type, int32_t protocol, bool fDebug );
    static int Connect(SOCK_SOCKET socket, const struct SOCK_sockaddr* address, int32_t addressLen) ;
    static int Send(SOCK_SOCKET socket, const char* buf, int32_t len, int32_t flags) ;
    static int Recv(SOCK_SOCKET socket, char* buf, int32_t len, int32_t flags);
    static int Close(SOCK_SOCKET socket);
    static int Listen( SOCK_SOCKET socket, int32_t backlog );
    static SOCK_SOCKET Accept( SOCK_SOCKET socket, struct SOCK_sockaddr* address, int* addressLen, bool fDebug );
    static int Select( int32_t nfds, SOCK_fd_set* readfds, SOCK_fd_set* writefds, SOCK_fd_set* except, const struct SOCK_timeval* timeout );
    static int RecvFrom( SOCK_SOCKET s, char* buf, int32_t len, int32_t flags, struct SOCK_sockaddr* from, int* fromlen );
    static int SendTo( SOCK_SOCKET s, const char* buf, int32_t len, int32_t flags, const struct SOCK_sockaddr* to, int32_t tolen );
    static int Shutdown( SOCK_SOCKET s, int32_t how);
    
    static bool  Initialize();
    static bool  Uninitialize();
    static void  CloseConnections(bool fCloseDbg);

    static void SaveConfig(int32_t index, HAL_Configuration_NetworkInterface *cfg);

    // Debugger methods ( socket_debugger.cpp )
    static void Debugger_Initialize();
    static void Debugger_Uninitialize();
    static void Debugger_Abort();

    static bool InitializeDbgListener( int ComPortNum );
    static bool UninitializeDbgListener( int ComPortNum );
    static int  Write( int ComPortNum, const char* Data, size_t size );
    static int  Read ( int ComPortNum, char*       Data, size_t size );
    static bool UpgradeToSsl( int ComPortNum, const int8_t* pCACert, uint32_t caCertLen, const int8_t* pDeviceCert, uint32_t deviceCertLen, const char* szTargetHost );
    static bool IsUsingSsl( int ComPortNum );

    void* GetSocketSslData(SOCK_SOCKET socket)
    {
        int32_t tmp;

        return GetSocketSslData(socket, tmp);
    }

    void* GetSocketSslData(SOCK_SOCKET socket, int32_t &sockIndex)
    {
        GLOBAL_LOCK_SOCKETS(x);
        
        for(int i=m_cntSockets-1; i>=0; i--)
        {
            if(socket == m_socketHandles[i].m_socket)
            {
                sockIndex = i;
                return m_socketHandles[i].m_sslData;
            }
        }
        
        return NULL;
    }

    void SetSocketSslData(SOCK_SOCKET socket, void* sslObj)
    {
        GLOBAL_LOCK_SOCKETS(x);

        for(int i=m_cntSockets-1; i>=0; i--)
        {
            if(socket == m_socketHandles[i].m_socket)
            {
                m_socketHandles[i].m_sslData = sslObj;
                
                break;
            }
        }
    }

    // required by SSL
    static void UnregisterSocket( int index );


    static void ClearStatusBitsForSocket(SOCK_SOCKET sock, bool fWrite);
    static bool InitializeMulticastDiscovery();

private:

    static void RegisterSocket( SOCK_SOCKET sock, bool selectable, bool fDebug );
    
    struct SocketRegisterMap
    {
        SOCK_SOCKET m_socket;
        uint32_t      m_flags;
        void*       m_sslData;
        
        static const uint32_t c_SelectableSocket = 0x0001;
        static const uint32_t c_DebugSocket      = 0x0002;
        static const uint32_t c_CloseSocket      = 0x0004;
    };

    enum DebuggerState
    {
        DbgSock_Uninitialized = 0,
        DbgSock_Listening     = 1,
        DbgSock_Connected     = 2,
    } m_stateDebugSocket;

    bool m_usingSSL;
    
    static void MulticastDiscoveryRespond(void* arg);
	static void MulticastDiscoverySchedule();

    static void CloseDebuggerSocket();
    static void OnDebuggerTimeout(void* arg);
    
    static HAL_COMPLETION s_DebuggerTimeoutCompletion;

    SOCK_SOCKET    m_SocketDebugListener;
    SOCK_SOCKET    m_SocketDebugStream;
    SOCK_SOCKET    m_multicastSocket;

    bool           m_fShuttingDown;

    int32_t  m_cntSockets;
    struct SocketRegisterMap m_socketHandles[SOCKETS_MAX_COUNT];

    static bool           s_initialized;
    static bool           s_initializedDbg;
    static const int32_t    c_WellKnownDebugPort = DEBUG_SOCKET_PORT;
    static bool           s_wirelessInitialized;
    static bool           s_discoveryInitialized;
};

extern Sockets_LWIP_Driver g_Sockets_LWIP_Driver;

#endif //_SOCKETS_H_

