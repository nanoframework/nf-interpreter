﻿//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_net_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::InitializeNetworkInterfaceSettings___VOID,
    Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::UpdateConfiguration___VOID__I4,
    NULL,
    Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetNetworkInterfaceCount___STATIC__I4,
    Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::GetNetworkInterface___STATIC__SystemNetNetworkInformationNetworkInterface__U4,
    Library_sys_net_native_System_Net_NetworkInformation_NetworkInterface::IPAddressFromString___STATIC__U4__STRING,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::GetWireless82011ConfigurationCount___STATIC__I4,
    Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::GetWireless82011Configuration___STATIC__SystemNetNetworkInformationWireless80211Configuration__I4,
    Library_sys_net_native_System_Net_NetworkInformation_Wireless80211Configuration::UpdateConfiguration___STATIC__VOID,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::GetWirelessAPConfigurationCount___STATIC__I4,
    Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::GetWirelessAPConfiguration___STATIC__SystemNetNetworkInformationWirelessAPConfiguration__I4,
    Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::UpdateConfiguration___STATIC__VOID,
    Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::NativeGetConnectedClients___STATIC__SZARRAY_SystemNetNetworkInformationWirelessAPStation__I4,
    Library_sys_net_native_System_Net_NetworkInformation_WirelessAPConfiguration::NativeDeauthStation___STATIC__STRING__I4,
    NULL,
    NULL,
    Library_sys_net_native_System_Net_Security_CertificateManager::AddCaCertificateBundle___STATIC__BOOLEAN__SZARRAY_U1,
    Library_sys_net_native_System_Net_Security_SslNative::SecureServerInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate,
    Library_sys_net_native_System_Net_Security_SslNative::SecureClientInit___STATIC__I4__I4__I4__SystemSecurityCryptographyX509CertificatesX509Certificate__SystemSecurityCryptographyX509CertificatesX509Certificate,
    Library_sys_net_native_System_Net_Security_SslNative::SecureAccept___STATIC__VOID__I4__OBJECT,
    Library_sys_net_native_System_Net_Security_SslNative::SecureConnect___STATIC__VOID__I4__STRING__OBJECT,
    Library_sys_net_native_System_Net_Security_SslNative::SecureRead___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4,
    Library_sys_net_native_System_Net_Security_SslNative::SecureWrite___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4,
    Library_sys_net_native_System_Net_Security_SslNative::SecureCloseSocket___STATIC__I4__OBJECT,
    Library_sys_net_native_System_Net_Security_SslNative::ExitSecureContext___STATIC__I4__I4,
    Library_sys_net_native_System_Net_Security_SslNative::DataAvailable___STATIC__I4__OBJECT,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate::ParseCertificate___STATIC__VOID__SZARRAY_U1__STRING__BYREF_STRING__BYREF_STRING__BYREF_SystemDateTime__BYREF_SystemDateTime,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::socket___STATIC__I4__I4__I4__I4,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::bind___STATIC__VOID__OBJECT__SystemNetEndPoint,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::connect___STATIC__VOID__OBJECT__SystemNetEndPoint__BOOLEAN,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::send___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::recv___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::close___STATIC__I4__OBJECT,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::listen___STATIC__VOID__OBJECT__I4,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::accept___STATIC__I4__OBJECT,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::getaddrinfo___STATIC__VOID__STRING__BYREF_STRING__BYREF_SZARRAY_SZARRAY_U1,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::shutdown___STATIC__VOID__OBJECT__I4__BYREF_I4,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::sendto___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__SystemNetEndPoint,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::recvfrom___STATIC__I4__OBJECT__SZARRAY_U1__I4__I4__I4__I4__BYREF_SystemNetEndPoint,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::getpeername___STATIC__VOID__OBJECT__BYREF_SystemNetEndPoint,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::getsockname___STATIC__VOID__OBJECT__BYREF_SystemNetEndPoint,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::getsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::setsockopt___STATIC__VOID__OBJECT__I4__I4__SZARRAY_U1,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::poll___STATIC__BOOLEAN__OBJECT__I4__I4,
    Library_sys_net_native_System_Net_Sockets_NativeSocket::ioctl___STATIC__VOID__OBJECT__U4__BYREF_U4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_sys_net_native_System_Security_Cryptography_X509Certificates_X509Certificate2::DecodePrivateKeyNative___STATIC__VOID__SZARRAY_U1__STRING,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Net =
{
    "System.Net",
    0x1118F266,
    method_lookup,
    { 100, 1, 3, 1 }
};
