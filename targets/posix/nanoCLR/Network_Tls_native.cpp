//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include <Target_Tls.h>
#include <sys_net_native.h>

using NativeTls = Library_sys_net_native_System_Net_Security_SslNative;
using NativeSocket = Library_sys_net_native_System_Net_Sockets_NativeSocket;
using ManagedCertificate = Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate;
using ManagedCertificate2 = Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2;

HRESULT NativeTls::
    SecureClientInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    int verification = stack.Arg1().NumericByRef().s4;
    CLR_RT_HeapBlock *ca = stack.Arg3().Dereference();
    CLR_RT_HeapBlock_Array *data = nullptr;
    int context;
    if ((verification != 0 && verification != 1 && verification != 2 && verification != 4 && verification != 6) ||
        stack.Arg2().Dereference() != nullptr || stack.Arg4().NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }
    if (ca != nullptr && verification != 1)
    {
        data = ca[ManagedCertificate::FIELD___certificate].DereferenceArray();
        FAULT_ON_NULL_ARG(data);
        if (data->m_numOfElements == 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
    }
    context = LinuxTls_CreateContext(
        stack.Arg0().NumericByRef().s4,
        data == nullptr ? nullptr : data->GetFirstElement(),
        data == nullptr ? 0 : data->m_numOfElements,
        verification != 1);
    if (context < 0)
    {
        NANOCLR_CHECK_HRESULT(ThrowCryptographicError(stack, LinuxTls_LastError()));
    }
    stack.SetResult_I4(context);
    NANOCLR_NOCLEANUP();
}

HRESULT NativeTls::
    SecureServerInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate__BOOLEAN(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *identity = stack.Arg2().Dereference();
    CLR_RT_HeapBlock_Array *certificate = nullptr;
    CLR_RT_HeapBlock_Array *privateKey = nullptr;
    CLR_RT_HeapBlock *password = nullptr;
    CLR_RT_TypeDef_Index certificateType;
    int context;
    if (stack.Arg1().NumericByRef().s4 != 1 || stack.Arg3().Dereference() != nullptr ||
        stack.Arg4().NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
    }
    FAULT_ON_NULL_ARG(identity);
    if (!g_CLR_RT_TypeSystem
             .FindTypeDef("X509Certificate2", "System.Security.Cryptography.X509Certificates", certificateType) ||
        !CLR_RT_ExecutionEngine::IsInstanceOf(identity->ObjectCls(), certificateType))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    certificate = identity[ManagedCertificate::FIELD___certificate].DereferenceArray();
    privateKey = identity[ManagedCertificate2::FIELD___privateKey].DereferenceArray();
    password = identity[ManagedCertificate2::FIELD___password].DereferenceString();
    FAULT_ON_NULL_ARG(certificate);
    FAULT_ON_NULL_ARG(privateKey);
    context = LinuxTls_CreateServerContext(
        stack.Arg0().NumericByRef().s4,
        certificate->GetFirstElement(),
        certificate->m_numOfElements,
        privateKey->GetFirstElement(),
        privateKey->m_numOfElements,
        password == nullptr ? nullptr : password->StringText());
    if (context < 0)
    {
        NANOCLR_CHECK_HRESULT(ThrowCryptographicError(stack, LinuxTls_LastError()));
    }
    stack.SetResult_I4(context);
    NANOCLR_NOCLEANUP();
}

HRESULT ManagedCertificate2::DecodePrivateKeyNative___STATIC__VOID__SZARRAY_U1__STRING(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock_Array *key = stack.Arg0().DereferenceArray();
    const char *password = stack.Arg1().RecoverString();
    FAULT_ON_NULL_ARG(key);
    if (!LinuxTls_DecodePrivateKey(key->GetFirstElement(), key->m_numOfElements, password))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    NANOCLR_NOCLEANUP();
}

namespace
{
HRESULT Handshake(CLR_RT_StackFrame &stack, bool server)
{
    NANOCLR_HEADER();
    int context = stack.Arg0().NumericByRef().s4;
    CLR_RT_HeapBlock *socket = (server ? stack.Arg1() : stack.Arg2()).Dereference();
    CLR_RT_HeapBlock timeoutValue;
    CLR_INT64 *timeout;
    bool signalled = true;
    int handle;
    int result;
    if (!server && stack.Arg1().RecoverString() == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_ARGUMENT_NULL);
    }
    FAULT_ON_NULL(socket);
    handle = socket[NativeSocket::FIELD__m_Handle].NumericByRef().s4;
    if (handle == NativeSocket::DISPOSED_HANDLE)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }
    timeoutValue.SetInteger(static_cast<CLR_INT64>(30000) * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(timeoutValue, timeout));
    for (;;)
    {
        if (!signalled)
        {
            NativeSocket::ThrowError(stack, SOCK_ETIMEDOUT);
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }
        result =
            server ? LinuxTls_Accept(context, handle) : LinuxTls_Connect(context, handle, stack.Arg1().RecoverString());
        if (result == 0)
        {
            break;
        }
        if (result == LinuxTls_Error)
        {
            NANOCLR_CHECK_HRESULT(NativeTls::ThrowCryptographicError(stack, LinuxTls_LastError()));
        }
        NANOCLR_CHECK_HRESULT(
            g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, signalled));
    }
    stack.PopValue();
    NANOCLR_NOCLEANUP();
}
} // namespace

HRESULT NativeTls::SecureAccept___STATIC__VOID__I4__OBJECT(CLR_RT_StackFrame &stack)
{
    return Handshake(stack, true);
}

HRESULT NativeTls::SecureConnect___STATIC__VOID__I4__STRING__OBJECT(CLR_RT_StackFrame &stack)
{
    return Handshake(stack, false);
}

HRESULT NativeTls::ReadWriteHelper(CLR_RT_StackFrame &stack, bool write)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
    CLR_RT_HeapBlock_Array *data = stack.Arg1().DereferenceArray();
    int offset = stack.Arg2().NumericByRef().s4;
    int length = stack.Arg3().NumericByRef().s4;
    int handle;
    CLR_RT_HeapBlock timeoutValue;
    CLR_INT64 *timeout;
    bool signalled = true;
    int total;
    int result;
    FAULT_ON_NULL(socket);
    FAULT_ON_NULL_ARG(data);
    if (offset < 0 || length < 0 || static_cast<CLR_UINT32>(offset) > data->m_numOfElements ||
        static_cast<CLR_UINT32>(length) > data->m_numOfElements - offset || data->m_sizeOfElement != 1)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INDEX_OUT_OF_RANGE);
    }
    handle = socket[NativeSocket::FIELD__m_Handle].NumericByRef().s4;
    if (handle == NativeSocket::DISPOSED_HANDLE)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }
    timeoutValue.SetInteger(static_cast<CLR_INT64>(stack.Arg4().NumericByRef().s4) * TIME_CONVERSION__TO_MILLISECONDS);
    NANOCLR_CHECK_HRESULT(stack.SetupTimeoutFromTicks(timeoutValue, timeout));
    if (stack.m_customState == 1)
    {
        stack.PushValueI4(0);
        stack.m_customState = 2;
    }
    total = stack.m_evalStack[1].NumericByRef().s4;
    while (total < length)
    {
        if (!signalled)
        {
            NativeSocket::ThrowError(stack, SOCK_ETIMEDOUT);
            NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
        }
        auto buffer = stack.Arg1().DereferenceArray()->GetFirstElement() + offset + total;
        result = write ? LinuxTls_Write(handle, buffer, length - total) : LinuxTls_Read(handle, buffer, length - total);
        if (result == LinuxTls_Error)
        {
            int error = LinuxTls_LastError();
            if (error > 0)
            {
                NativeSocket::ThrowError(stack, error);
                NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
            }
            NANOCLR_CHECK_HRESULT(ThrowCryptographicError(stack, error));
        }
        if (result == LinuxTls_WantRead || result == LinuxTls_WantWrite)
        {
            NANOCLR_CHECK_HRESULT(
                g_CLR_RT_ExecutionEngine.WaitEvents(stack.m_owningThread, *timeout, Event_Socket, signalled));
            continue;
        }
        if (result == 0)
        {
            break;
        }
        total += result;
        stack.m_evalStack[1].NumericByRef().s4 = total;
        if (!write)
        {
            break;
        }
    }
    stack.PopValue();
    stack.PopValue();
    stack.SetResult_I4(total);
    NANOCLR_NOCLEANUP();
}

HRESULT NativeTls::SecureRead___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    return ReadWriteHelper(stack, false);
}

HRESULT NativeTls::SecureWrite___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4(CLR_RT_StackFrame &stack)
{
    return ReadWriteHelper(stack, true);
}

HRESULT NativeTls::DataAvailable___STATIC__I4__OBJECT(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
    int result;
    FAULT_ON_NULL(socket);
    result = LinuxTls_Available(socket[NativeSocket::FIELD__m_Handle].NumericByRef().s4);
    if (result < 0)
    {
        NANOCLR_CHECK_HRESULT(ThrowCryptographicError(stack, LinuxTls_LastError()));
    }
    stack.SetResult_I4(result);
    NANOCLR_NOCLEANUP();
}

HRESULT NativeTls::SecureCloseSocket___STATIC__I4__OBJECT(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *socket = stack.Arg0().Dereference();
    FAULT_ON_NULL(socket);
    stack.SetResult_I4(SOCK_close(socket[NativeSocket::FIELD__m_Handle].NumericByRef().s4));
    NANOCLR_NOCLEANUP();
}

HRESULT NativeTls::ExitSecureContext___STATIC__I4__I4(CLR_RT_StackFrame &stack)
{
    stack.SetResult_I4(LinuxTls_FreeContext(stack.Arg0().NumericByRef().s4) ? 0 : -1);
    return S_OK;
}

HRESULT NativeTls::ThrowCryptographicError(CLR_RT_StackFrame &stack, int error)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock &exception = stack.m_owningThread->m_currentException;
    NANOCLR_CHECK_HRESULT(Library_corlib_native_System_Exception::CreateInstance(
        exception,
        g_CLR_RT_WellKnownTypes.CryptographicException,
        CLR_E_FAIL,
        &stack));
    exception
        .Dereference()[Library_sys_net_native_System_Security_Cryptography_CryptographicException::FIELD___errorCode]
        .SetInteger(error);
    NANOCLR_SET_AND_LEAVE(CLR_E_PROCESS_EXCEPTION);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_net_native_System_Net_Security_CertificateManager::
    AddCaCertificateBundle___STATIC__BOOLEAN__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock_Array *data = stack.Arg0().DereferenceArray();
    FAULT_ON_NULL_ARG(data);
    if (!LinuxTls_SetCaBundle(data->GetFirstElement(), data->m_numOfElements))
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }
    stack.SetResult_Boolean(true);
    NANOCLR_NOCLEANUP();
}

bool SSL_ParseCertificate(const char *data, size_t length, X509CertData *certificate)
{
    return certificate != nullptr &&
           LinuxTls_ParseCertificate(reinterpret_cast<const uint8_t *>(data), length, *certificate);
}
