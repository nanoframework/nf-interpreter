//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"

// remap types to ease code readability
typedef Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate X509Certificate;
typedef Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2 X509Certificate2;

//--//

void Time_GetDateTime(DATE_TIME_INFO *dt)
{
    NATIVE_PROFILE_CLR_NETWORK();
    SYSTEMTIME st;

    HAL_Time_ToSystemTime(HAL_Time_CurrentTime(), &st);

    // TODO check if offset needed for security date time, maybe needed to certificate validation
    //    dt->tzOffset = Time_GetTimeZoneOffset() * 60;
    dt->tzOffset = 0;

    dt->year = st.wYear;
    dt->month = st.wMonth;
    dt->day = st.wDay;
    dt->hour = st.wHour;
    dt->minute = st.wMinute;
    dt->second = st.wSecond;
    dt->msec = st.wMilliseconds;

    dt->dlsTime = 0;
}

// Initalise SSL as a server
HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureServerInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    return InitHelper(stack, true);
}

// Initalise SSL as a client
HRESULT Library_sys_net_native_System_Net_Security_SslNative::
    SecureClientInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    return InitHelper(stack, false);
}

//
//  Server - socket connected now accept connection by doing the server SSL handshake
//
HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureAccept___STATIC__VOID__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
    CLR_RT_HeapBlock *socket = stack.Arg1().Dereference();
    CLR_INT32 timeout_ms = -1; // wait forever
    CLR_RT_HeapBlock hbTimeout;

    int result = 0;
    CLR_INT32 handle;
    bool fRes = true;
    CLR_INT64 *timeout;

    FAULT_ON_NULL(socket);

    handle = socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;

    /* Because we could have been a rescheduled call due to a prior call that would have blocked, we need to see
     * if our handle has been shutdown before continuing. */
    if (handle == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
    {
        ThrowError(stack, CLR_E_OBJECT_DISPOSED);
        NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
    }

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    // first make sure we have data to read or ability to write
    while (true)
    {
        result = SSL_Accept(handle, sslContext);

        if (result == SOCK_EWOULDBLOCK || result == SOCK_TRY_AGAIN)
        {
            // non-blocking - allow other threads to run while we wait for socket activity
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));
        }
        else
        {
            break;
        }
    }

    stack.PopValue(); // Timeout

    NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));

    NANOCLR_NOCLEANUP();
}

//
//  Client - Natve socket connected, now do client side SSL handshake
//
HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureConnect___STATIC__VOID__I4__STRING__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;
    CLR_RT_HeapBlock *hb = stack.Arg1().DereferenceString();
    CLR_RT_HeapBlock *socket = stack.Arg2().Dereference();
    CLR_RT_HeapBlock hbTimeout;

    int result;
    const char *szName;
    CLR_INT32 handle;
    bool fRes = true;
    CLR_INT64 *timeout;

    FAULT_ON_NULL(socket);

    handle = socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;

    /* Because we could have been a rescheduled call due to a prior call that would have blocked, we need to see
     * if our handle has been shutdown before continuing. */
    if (handle == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
    {
        ThrowError(stack, CLR_E_OBJECT_DISPOSED);
        NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
    }

    FAULT_ON_NULL_ARG(hb);

    szName = hb->StringText();

    // Infinite Timeout
    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)-1);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    while (true)
    {
        result = SSL_Connect(handle, szName, sslContext);

        if (result == SOCK_EWOULDBLOCK || result == SOCK_TRY_AGAIN)
        {
            // non-blocking - allow other threads to run while we wait for socket activity
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));

            if (result < 0)
                break;
        }
        else
        {
            break;
        }
    }

    stack.PopValue(); // Timeout

    NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureRead___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    return ReadWriteHelper(stack, false);
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureWrite___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    return ReadWriteHelper(stack, true);
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::SecureCloseSocket___STATIC__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    int result;
    CLR_INT32 handle;

    CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
    FAULT_ON_NULL(socket);

    handle = socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;

    result = SSL_CloseSocket(handle);

    stack.SetResult_I4(result);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::ExitSecureContext___STATIC__I4__I4(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_INT32 sslContext = stack.Arg0().NumericByRef().s4;

    int result = SSL_ExitContext(sslContext) == TRUE ? 0 : -1;

    stack.SetResult_I4(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::DataAvailable___STATIC__I4__OBJECT(
    CLR_RT_StackFrame &stack)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
    int result;
    CLR_INT32 handle;

    FAULT_ON_NULL(socket);

    handle = socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;

    result = SSL_DataAvailable(handle);

    // ThrowOnError expects anything other than 0 to be a failure - so return 0 if we don't have an error
    NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result >= 0 ? 0 : result));

    stack.SetResult_I4(result);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::ReadWriteHelper(CLR_RT_StackFrame &stack, bool isWrite)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
    CLR_RT_HeapBlock_Array *arrData = stack.Arg1().DereferenceArray();
    CLR_INT32 offset = stack.Arg2().NumericByRef().s4;
    CLR_INT32 count = stack.Arg3().NumericByRef().s4;
    CLR_INT32 timeout_ms = stack.Arg4().NumericByRef().s4;
    CLR_UINT8 *buffer;
    CLR_RT_HeapBlock hbTimeout;

    CLR_INT32 totReadWrite;
    bool fRes = true;
    CLR_INT64 *timeout;
    int result = 0;
    CLR_INT32 handle;

    if (count == 0)
    {
        stack.SetResult_I4(0);
        NANOCLR_SET_AND_LEAVE(S_OK);
    }

    FAULT_ON_NULL(socket);

    handle = socket[Library_sys_net_native_System_Net_Sockets_NativeSocket::FIELD__m_Handle].NumericByRef().s4;

    // Check not Disposed
    if (handle == Library_sys_net_native_System_Net_Sockets_NativeSocket::DISPOSED_HANDLE)
    {
        ThrowError(stack, CLR_E_OBJECT_DISPOSED);
        NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
    }

    FAULT_ON_NULL(arrData);

    // !! need to cast to CLR_INT64 otherwise it wont setup a proper timeout infinite
    hbTimeout.SetInteger((CLR_INT64)timeout_ms * TIME_CONVERSION__TO_MILLISECONDS);

    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(hbTimeout, timeout));

    //
    // Push "totReadWrite" onto the eval stack.
    //
    if (stack.m_customState == 1)
    {
        stack.PushValueI4(0);

        stack.m_customState = 2;
    }

    totReadWrite = stack.m_evalStack[1].NumericByRef().s4;

    buffer = arrData->GetElement(offset + totReadWrite);
    count -= totReadWrite;

    if ((offset + count + totReadWrite) > (int)arrData->m_numOfElements)
        NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);

    while (count > 0)
    {
        // first make sure we have data to read or ability to write
        while (fRes)
        {
            if (!isWrite)
            {
                // check SSL_DataAvailable() in case SSL has already read and buffered socket data
                result = SSL_DataAvailable(handle);

                if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
                {
                    break;
                }
            }

            result =
                Library_sys_net_native_System_Net_Sockets_NativeSocket::Helper__SelectSocket(handle, isWrite ? 1 : 0);

            if ((result > 0) || ((result < 0) && (SOCK_getlasterror() != SOCK_EWOULDBLOCK)))
            {
                break;
            }

            // non-blocking - allow other threads to run while we wait for socket activity
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, fRes));

            // timeout expired
            if (!fRes)
            {
                result = SOCK_SOCKET_ERROR;

                ThrowError(stack, SOCK_ETIMEDOUT);

                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }
        }

        // socket is in the excepted state, so let's bail out
        if (SOCK_SOCKET_ERROR == result)
        {
            break;
        }

        if (isWrite)
        {
            result = SSL_Write(handle, (const char *)buffer, count);
        }
        else
        {
            result = SSL_Read(handle, (char *)buffer, count);

            if (result == SSL_RESULT__WOULD_BLOCK)
            {
                continue;
            }
        }

        // ThrowOnError expects anything other than 0 to be a failure - so return 0 if we don't have an error
        if (result <= 0)
        {
            break;
        }

        buffer += result;
        totReadWrite += result;
        count -= result;

        // read is non-blocking if we have any data
        if (!isWrite && (totReadWrite > 0))
        {
            break;
        }

        stack.m_evalStack[1].NumericByRef().s4 = totReadWrite;
    }

    stack.PopValue(); // totReadWrite
    stack.PopValue(); // Timeout

    if (result < 0)
    {
        NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
    }

    stack.SetResult_I4(totReadWrite);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::InitHelper(CLR_RT_StackFrame &stack, bool isServer)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index x509Certificate2TypeDef;

    CLR_INT32 sslContext = -1;
    CLR_INT32 sslMode = stack.Arg0().NumericByRef().s4;
    CLR_INT32 sslVerify = stack.Arg1().NumericByRef().s4;
    CLR_RT_HeapBlock *hbCert = stack.Arg2().Dereference();
    CLR_RT_HeapBlock *caCert = stack.Arg3().Dereference();
    bool useDeviceCertificate = (bool)stack.Arg4().NumericByRef().u1;
    CLR_RT_HeapBlock_Array *arrCert = NULL;
    CLR_RT_HeapBlock_Array *privateKey = NULL;
    CLR_UINT8 *sslCert = NULL;
    volatile int result;
    uint8_t *pk = NULL;
    const char *pkPassword = NULL;
    CLR_UINT32 pkPasswordLength = 0;

    if (hbCert != NULL)
    {
        g_CLR_RT_TypeSystem.FindTypeDef(
            "X509Certificate2",
            "System.Security.Cryptography.X509Certificates",
            x509Certificate2TypeDef);

        arrCert = hbCert[X509Certificate::FIELD___certificate].DereferenceArray();
        arrCert->Pin();

        // there is a client certificate, find if it's a X509Certificate2
        if (hbCert->ObjectCls().Type() == x509Certificate2TypeDef.Type())
        {
            // get private key
            privateKey = hbCert[X509Certificate2::FIELD___privateKey].DereferenceArray();

            // grab the first element, if there is a private key
            if (privateKey)
            {
                pk = privateKey->GetFirstElement();

                // get password field
                CLR_RT_HeapBlock *passwordHb = hbCert[X509Certificate2::FIELD___password].Dereference();

                // get password length, if there is a password
                if (passwordHb)
                {
                    pkPassword = passwordHb->StringText();
                    pkPasswordLength = hal_strlen_s(pkPassword);
                }
            }
        }

        // get certificate
        sslCert = arrCert->GetFirstElement();
    }

    if (isServer)
    {
        result =
            (SSL_ServerInit(
                 sslMode,
                 sslVerify,
                 (const char *)sslCert,
                 sslCert == NULL ? 0 : arrCert->m_numOfElements,
                 pk,
                 pk == NULL ? 0 : privateKey->m_numOfElements,
                 pkPassword,
                 pkPasswordLength,
                 sslContext,
                 useDeviceCertificate)
                 ? 0
                 : -1);
    }
    else
    {
        result =
            (SSL_ClientInit(
                 sslMode,
                 sslVerify,
                 (const char *)sslCert,
                 sslCert == NULL ? 0 : arrCert->m_numOfElements,
                 pk,
                 pk == NULL ? 0 : privateKey->m_numOfElements,
                 pkPassword,
                 pkPasswordLength,
                 sslContext,
                 useDeviceCertificate)
                 ? 0
                 : -1);
    }

    NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));

    if (caCert != NULL)
    {
        arrCert = caCert[X509Certificate::FIELD___certificate].DereferenceArray();

        // If arrCert == NULL then the certificate is an X509Certificate2 which uses a certificate handle
        if (arrCert == NULL)
        {
            arrCert = caCert[X509Certificate::FIELD___handle].DereferenceArray();
            FAULT_ON_NULL(arrCert);

            sslCert = arrCert->GetFirstElement();

            // pass the certificate handle as the data parameter
            result =
                (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

            NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
        }
        else
        {

            arrCert->Pin();

            sslCert = arrCert->GetFirstElement();

            result =
                (SSL_AddCertificateAuthority(sslContext, (const char *)sslCert, arrCert->m_numOfElements) ? 0 : -1);

            NANOCLR_CHECK_HRESULT(ThrowOnError(stack, result));
        }
    }

    stack.SetResult_I4(sslContext);

    NANOCLR_CLEANUP();

    if (FAILED(hr) && (sslContext != -1))
    {
        SSL_ExitContext(sslContext);
    }

    NANOCLR_CLEANUP_END();
}

void Library_sys_net_native_System_Net_Security_SslNative::ThrowError(CLR_RT_StackFrame &stack, int errorCode)
{
    NATIVE_PROFILE_CLR_NETWORK();
    CLR_RT_HeapBlock &res = stack.m_owningThread->m_currentException;

    if ((Library_corlib_native_System_Exception::CreateInstance(
            res,
            g_CLR_RT_WellKnownTypes.m_SocketException,
            CLR_E_FAIL,
            &stack)) == S_OK)
    {
        res.Dereference()[Library_sys_net_native_System_Net_Sockets_SocketException::FIELD___errorCode].SetInteger(
            errorCode);
    }
}

HRESULT Library_sys_net_native_System_Net_Security_SslNative::ThrowOnError(CLR_RT_StackFrame &stack, int res)
{
    NATIVE_PROFILE_CLR_NETWORK();
    NANOCLR_HEADER();

    if (res != 0)
    {
        ThrowError(stack, res);

        NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
    }

    NANOCLR_NOCLEANUP();
}
