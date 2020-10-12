//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_ti_easylink.h"

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
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_AbsoluteTime___U4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_Rssi___I1,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_Frequency___U4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_RfPower___I1,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_IeeeAddress___SZARRAY_U1,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::GetConfiguration___U4__nanoFrameworkTIEasyLinkControlOption,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::DisposeNative___VOID,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::UpdateRxAddressFilterNative___VOID,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::InitNative___U1,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::ReceiveNative___U1__BYREF_nanoFrameworkTIEasyLinkReceivedPacket__SystemTimeSpan,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::SetConfigurationNative___U1__nanoFrameworkTIEasyLinkControlOption__U4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::SetFrequencyNative___U1__U4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::SetRfPowerNative___U1__I1,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::TransmitNative___U1__nanoFrameworkTIEasyLinkTransmitPacket__SystemTimeSpan__SystemTimeSpan,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_TI_EasyLink =
{
    "nanoFramework.TI.EasyLink",
    0x948DBCF9,
    method_lookup,
    { 100, 0, 0, 3 }
};

// clang-format on
