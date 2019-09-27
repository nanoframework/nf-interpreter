
//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_rmt_native.h"


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
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeDispose___STATIC__VOID__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetChannelStatus___STATIC__I4__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetClockDivider___STATIC__U1__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetMemoryBlockNumber___STATIC__U1__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetMemoryLowPower___STATIC__BOOLEAN__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeGetSourceClock___STATIC__I4__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetClockDivider___STATIC__VOID__I4__U1,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetMemoryBlockNumber___STATIC__VOID__I4__U1,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetMemoryLowPower___STATIC__VOID__I4__BOOLEAN,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_BaseRmtChannel::NativeSetSourceClock___STATIC__VOID__I4__I4,
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
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeInitializeTransmitter___STATIC__I4__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetIdleLevel___STATIC__I4__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeGetIsChannelIdle___STATIC__BOOLEAN__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetIsChannelIdle___STATIC__VOID__I4__BOOLEAN,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetIdleLevel___STATIC__VOID__I4__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeSetCarrierMode___STATIC__VOID__I4__BOOLEAN__U2__U2__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeTxFillItems___STATIC__U4__I4__SZARRAY_nanoFrameworkHardwareEsp32RmtRmtItem__U2,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeTxStart___STATIC__U4__I4__BOOLEAN,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeTxStop___STATIC__U4__I4,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeWriteItems___STATIC__U4__I4__SZARRAY_nanoFrameworkHardwareEsp32RmtRmtItem__BOOLEAN,
    Library_nanoFramework_hardware_esp32_rmt_native_nanoFramework_Hardware_Esp32_Rmt_TransmitterChannel::NativeWaitTxDone___STATIC__U4__I4__I4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Hardware_Esp32_Rmt =
{
    "nanoFramework.Hardware.Esp32.Rmt", 
    0x3C07E8B6,
    method_lookup,
    ////////////////////////////////////////////////////////////////////////////////////
    // check if the version bellow matches the one in AssemblyNativeVersion attribute //
    ////////////////////////////////////////////////////////////////////////////////////
    { 100, 0, 1, 0 }
};
