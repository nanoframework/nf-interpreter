//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_devices_onewire_native.h"

static const CLR_RT_MethodHandler method_lookup[] =
{
    Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::TouchReset___BOOLEAN,
    Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::TouchBit___BOOLEAN__BOOLEAN,
    Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::TouchByte___U1__U1,
    Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::WriteByte___U1__U1,
    Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::ReadByte___U1,
    Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::FindFirstDevice___BOOLEAN__BOOLEAN__BOOLEAN,
    Library_nf_devices_onewire_native_nanoFramework_Devices_OneWire_OneWireController::FindNextDevice___BOOLEAN__BOOLEAN__BOOLEAN,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Devices_OneWire =
{
    "nanoFramework.Devices.OneWire",
    0xA5C172BD,
    method_lookup,
    { 100, 0, 3, 4 }
};
