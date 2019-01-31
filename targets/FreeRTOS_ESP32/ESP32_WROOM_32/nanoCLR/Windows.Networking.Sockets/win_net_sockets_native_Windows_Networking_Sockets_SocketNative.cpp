//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "win_net_sockets_native.h"

// TODO -  Check if required, for now just ignore
#define SwapEndianIfBEc16(x)    (x)

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::socket___STATIC__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
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

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::bind___STATIC__VOID__OBJECT__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    return BindConnectHelper( stack, true );
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::connect___STATIC__VOID__OBJECT__SZARRAY_U1__BOOLEAN( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    return BindConnectHelper( stack, false );
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::send___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::recv___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::close___STATIC__I4__OBJECT( CLR_RT_StackFrame& stack )
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

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::listen___STATIC__VOID__OBJECT__I4( CLR_RT_StackFrame& stack )
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

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::accept___STATIC__I4__OBJECT( CLR_RT_StackFrame& stack )
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

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::getaddrinfo___STATIC__VOID__STRING__BYREF_STRING__BYREF_SZARRAY_SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::shutdown___STATIC__VOID__OBJECT__I4__BYREF_I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::sendto___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::recvfrom___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__BYREF_SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::getpeername___STATIC__VOID__OBJECT__BYREF_SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::getsockname___STATIC__VOID__OBJECT__BYREF_SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::getsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::setsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::poll___STATIC__BOOLEAN__OBJECT__I4__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::ioctl___STATIC__VOID__OBJECT__U4__BYREF_U4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}



//--//

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::BindConnectHelper( CLR_RT_StackFrame& stack, bool fBind )
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
        
        if(!fThrowOnWouldBlock && SOCK_getlasterror() == SOCK_EWOULDBLOCK)
        {
            NANOCLR_SET_AND_LEAVE(S_OK);            
        }
    }

    NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::MarshalSockAddress( CLR_RT_HeapBlock& blkDst, const struct SOCK_sockaddr* addrSrc, CLR_UINT32 addrLenSrc )
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* arr = NULL;

    CLR_RT_HeapBlock blkArr; blkArr.SetObjectReference( NULL );
    CLR_RT_ProtectFromGC gc( blkArr );
    SOCK_sockaddr_in* dst;
    SOCK_sockaddr_in* src = (SOCK_sockaddr_in*)addrSrc;
        
    NANOCLR_CHECK_HRESULT(CLR_RT_HeapBlock_Array::CreateInstance( blkArr, addrLenSrc, g_CLR_RT_WellKnownTypes.m_UInt8 ));
    
    arr = blkArr.DereferenceArray();

    _ASSERTE(arr);

    dst = (SOCK_sockaddr_in*)arr->GetFirstElement();

    dst->sin_family           = SwapEndianIfBEc16(src->sin_family);
    dst->sin_port             = src->sin_port;
    dst->sin_addr.S_un.S_addr = src->sin_addr.S_un.S_addr;

    memcpy(dst->sin_zero, src->sin_zero, sizeof(dst->sin_zero));

    _ASSERTE(blkDst.DataType() == DATATYPE_BYREF || blkDst.DataType() == DATATYPE_ARRAY_BYREF);

    NANOCLR_CHECK_HRESULT(blkArr.StoreToReference( blkDst, 0 ));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::MarshalSockAddress( struct SOCK_sockaddr* addrDst, CLR_UINT32& addrLen, const CLR_RT_HeapBlock& blkSockAddress )
{        
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock_Array* ptrSockAddress;
    SOCK_sockaddr_in* dst = (SOCK_sockaddr_in*)addrDst;
    SOCK_sockaddr_in* src;    

    ptrSockAddress = blkSockAddress.DereferenceArray();                    
    FAULT_ON_NULL(ptrSockAddress);

    if(ptrSockAddress->m_numOfElements > addrLen) NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);

    src = (SOCK_sockaddr_in*)ptrSockAddress->GetFirstElement();

    dst->sin_family           = SwapEndianIfBEc16(src->sin_family);
    dst->sin_port             = src->sin_port;
    dst->sin_addr.S_un.S_addr = src->sin_addr.S_un.S_addr; //already in network byte order

    memcpy(dst->sin_zero, src->sin_zero, sizeof(dst->sin_zero));

    addrLen = ptrSockAddress->m_numOfElements;

    NANOCLR_NOCLEANUP();
}

// HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::SendRecvHelper( CLR_RT_StackFrame& stack, bool fSend, bool fAddress )
// {
//     NATIVE_PROFILE_CLR_NETWORK();
//     NANOCLR_HEADER();

//     CLR_RT_HeapBlock*       socket    = stack.Arg0().Dereference();
//     CLR_INT32               handle;
//     CLR_RT_HeapBlock_Array* arrData   = stack.Arg1().DereferenceArray(); 
//     CLR_UINT32              offset    = stack.Arg2().NumericByRef().u4;
//     CLR_UINT32              count     = stack.Arg3().NumericByRef().u4;
//     CLR_INT32               flags     = stack.Arg4().NumericByRef().s4;
//     CLR_INT32               timeout_ms = stack.Arg5().NumericByRef().s4;
//     CLR_RT_HeapBlock        hbTimeout;

//     CLR_INT64* timeout;
//     CLR_UINT8* buf;
//     bool       fRes = true;
//     CLR_INT32  totReadWrite;
//     CLR_INT32  ret = 0;

//     FAULT_ON_NULL(socket);
//     FAULT_ON_NULL(arrData);
    
//     handle = socket[ FIELD__m_Handle ].NumericByRef().s4;

//     /* Because we could have been a rescheduled call due to a prior call that would have blocked, we need to see
//      * if our handle has been shutdown before continuing. */
//     if (handle == DISPOSED_HANDLE)
//     {
//         ThrowError( stack, CLR_E_OBJECT_DISPOSED );
//         NANOCLR_SET_AND_LEAVE (CLR_E_PROCESS_EXCEPTION);
//     }

//     if(offset + count > arrData->m_numOfElements) NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);    


//     hbTimeout.SetInteger( timeout_ms );
        
//     NANOCLR_CHECK_HRESULT(stack.SetupTimeout( hbTimeout, timeout ));

//     //
//     // Push "totReadWrite" onto the eval stack.
//     //
//     if(stack.m_customState == 1)
//     {
//         stack.PushValueI4( 0 );
        
//         stack.m_customState = 2;
//     }

//     totReadWrite = stack.m_evalStack[ 1 ].NumericByRef().s4;

//     buf    = arrData->GetElement( offset + totReadWrite );
//     count -= totReadWrite;

//     while(count > 0)
//     {
//         CLR_INT32 bytes = 0;

//         // first make sure we have data to read or ability to write
//         while(fRes)
//         {
//             ret = Helper__SelectSocket( handle, fSend ? 1 : 0 );

//             if(ret != 0) break;

//             // non-blocking - allow other threads to run while we wait for handle activity
//             NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.WaitEvents( stack.m_owningThread, *timeout, CLR_RT_ExecutionEngine::c_Event_Socket, fRes ));
//         }

//         // timeout expired
//         if(!fRes)
//         {
//             ret = SOCK_SOCKET_ERROR;
            
//             ThrowError( stack, SOCK_ETIMEDOUT );

//             NANOCLR_SET_AND_LEAVE( CLR_E_PROCESS_EXCEPTION );
//         }

//         // socket is in the excepted state, so let's bail out
//         if(SOCK_SOCKET_ERROR == ret)
//         {
//             break;
//         }

//         if(fAddress)
//         {
//             struct SOCK_sockaddr addr;
//             CLR_UINT32 addrLen = sizeof(addr);
//             CLR_RT_HeapBlock& blkAddr = stack.Arg6();

//             if(fSend)
//             {
//                 NANOCLR_CHECK_HRESULT(MarshalSockAddress( &addr, addrLen, blkAddr ));

//                 bytes = SOCK_sendto( handle, (const char*)buf, count, flags, &addr, addrLen );
//             }
//             else
//             {
//                 CLR_RT_HeapBlock* pBlkAddr = blkAddr.Dereference();
                
//                 NANOCLR_CHECK_HRESULT(MarshalSockAddress( &addr, addrLen, *pBlkAddr ));

//                 bytes = SOCK_recvfrom( handle, (char*)buf, count, flags, &addr, (int*)&addrLen );

//                 if(bytes != SOCK_SOCKET_ERROR)
//                 {
//                     NANOCLR_CHECK_HRESULT(MarshalSockAddress( blkAddr, &addr, addrLen ));
//                 }
//             }
//         }
//         else
//         {
//             if(fSend)
//             {
//                 bytes = SOCK_send( handle, (const char*)buf, count, flags );
//             }
//             else
//             {
//                 bytes = SOCK_recv( handle, (char*)buf, count, flags );
//             }
//         }

//         // send/recv/sendto/recvfrom failed
//         if(bytes == SOCK_SOCKET_ERROR)
//         {
//             CLR_INT32 err = SOCK_getlasterror();
            
//             if(err != SOCK_EWOULDBLOCK)
//             {
//                 ret = SOCK_SOCKET_ERROR;
//                 break;
//             }
            
//             continue;
//         }
//                 // zero recv bytes indicates the handle has been closed.
//         else if(!fSend && (bytes == 0)) 
//         {
//             break;
//         }
        
//         buf          += bytes;
//         totReadWrite += bytes;
//         count        -= bytes;

//         stack.m_evalStack[ 1 ].NumericByRef().s4 = totReadWrite;        

//         // receive returns immediately after receiving bytes.
//         if(!fSend && (totReadWrite > 0))
//         {
//             break;
//         }

//     }

//     stack.PopValue();       // totReadWrite
//     stack.PopValue();       // Timeout
    
//     NANOCLR_CHECK_HRESULT(ThrowOnError( stack, ret ));

//     stack.SetResult_I4( totReadWrite );

//     NANOCLR_NOCLEANUP();
// }

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::SockOptHelper( CLR_RT_StackFrame& stack, bool fGet )
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

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::SockNameHelper( CLR_RT_StackFrame& stack, bool fPeer )
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




void Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::ThrowError( CLR_RT_StackFrame& stack, CLR_INT32 errorCode )
{        
    NATIVE_PROFILE_CLR_NETWORK();
    // CLR_RT_HeapBlock& res = stack.m_owningThread->m_currentException;
                            
    // if((Library_corlib_native_System_Exception::CreateInstance( res, g_CLR_RT_WellKnownTypes.m_SocketException, CLR_E_FAIL, &stack )) == S_OK)
    // {
    //     res.Dereference()[ Library_system_sockets_System_Net_Sockets_SocketException::FIELD___errorCode ].SetInteger( errorCode );
    // }
}

HRESULT Library_win_net_sockets_native_Windows_Networking_Sockets_SocketNative::ThrowOnError( CLR_RT_StackFrame& stack, CLR_INT32 res )
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