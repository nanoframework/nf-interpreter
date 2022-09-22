//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_dev_onewire.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchReset___BOOLEAN,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchBit___BOOLEAN__BOOLEAN,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::TouchByte___U1__U1,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::WriteByte___U1__U1,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::ReadByte___U1,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::FindFirstDevice___BOOLEAN__BOOLEAN__BOOLEAN,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::FindNextDevice___BOOLEAN__BOOLEAN__BOOLEAN,
    NULL,
    NULL,
    NULL,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::NativeDispose___VOID,
    Library_nf_dev_onewire_nanoFramework_Device_OneWire_OneWireHost::NativeInit___VOID,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Device_OneWire =
{
    "nanoFramework.Device.OneWire",
    0xB95C43B4,
    method_lookup,
    { 100, 0, 4, 0 }
};

// clang-format on
