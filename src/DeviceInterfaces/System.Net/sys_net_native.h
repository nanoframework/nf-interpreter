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

struct Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration
{
    static const int FIELD___configurationIndex = 1;
    static const int FIELD___id = 2;
    static const int FIELD___authentication = 3;
    static const int FIELD___encryption = 4;
    static const int FIELD___radio = 5;
    static const int FIELD___password = 6;
    static const int FIELD___ssid = 7;

    NANOCLR_NATIVE_DECLARE(GetWireless82011ConfigurationCount___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(GetWireless82011Configuration___STATIC__SystemNetNetworkInformationWireless80211Configuration__I4);
    NANOCLR_NATIVE_DECLARE(UpdateConfiguration___STATIC__VOID);

    //--//

};

struct Library_sys_net_native_System_Net_NetworkInformation_NetworkChange
{
    static const int FIELD_STATIC__NetworkAddressChanged = 0;
    static const int FIELD_STATIC__NetworkAvailabilityChanged = 1;


    //--//

};

struct Library_sys_net_native_System_Net_NetworkInformation_NetworkChange__NetworkEvent
{
    static const int FIELD__EventType = 3;
    static const int FIELD__Flags = 4;
    static const int FIELD__Time = 5;


    //--//

};

struct Library_sys_net_native_System_Net_IPAddress
{
    static const int FIELD_STATIC__Any = 2;
    static const int FIELD_STATIC__Loopback = 3;

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

struct Library_sys_net_native_System_Net_NetworkInformation_NetworkAvailabilityEventArgs
{
    static const int FIELD___isAvailable = 1;


    //--//

};

struct Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface
{
    static const int FIELD___interfaceIndex = 1;
    static const int FIELD___macAddress = 2;
    static const int FIELD___startupAddressMode = 3;
    static const int FIELD___specificConfigId = 4;
    static const int FIELD___automaticDns = 5;
    static const int FIELD___ipv4Address = 6;
    static const int FIELD___ipv4NetMask = 7;
    static const int FIELD___ipv4GatewayAddress = 8;
    static const int FIELD___ipv4dnsAddress1 = 9;
    static const int FIELD___ipv4dnsAddress2 = 10;
    static const int FIELD___ipv6Address = 11;
    static const int FIELD___ipv6NetMask = 12;
    static const int FIELD___ipv6GatewayAddress = 13;
    static const int FIELD___ipv6dnsAddress1 = 14;
    static const int FIELD___ipv6dnsAddress2 = 15;
    static const int FIELD___networkInterfaceType = 16;

    NANOCLR_NATIVE_DECLARE(InitializeNetworkInterfaceSettings___VOID);
    NANOCLR_NATIVE_DECLARE(UpdateConfiguration___VOID__I4);
    NANOCLR_NATIVE_DECLARE(GetNetworkInterfaceCount___STATIC__I4);
    NANOCLR_NATIVE_DECLARE(GetNetworkInterface___STATIC__SystemNetNetworkInformationNetworkInterface__U4);
    NANOCLR_NATIVE_DECLARE(IPAddressFromString___STATIC__U4__STRING);

    //--//

};

struct Library_sys_net_native_System_Net_Security_CertificateManager
{
    NANOCLR_NATIVE_DECLARE(AddCaCertificateBundle___STATIC__BOOLEAN__SZARRAY_U1);

    //--//

};

struct Library_sys_net_native_System_Net_Security_SslNative
{
    NANOCLR_NATIVE_DECLARE(SecureServerInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate);
    NANOCLR_NATIVE_DECLARE(SecureClientInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate);
    NANOCLR_NATIVE_DECLARE(SecureAccept___STATIC__VOID__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(SecureConnect___STATIC__VOID__I4__STRING__OBJECT);
    NANOCLR_NATIVE_DECLARE(SecureRead___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(SecureWrite___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4);
    NANOCLR_NATIVE_DECLARE(SecureCloseSocket___STATIC__I4__OBJECT);
    NANOCLR_NATIVE_DECLARE(ExitSecureContext___STATIC__I4__I4);
    NANOCLR_NATIVE_DECLARE(DataAvailable___STATIC__I4__OBJECT);

    //--//

    static HRESULT ReadWriteHelper( CLR_RT_StackFrame& stack, bool isWrite  );
    static HRESULT InitHelper     ( CLR_RT_StackFrame& stack, bool isServer );
    static HRESULT ThrowOnError   ( CLR_RT_StackFrame& stack, int err       );
    static void    ThrowError     ( CLR_RT_StackFrame& stack, int errorCode );

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

struct Library_sys_net_native_System_Net_Security_SslStream
{
    static const int FIELD___sslVerification = 6;
    static const int FIELD___sslContext = 7;
    static const int FIELD___isServer = 8;


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

struct Library_sys_net_native_System_Net_Sockets_Socket
{
    static const int FIELD__m_Handle = 1;
    static const int FIELD__m_fBlocking = 2;
    static const int FIELD__m_localEndPoint = 3;
    static const int FIELD__m_recvTimeout = 4;
    static const int FIELD__m_sendTimeout = 5;
    static const int FIELD___socketType = 6;


    //--//

};

struct Library_sys_net_native_System_Net_Sockets_SocketException
{
    static const int FIELD___errorCode = 5;


    //--//

};

struct Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate
{
    static const int FIELD___certificate = 1;
    static const int FIELD___password = 2;
    static const int FIELD___issuer = 3;
    static const int FIELD___subject = 4;
    static const int FIELD___effectiveDate = 5;
    static const int FIELD___expirationDate = 6;
    static const int FIELD___handle = 7;
    static const int FIELD___sessionHandle = 8;

    NANOCLR_NATIVE_DECLARE(ParseCertificate___STATIC__VOID__SZARRAY_U1__STRING__BYREF_STRING__BYREF_STRING__BYREF_mscorlibSystemDateTime__BYREF_mscorlibSystemDateTime);

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Net;

#endif  //_SYS_NET_NATIVE_H_
