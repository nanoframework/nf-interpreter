//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef _WIN_NET_SOCKETS_NATIVE_H_
#define _WIN_NET_SOCKETS_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>


struct Library_win_net_sockets_native_Windows_Networking_EndpointPair
{
    static const int FIELD__localHostName = 1;
    static const int FIELD__localServiceName = 2;
    static const int FIELD__remoteHostName = 3;
    static const int FIELD__remoteServiceName = 4;


    //--//

};

struct Library_win_net_sockets_native_Windows_Networking_HostName
{
    static const int FIELD__canonicalName = 1;
    static const int FIELD__displayName = 2;
    static const int FIELD__rawName = 3;
    static const int FIELD__hostNameType = 4;


    //--//

};

struct Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative
{
    NANOCLR_NATIVE_DECLARE(socket___STATIC__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(bind___STATIC__VOID__OBJECT__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(connect___STATIC__VOID__OBJECT__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(send___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(recv___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(close___STATIC__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(listen___STATIC__VOID__OBJECT__I4);
    NANOCLR_NATIVE_DECLARE(accept___STATIC__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(getaddrinfo___STATIC__VOID__STRING__BYREF_STRING__BYREF_SZARRAY_SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(shutdown___STATIC__VOID__OBJECT__I4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(sendto___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(recvfrom___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__BYREF_SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(getpeername___STATIC__VOID__OBJECT__BYREF_SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(getsockname___STATIC__VOID__OBJECT__BYREF_SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(getsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(setsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(poll___STATIC__BOOLEAN__OBJECT__I4__I4);
    NANOCLR_NATIVE_DECLARE(ioctl___STATIC__VOID__OBJECT__U4__BYREF_U4);

    //--//
    static HRESULT MarshalSockAddress( struct SOCK_sockaddr* addrDst, CLR_UINT32& addrLenDst, const CLR_RT_HeapBlock& blkSrc );
    static HRESULT MarshalSockAddress( CLR_RT_HeapBlock& blkDst, const struct SOCK_sockaddr* addrSrc, CLR_UINT32 addrLenSrc );
    static HRESULT SendRecvHelper( CLR_RT_StackFrame& stack, bool fSend, bool fAddress );
    static HRESULT SockOptHelper( CLR_RT_StackFrame& stack, bool fGet );
    static HRESULT SockNameHelper( CLR_RT_StackFrame& stack, bool fPeer );
    static HRESULT BindConnectHelper( CLR_RT_StackFrame& stack, bool fBind );
    static HRESULT ThrowOnError( CLR_RT_StackFrame& stack, CLR_INT32 err );
    static void    ThrowError( CLR_RT_StackFrame& stack, CLR_INT32 errorCode );
    
    static CLR_INT32 Helper__SelectSocket( CLR_INT32 socket, CLR_INT32 mode );

    /* WARNING!!!
     * The value of this constant is the offset for the m_Handle field in the System.Net.Sockets.Socket class.
     * It is defined here to avoid a circular reference issue.
     */
    static const int FIELD__m_Handle = 1;

    static const int DISPOSED_HANDLE = -1;
};

struct Library_win_net_sockets_native_Windows_Networking_Sockets_StreamSocket
{
    static const int FIELD__control = 1;
    static const int FIELD__socketHandle = 2;


    //--//

};

struct Library_win_net_sockets_native_Windows_Networking_Sockets_StreamSocketControl
{
    static const int FIELD__keepAlive = 1;
    static const int FIELD__minProtectionLevel = 2;
    static const int FIELD__noDelay = 3;
    static const int FIELD__outboundBufferSizeInBytes = 4;
    static const int FIELD__outboundUnicastHopLimit = 5;
    static const int FIELD__qualityOfService = 6;
    static const int FIELD__serializeConnectionAttempts = 7;


    //--//

};

struct Library_win_net_sockets_native_Windows_Networking_Sockets_StreamSocketInformation
{
    static const int FIELD__localAddress = 1;
    static const int FIELD__localPort = 2;
    static const int FIELD__protectionLevel = 3;
    static const int FIELD__remoteAddress = 4;
    static const int FIELD__remoteHostName = 5;
    static const int FIELD__remotePort = 6;
    static const int FIELD__remoteServiceName = 7;


    //--//

};


    //--//

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Networking_Sockets;

#endif  //_WIN_NET_SOCKETS_NATIVE_H_
