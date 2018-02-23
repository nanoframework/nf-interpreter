//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#ifndef _SYS_NET_NATIVE_H_
#define _SYS_NET_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <nanoHAL_time.h>
#include <corlib_native.h>


struct Library_sys_net_native_System_Net_IPAddress
{
    static const int FIELD_STATIC__Any = 0;
    static const int FIELD_STATIC__Loopback = 1;

    static const int FIELD__m_Address = 1;


    //--//

};

struct Library_sys_net_native_System_Net_IPEndPoint
{
    static const int FIELD__m_Address = 1;
    static const int FIELD__m_Port = 2;


    //--//

};

struct Library_sys_net_native_System_Net_IPHostEntry
{
    static const int FIELD__hostName = 1;
    static const int FIELD__addressList = 2;


    //--//

};

struct Library_sys_net_native_System_Net_SocketAddress
{
    static const int FIELD__m_Buffer = 1;


    //--//

};

struct Library_sys_net_native_System_Net_Sockets_NativeSocket
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

struct Library_sys_net_native_System_Net_Sockets_NetworkStream
{
    static const int FIELD___socket = 1;
    static const int FIELD___socketType = 2;
    static const int FIELD___remoteEndPoint = 3;
    static const int FIELD___ownsSocket = 4;
    static const int FIELD___disposed = 5;

    //--//

};

struct Library_sys_net_native_System_Net_Sockets_Socket
{
    static const int FIELD__m_Handle = 1;
    static const int FIELD__m_fBlocking = 2;
    static const int FIELD__m_localEndPoint = 3;
    static const int FIELD__m_recvTimeout = 4;
    static const int FIELD__m_sendTimeout = 5;


    //--//

};

struct Library_sys_net_native_System_Net_Sockets_SocketException
{
    static const int FIELD___errorCode = 5;


    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Net;

#endif  //_SYS_NET_NATIVE_H_
