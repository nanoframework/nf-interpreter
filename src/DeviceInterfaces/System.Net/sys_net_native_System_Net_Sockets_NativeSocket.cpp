//
// Copyright (c) 2018 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "sys_net_native.h"

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::socket___STATIC__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_INT32 family   = stack.Arg0().NumericByRef().s4;
    CLR_INT32 type     = stack.Arg1().NumericByRef().s4;
    CLR_INT32 protocol = stack.Arg2().NumericByRef().s4;
    
    CLR_INT32 nonBlocking = 1;
    CLR_INT32 sock        = SOCK_socket( family, type, protocol );
    
    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, sock ));
    
    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, SOCK_ioctl( sock, SOCK_FIONBIO, &nonBlocking ) ));

    stack.SetResult_I4( sock );
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::bind___STATIC__VOID__OBJECT__SystemNetEndPoint( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    return BindConnectHelper( stack, true );
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::connect___STATIC__VOID__OBJECT__SystemNetEndPoint__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    return BindConnectHelper( stack, false );

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::send___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    return SendRecvHelper( stack, true, false );
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::recv___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    return SendRecvHelper( stack, false, false );
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::close___STATIC__I4__OBJECT( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    CLR_INT32 handle, ret;

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference(); FAULT_ON_NULL(socket);
    
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    ret = SOCK_close( handle );
    
    //If a socket gets closed, we need to make sure to wake up any threads that are waiting on it.
    Events_Set( SYSTEM_EVENT_FLAG_SOCKET );

    stack.SetResult_I4( ret );
    
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::listen___STATIC__VOID__OBJECT__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference();
    CLR_INT32 handle;
    CLR_INT32 backlog = stack.Arg1().NumericByRef().s4;
    CLR_INT32 ret;

    FAULT_ON_NULL(socket);
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    ret = SOCK_listen( handle, backlog );

    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::accept___STATIC__I4__OBJECT( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference();
    CLR_INT32 handle;
    CLR_INT32 ret;
    CLR_INT32 nonBlocking = 1;

    FAULT_ON_NULL(socket);
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;


    /* Because we could have been a rescheduled call due to a prior call that would have blocked, we need to see
            * if our handle has been shutdown before continuing. */
    if (handle == DISPOSED_HANDLE)
    {
        ThrowError(stack, CLR_E_OBJECT_DISPOSED);
        NANOCLR_SET_AND_LEAVE (CLR_E_PROCESS_EXCEPTION);
    }

    ret = SOCK_accept( handle, NULL, NULL );

    if(ret != SOCK_SOCKET_ERROR)
    {
        NANOCLR_CHECK_HRESULT(ThrowOnError( stack, SOCK_ioctl( ret, SOCK_FIONBIO, &nonBlocking ) ));
    }

    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    stack.SetResult_I4( ret );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::getaddrinfo___STATIC__VOID__STRING__BYREF_STRING__BYREF_SZARRAY_SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    LPCSTR szName = stack.Arg0().RecoverString();
    struct SOCK_addrinfo hints;
    struct SOCK_addrinfo* addr = NULL;
    struct SOCK_addrinfo* addrT;
    CLR_UINT32        cAddresses = 0;
    CLR_RT_HeapBlock* pAddress;
    CLR_INT32         timeout_ms = 30000;
    CLR_RT_HeapBlock  hbTimeout;
    CLR_INT32         ret;
    bool              fRes = true;
    CLR_INT64*        timeout;
    uint32_t          socketError = SOCK_SOCKET_ERROR;

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger( (CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS );

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

    do
    {
        memset( &hints, 0, sizeof(hints) );

        ret = SOCK_getaddrinfo( szName, NULL, &hints, &addr );

        if(ret == SOCK_SOCKET_ERROR)
        {
            // get last error from socket
            socketError = SOCK_getlasterror();

            if(socketError == SOCK_EWOULDBLOCK)
            {
                // non-blocking - allow other threads to run while we wait for handle activity
                NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_Socket, fRes ));
            }
            else
            {
                break;
            }
        }
        else
        {
            // return without error, clear socket error
            socketError = 0;

            // done here
            break;
        }
    }
    while(fRes);
    
    // timeout expired
    if(!fRes)
    {
        ret = SOCK_SOCKET_ERROR;
        
        ThrowError( stack, SOCK_ETIMEDOUT );
    
        NANOCLR_SET_AND_LEAVE( CLR_E_PROCESS_EXCEPTION );
    }
    else
    {
        // error wasn't a timeout so use socket error
        ret = socketError;
    }

    // getaddrinfo returns a winsock error code rather than SOCK_SOCKET_ERROR, so pass this on to the exception handling
    if(ret != 0)
    {
        ThrowError( stack, ret );
        NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
    }

    {
        CLR_RT_HeapBlock  hbCanonicalName;
        CLR_RT_HeapBlock  hbAddresses;
        
        hbCanonicalName.SetObjectReference( NULL );
        CLR_RT_ProtectFromGC gc( hbCanonicalName );

        hbAddresses.SetObjectReference( NULL );
        CLR_RT_ProtectFromGC gc2( hbAddresses );

        for(int pass = 0; pass < 2; pass++)
        {                                    
            cAddresses = 0;

            for(addrT = addr; addrT != NULL; addrT = addrT->ai_next)
            {
                if(pass == 1)
                {
                    if(addrT->ai_canonname && addrT->ai_canonname[ 0 ])
                    {
                        //allocate return string
                        NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_String::CreateInstance( hbCanonicalName, addrT->ai_canonname ));
                        NANOCLR_CHECK_HRESULT(hbCanonicalName.StoreToReference( stack.Arg1(), 0 ));
                    }

                    //allocate address and store into array
                    pAddress = (CLR_RT_HeapBlock*)hbAddresses.DereferenceArray()->GetElement( cAddresses );

                    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( *pAddress, (CLR_UINT32)addrT->ai_addrlen, g_CLR_RT_WellKnownTypes.m_UInt8 ));

                    //copy address.
                    memcpy( pAddress->DereferenceArray()->GetFirstElement(), addrT->ai_addr, addrT->ai_addrlen );
                }
                                
                cAddresses++;
            }
                            
            if(pass == 0)
            {
                //allocate array of byte arrays
                CLR_RT_ReflectionDef_Index idx;

                idx.m_kind               = REFLECTION_TYPE;
                idx.m_levels             = 2;
                idx.m_data.m_type.m_data = g_CLR_RT_WellKnownTypes.m_UInt8.m_data;

                NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( hbAddresses, cAddresses, idx ));

                NANOCLR_CHECK_HRESULT(hbAddresses.StoreToReference( stack.Arg2(), 0 ));                
            }
        }    
    }

    stack.PopValue();       // Timeout
    
    NANOCLR_CLEANUP();

    if( addr ) SOCK_freeaddrinfo( addr );

    NANOCLR_CLEANUP_END();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::shutdown___STATIC__VOID__OBJECT__I4__BYREF_I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference();
    CLR_INT32 handle;
    CLR_INT32 how = stack.Arg1().NumericByRef().s4;
    CLR_INT32 ret;
    
    FAULT_ON_NULL(socket);
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    ret = SOCK_shutdown( handle, how );

    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::sendto___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__SystemNetEndPoint( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    return SendRecvHelper( stack, true, true );
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::recvfrom___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__BYREF_SystemNetEndPoint( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    return SendRecvHelper( stack, false, true );
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::getpeername___STATIC__VOID__OBJECT__BYREF_SystemNetEndPoint( CLR_RT_StackFrame& stack )
{
   NATIVE_PROFILE_CLR_NETWORK();
   return SockNameHelper( stack, true );
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::getsockname___STATIC__VOID__OBJECT__BYREF_SystemNetEndPoint( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    return SockNameHelper( stack, false );
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::getsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    return SockOptHelper( stack, true );
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::setsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    return SockOptHelper( stack, false );
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::poll___STATIC__BOOLEAN__OBJECT__I4__I4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference();
    CLR_INT32 handle;
    CLR_INT32 mode       = stack.Arg1().NumericByRef().s4;    
    CLR_INT32 timeout_us = stack.Arg2().NumericByRef().s4;
    CLR_RT_HeapBlock hbTimeout;

    CLR_INT32 res = 0;
    bool fRes     = true;

    CLR_INT64* timeout;

    FAULT_ON_NULL(socket);
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    /* Because we could have been a rescheduled call due to a prior call that would have blocked, we need to see
     * if our handle has been shutdown before continuing. */
    if (handle == DISPOSED_HANDLE)
    {
        ThrowError( stack, CLR_E_OBJECT_DISPOSED );
        NANOCLR_SET_AND_LEAVE (CLR_E_PROCESS_EXCEPTION);
    }

    if(timeout_us < 0)
    {
        // Infinite Timeout
        // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
        hbTimeout.SetInteger( (CLR_INT64)-1 );
    }
    else
    {
        // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
        hbTimeout.SetInteger( (CLR_INT64)timeout_us * TIME_CONVERSION__TO_MILLISECONDS / 1000 );
    }

   
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

    while(fRes)
    {
        res = Helper__SelectSocket( handle, mode );

        if(res != 0) break;

        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_Socket, fRes ));
    }

    stack.PopValue(); //timer

    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, res ));

    stack.SetResult_Boolean( res != 0 );   

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::ioctl___STATIC__VOID__OBJECT__U4__BYREF_U4( CLR_RT_StackFrame& stack )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference();
    CLR_INT32 handle;
    CLR_INT32 cmd     = stack.Arg1().NumericByRef().s4;
    CLR_RT_HeapBlock blkArg;
    CLR_INT32 ret;

    FAULT_ON_NULL(socket);
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    _SIDE_ASSERTE(SUCCEEDED(blkArg.LoadFromReference( stack.Arg2() )));

    ret = SOCK_ioctl( handle, cmd, (CLR_INT32*)&blkArg.NumericByRef().s4 );
    
    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    _SIDE_ASSERTE(SUCCEEDED(blkArg.StoreToReference( stack.Arg2(), 0 )));

    NANOCLR_NOCLEANUP();
}

//--//

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::BindConnectHelper( CLR_RT_StackFrame& stack, bool fBind )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference();

    CLR_INT32 handle;
    SOCK_sockaddr addr;
    CLR_UINT32 addrLen = sizeof(addr);
    CLR_INT32 ret;
    bool fThrowOnWouldBlock = false;

    FAULT_ON_NULL(socket);
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    NANOCLR_CHECK_HRESULT(MarshalSockAddress( &addr, addrLen, stack.Arg1() ));

    if(fBind)
    {
        ret = SOCK_bind( handle, &addr, addrLen );
    }
    else
    {
        ret = SOCK_connect( handle, &addr, addrLen );
        
        fThrowOnWouldBlock = (stack.Arg2().NumericByRefConst().s4 != 0);
        
        if(!fThrowOnWouldBlock && SOCK_getsocklasterror( handle ) == SOCK_EWOULDBLOCK)
        {
            NANOCLR_SET_AND_LEAVE(S_OK);            
        }
    }

    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    NANOCLR_NOCLEANUP();
}

CLR_INT32 Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(CLR_INT32 handle, CLR_INT32 mode )
{
    struct SOCK_timeval timeval;
    SOCK_fd_set* readfds   = NULL;
    SOCK_fd_set* writefds  = NULL;
    SOCK_fd_set* exceptfds = NULL;
    SOCK_fd_set fds;
    SOCK_fd_set fdsExcept;
    CLR_INT32   res = 0;

    switch(mode)
    {
        case 0:
            readfds = &fds;
            break;
        case 1:
            writefds = &fds;
            break;
        default:
            _ASSERTE(FALSE);
            // fall through
        case 2:
            exceptfds = &fds;
            break;
    }
    
    fds.fd_count      = 1;
    fds.fd_array[ 0 ] = handle;

    // This Poll method is a little handicapped in the sense that it only allows the caller to wait
    // for a read, a write or an except. This causes a problem when there is a socket exception.  The
    // poll will continue to block forever because the select statement wasn't looking for exceptions
    // Therefore, we will force the select call to look for the except case if it is not already doing it.
    if(exceptfds == NULL)
    {
        fdsExcept.fd_count      = 1;
        fdsExcept.fd_array[ 0 ] = handle;
        exceptfds               = &fdsExcept;
    }

    timeval.tv_sec  = 0;
    timeval.tv_usec = 0;

    res = SOCK_select( 1, readfds, writefds, exceptfds, &timeval );

    // socket is in the exception state (only return error if caller was NOT looking for the excepted state)
    if((mode != 2) && (fdsExcept.fd_count != 0))
    {
        // For read mode ignore exception if we have data to read
        if(!(mode == 0 && fds.fd_count != 0))
        {
            return SOCK_SOCKET_ERROR;
        }
    }

    return res;
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::MarshalSockAddress( CLR_RT_HeapBlock& blkDst, const struct SOCK_sockaddr* addrSrc, CLR_UINT32 addrLenSrc )
{
    NATIVE_PROFILE_CLR_NETWORK();

    NANOCLR_HEADER();

    (void)addrLenSrc;

    SOCK_sockaddr_in* src = (SOCK_sockaddr_in*)addrSrc;

    CLR_RT_TypeDef_Index ipAddressTypeDef;
    CLR_RT_TypeDef_Index ipEndPointTypeDef;
    CLR_RT_HeapBlock* ipAddressHbObj;
    CLR_RT_HeapBlock* ipEndPointHbObj;

    CLR_RT_HeapBlock ipAddress; ipAddress.SetObjectReference( NULL );
    CLR_RT_ProtectFromGC gc1( ipAddress );

    CLR_RT_HeapBlock ipEndPoint; ipEndPoint.SetObjectReference( NULL );
    CLR_RT_ProtectFromGC gc2( ipEndPoint );

    // find <IPAddress> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef( "IPAddress", "System.Net", ipAddressTypeDef );

    // create an instance of <IPAddress>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(ipAddress, ipAddressTypeDef));

    // find <IPEndPoint> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef( "IPEndPoint", "System.Net", ipEndPointTypeDef );

    // create an instance of <IPEndPoint>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(ipEndPoint, ipEndPointTypeDef));

    // dereference the objects in order to reach their fields
    ipAddressHbObj = ipAddress.Dereference();
    ipEndPointHbObj = ipEndPoint.Dereference();

    {
        // get a reference to the managed fields and set them

        // IPAddress _address field
        // CLR_INT64 fields need to be accessed by pointer
        CLR_RT_HeapBlock& addressFieldRef = ipAddressHbObj[ Library_sys_net_native_System_Net_IPAddress::FIELD___address ];
        CLR_INT64* pRes = (CLR_INT64*)&addressFieldRef.NumericByRef().s8;
        *pRes = src->sin_addr.S_un.S_addr;
        
        // IPAddress _family field
        ipAddressHbObj[ Library_sys_net_native_System_Net_IPAddress::FIELD___family ].NumericByRef().s4 = src->sin_family;
        
        // IPEndPoint _port field
        // take care of endianess swapping
        ipEndPointHbObj[ Library_sys_net_native_System_Net_IPEndPoint::FIELD___port ].NumericByRef().s4 = SOCK_ntohs(src->sin_port);

        // set IPEndPoint address with IPAddress heap block object
        ipEndPointHbObj[ Library_sys_net_native_System_Net_IPEndPoint::FIELD___address ].SetObjectReference( ipAddressHbObj );
    }

    _ASSERTE(blkDst.DataType() == DATATYPE_BYREF);

    // store the new IPEndPoint object to reference
    NANOCLR_CHECK_HRESULT(ipEndPoint.StoreToReference( blkDst, 0 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::MarshalSockAddress( struct SOCK_sockaddr* addrDst, CLR_UINT32& addrLenDst, const CLR_RT_HeapBlock& blkEndPointAddress )
{        
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    SOCK_sockaddr_in* dst = (SOCK_sockaddr_in*)addrDst;

    CLR_RT_HeapBlock* endPointAddress;
    CLR_RT_HeapBlock* remoteEndPointAddress;
    int64_t address;
    int32_t port;

    endPointAddress = blkEndPointAddress.Dereference();                    
    FAULT_ON_NULL(endPointAddress);

    // get a pointer to the managed field object instance for m_Address
    remoteEndPointAddress = endPointAddress[ Library_sys_net_native_System_Net_IPEndPoint::FIELD___address ].Dereference();
    FAULT_ON_NULL(remoteEndPointAddress);

    // get value of m_Address field (type long)
    address = (CLR_INT64)remoteEndPointAddress[ Library_sys_net_native_System_Net_IPAddress::FIELD___address ].NumericByRef().s8;

    // get value of m_Port field (type int)
    port = endPointAddress[ Library_sys_net_native_System_Net_IPEndPoint::FIELD___port ].NumericByRef().s4;

    // clear struct
    memset(dst, 0, sizeof(SOCK_sockaddr));

    dst->sin_family           = SOCK_AF_INET;
    // need to convert port number to network order
    dst->sin_port             = SOCK_htons(port);

    //address already in network byte order
    memcpy((int8_t*)&dst->sin_addr.S_un.S_addr, (int8_t*)&address, sizeof(address));

    addrLenDst = sizeof(address);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::SendRecvHelper( CLR_RT_StackFrame& stack, bool fSend, bool fAddress )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock*       socket    = stack.Arg0().Dereference();
    CLR_INT32               handle;
    CLR_RT_HeapBlock_Array* arrData   = stack.Arg1().DereferenceArray(); 
    CLR_UINT32              offset    = stack.Arg2().NumericByRef().u4;
    CLR_UINT32              count     = stack.Arg3().NumericByRef().u4;
    CLR_INT32               flags     = stack.Arg4().NumericByRef().s4;
    CLR_RT_HeapBlock        hbTimeout;

    CLR_INT64* timeout;
    CLR_UINT8* buf;
    bool       fRes = true;
    CLR_INT32  totReadWrite;
    CLR_INT32  ret = 0;

    FAULT_ON_NULL(socket);
    FAULT_ON_NULL(arrData);
    
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    /* Because we could have been a rescheduled call due to a prior call that would have blocked, we need to see
     * if our handle has been shutdown before continuing. */
    if (handle == DISPOSED_HANDLE)
    {
        ThrowError( stack, CLR_E_OBJECT_DISPOSED );
        NANOCLR_SET_AND_LEAVE (CLR_E_PROCESS_EXCEPTION);
    }

    if(offset + count > arrData->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);    

    // Infinite Timeout
    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger( (CLR_INT64)-1 );
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks( hbTimeout, timeout ));

    //
    // Push "totReadWrite" onto the eval stack.
    //
    if(stack.m_customState == 1)
    {
        stack.PushValueI4( 0 );
        
        stack.m_customState = 2;
    }

    totReadWrite = stack.m_evalStack[ 1 ].NumericByRef().s4;

    buf    = arrData->GetElement( offset + totReadWrite );
    count -= totReadWrite;

    while(count > 0)
    {
        CLR_INT32 bytes = 0;

        // first make sure we have data to read or ability to write
        while(fRes)
        {
            ret = Helper__SelectSocket( handle, fSend ? 1 : 0 );

            if(ret != 0) break;

            // non-blocking - allow other threads to run while we wait for handle activity
            NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_Socket, fRes ));
        }

        // timeout expired
        if(!fRes)
        {
            ret = SOCK_SOCKET_ERROR;
            
            ThrowError( stack, SOCK_ETIMEDOUT );

            NANOCLR_SET_AND_LEAVE( CLR_E_PROCESS_EXCEPTION );
        }

        // socket is in the excepted state, so let's bail out
        if(SOCK_SOCKET_ERROR == ret)
        {
            break;
        }

        if(fAddress)
        {
            struct SOCK_sockaddr addr;
            CLR_UINT32 addrLen = sizeof(addr);
            CLR_RT_HeapBlock& blkAddr = stack.Arg6();

            if(fSend)
            {
                NANOCLR_CHECK_HRESULT(MarshalSockAddress( &addr, addrLen, blkAddr ));

                bytes = SOCK_sendto( handle, (const char*)buf, count, flags, &addr, addrLen );
            }
            else
            {
                CLR_RT_HeapBlock* pBlkAddr = blkAddr.Dereference();
                
                NANOCLR_CHECK_HRESULT(MarshalSockAddress( &addr, addrLen, *pBlkAddr ));

                bytes = SOCK_recvfrom( handle, (char*)buf, count, flags, &addr, (int*)&addrLen );

                if(bytes != SOCK_SOCKET_ERROR)
                {
                    NANOCLR_CHECK_HRESULT(MarshalSockAddress( blkAddr, &addr, addrLen ));
                }
            }
        }
        else
        {
            if(fSend)
            {
                bytes = SOCK_send( handle, (const char*)buf, count, flags );
            }
            else
            {
                bytes = SOCK_recv( handle, (char*)buf, count, flags );
            }
        }

        // send/recv/sendto/recvfrom failed
        if(bytes == SOCK_SOCKET_ERROR)
        {
            CLR_INT32 err = SOCK_getsocklasterror(handle);
            
            if(err != SOCK_EWOULDBLOCK)
            {
                ret = SOCK_SOCKET_ERROR;
                break;
            }
            
            continue;
        }
                // zero recv bytes indicates the handle has been closed.
        else if(!fSend && (bytes == 0)) 
        {
            break;
        }
        
        buf          += bytes;
        totReadWrite += bytes;
        count        -= bytes;

        stack.m_evalStack[ 1 ].NumericByRef().s4 = totReadWrite;        

        // receive returns immediately after receiving bytes.
        if(!fSend && (totReadWrite > 0))
        {
            break;
        }

    }

    stack.PopValue();       // totReadWrite
    stack.PopValue();       // Timeout
    
    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    stack.SetResult_I4( totReadWrite );

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::SockOptHelper( CLR_RT_StackFrame& stack, bool fGet )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference();
    CLR_INT32 handle;
    CLR_INT32 level   = stack.Arg1().NumericByRef().s4;
    CLR_INT32 optname = stack.Arg2().NumericByRef().s4;
    CLR_RT_HeapBlock_Array* arrOpt = stack.Arg3().DereferenceArray(); 
    char* optval;
    CLR_INT32 optlen;
    CLR_INT32 ret;

    FAULT_ON_NULL(socket);
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    FAULT_ON_NULL(arrOpt);
    
    optval = (char*)arrOpt->GetFirstElement();
    optlen = arrOpt->m_numOfElements;

    if(fGet)
    {
        ret = SOCK_getsockopt( handle, level, optname, optval, &optlen );
        _ASSERTE( optlen <= (CLR_INT32)arrOpt->m_numOfElements ); 
    }
    else
    {
        ret = SOCK_setsockopt( handle, level, optname, optval, optlen );
    }

    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::SockNameHelper( CLR_RT_StackFrame& stack, bool fPeer )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock* socket = stack.Arg0().Dereference();
    CLR_INT32 handle;
    CLR_INT32 ret;
    
    struct SOCK_sockaddr addr;
    CLR_INT32 addrLen = sizeof(addr);

    FAULT_ON_NULL(socket);
    handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

    if(fPeer)
    {
        ret = SOCK_getpeername( handle, &addr, &addrLen );
    }
    else
    {
        ret = SOCK_getsockname( handle, &addr, &addrLen );
    }
    
    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    NANOCLR_CHECK_HRESULT(MarshalSockAddress( stack.Arg1(), &addr, addrLen ));
    
    NANOCLR_NOCLEANUP();
}

void Library_sys_net_native_System_Net_Sockets_NativeSocket::ThrowError( CLR_RT_StackFrame& stack, CLR_INT32 errorCode )
{        
    NATIVE_PROFILE_CLR_NETWORK();
    CLR_RT_HeapBlock& res = stack.m_owningThread->m_currentException;
                            
    if((Library_corlib_native_System_Exception::CreateInstance( res, g_CLR_RT_WellKnownTypes.m_SocketException, CLR_E_FAIL, &stack )) == S_OK)
    {
         res.Dereference()[ Library_sys_net_native_System_Net_Sockets_SocketException::FIELD___errorCode ].SetInteger( errorCode );
    }
}

HRESULT Library_sys_net_native_System_Net_Sockets_NativeSocket::ThrowOnError( CLR_RT_StackFrame& stack, CLR_INT32 res )
{        
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    if(res == SOCK_SOCKET_ERROR)
    {                
        CLR_INT32 err = SOCK_getlasterror();

        ThrowError( stack, err );

        NANOCLR_SET_AND_LEAVE( CLR_E_PROCESS_EXCEPTION );
    }

    NANOCLR_NOCLEANUP();
}
