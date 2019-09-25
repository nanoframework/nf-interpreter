//
// Copyright (c) 2019 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_ti_easylink.h"

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_AbsoluteTime___U4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_Rssi___I1,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_Frequency___U4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::set_Frequency___VOID__U4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_RfPower___I1,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::set_RfPower___VOID__I1,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::get_IeeeAddress___SZARRAY_U1,
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
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::DisposeNative___VOID,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::UpdateRxAddressFilterNative___VOID,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::InitNative___I4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::ReceiveNative___I4__BYREF_nanoFrameworkTIEasyLinkReceivedPacket__I4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::SetConfigurationNative___I4__nanoFrameworkTIEasyLinkControlOption__U4,
    Library_nf_ti_easylink_nanoFramework_TI_EasyLink_EasyLinkController::TransmitNative___I4__nanoFrameworkTIEasyLinkTransmitPacket__I4__I4,
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

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_TI_EasyLink =
{
    "nanoFramework.TI.EasyLink", 
    0x55A7E273,
    method_lookup,
    { 100, 0, 0, 0 }
};
