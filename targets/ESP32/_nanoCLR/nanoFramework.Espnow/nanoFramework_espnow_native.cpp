// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

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
    Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeInitialize___I4,
    Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeDispose___VOID__BOOLEAN,
    Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeEspNowSend___I4__SZARRAY_U1__SZARRAY_U1__I4,
    Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController::NativeEspNowAddPeer___I4__SZARRAY_U1__U1__BOOLEAN__SZARRAY_U1,
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
    Library_nanoFramework_EspNow_native_nanoFramework_EspNow_EspNowController__EspNowEventHandler::ProcessEvent___nanoFrameworkRuntimeEventsBaseEvent__U4__U4__SystemDateTime,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_EspNow =
{
    "nanoFramework.EspNow",
    0xC6C47E7A,
    method_lookup,
    { 100, 0, 0, 0 }
};

// clang-format on
