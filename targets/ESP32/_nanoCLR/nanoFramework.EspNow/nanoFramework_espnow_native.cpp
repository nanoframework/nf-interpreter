// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.

#include "nanoFramework_espnow_native.h"

// clang-format off

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
    NULL,
    NULL,
    Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeInitialize___I4,
    Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeDispose___VOID__BOOLEAN,
    Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4,
    Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeEspNowAddPeer___I4__SZARRAY_U1__U1__BOOLEAN__SZARRAY_U1,
    Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeGetMaximumDataLength___I4,
    Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeReadPacket___I4__SZARRAY_U1__SZARRAY_U1__I4,
    Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeGetReceiveOverflowCount___I4,
    Library_nf_espnow_nanoFramework_EspNow_EspNowController::NativeReadSendStatus___I4__SZARRAY_U1,
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
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_EspNow =
{
    "nanoFramework.EspNow",
    0x65E7BDBE,
    method_lookup,
    { 100, 0, 0, 1 }
};

// clang-format on
