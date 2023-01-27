//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef SOCKETS_H
#define SOCKETS_H

//--//

#include "nanoHAL.h"

#include <zephyr.h>

//#include <string.h>
#include <net/socket.h>
#include <fcntl.h>
#ifndef restrict
#define restrict
#endif
#include <nrf_socket.h>

#define NATIVE_PROFILE_PAL_NETWORK()
//#define NATIVE_PROFILE_PAL_NETWORK() printk("%s++\n", __func__);

int GetAvailBytes (SOCK_SOCKET socket);
extern "C" int signal_nf_stack(void);

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

struct Sockets_nrf9160_Driver
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

    //(db) TODO: reduce the code below with a single lookup
    //struct Sockets_nrf9160_Driver::SocketRegisterMap *GetSockRegMap(SOCK_SOCKET socket)
    uint32_t *GetSockFlags(SOCK_SOCKET socket)
    {
        for(int i=m_cntSockets-1; i>=0; i--)
        {
            if(socket == m_socketHandles[i].m_socket)
                return &m_socketHandles[i].m_flags;
        }
        printk ("***** ERROR socket %d not found in SocketRegisterMap\n", socket);
        ASSERT(FALSE);
        return 0;
    }

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

#ifdef PERRADDR_WORKAROUND    
    int GetSocketAddrData(SOCK_SOCKET socket, void *addr, int *len)
    {
        GLOBAL_LOCK_SOCKETS(x);
        
        for(int i=m_cntSockets-1; i>=0; i--)
        {
            if(socket == m_socketHandles[i].m_socket)
            {
                int addrlen = m_socketHandles[i].peeraddr_len;
                if (*len >= addrlen)
                {
                    memcpy (addr, &m_socketHandles[i].peeraddr, addrlen);
                    *len = addrlen;
                }
                return 0;
            }
        }
        printk ("GetSocketAddrData sock %d. data not found %p %d\n", socket, addr, *len);
        return -1;
    }

    int SetSocketAddrData(SOCK_SOCKET socket, const void* data, int len)
    {
        GLOBAL_LOCK_SOCKETS(x);

        for(int i=m_cntSockets-1; i>=0; i--)
        {
            if(socket == m_socketHandles[i].m_socket)
            {
                //printk ("SetSocketAddrData sock found %d\n", i);
                if (len <= (int)sizeof (m_socketHandles[i].peeraddr))
                {
                    //printk ("SetSocketAddrData copy data\n");
                    memcpy (&m_socketHandles[i].peeraddr, data, len);
                    m_socketHandles[i].peeraddr_len = len;
                }
                else
                    printk ("SetSocketAddrData sockaddr too long %d  %d\n", len, sizeof (m_socketHandles[i].peeraddr));

                return 0;
            }
        }
        printk ("SetSocketAddrData sock %d. data not found %p %d\n", socket, data, len);
        return -1;
    }
#endif
    // required by SSL
    static void UnregisterSocket( int index );


    static void ClearStatusBitsForSocket(SOCK_SOCKET sock, bool fWrite);
    static bool InitializeMulticastDiscovery();

private:

    static void SockMonitorThread(void *a, void *b, void *c);

    static void RegisterSocket( SOCK_SOCKET sock, bool selectable, bool fDebug );
    
    struct SocketRegisterMap
    {
        SOCK_SOCKET m_socket;
        uint32_t      m_flags;
        void*       m_sslData;
#ifdef PERRADDR_WORKAROUND    
        // Workaround for getpeeraddr
        struct SOCK_sockaddr peeraddr;
        int peeraddr_len;
#endif
        
        static const uint32_t c_SelectableSocket = 0x0001;
        static const uint32_t c_DebugSocket      = 0x0002;
        static const uint32_t c_CloseSocket      = 0x0004;

        static const uint32_t c_SelRdSocket      = 0x0010;
        static const uint32_t c_SelWrSocket      = 0x0020;
        static const uint32_t c_SelErSocket      = 0x0040;
        static const uint32_t c_SelSocketMask    = 0x0070;
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

    //(db)
    struct k_sem   sem_sockmon;

    int32_t  m_cntSockets;
    struct SocketRegisterMap m_socketHandles[SOCKETS_MAX_COUNT];

    static bool           s_initialized;
    static bool           s_initializedDbg;
    static const int32_t    c_WellKnownDebugPort = DEBUG_SOCKET_PORT;
    static bool           s_wirelessInitialized;
    static bool           s_discoveryInitialized;
};

extern Sockets_nrf9160_Driver g_Sockets_nrf9160_Driver;

#endif // SOCKETS_H

