//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_spi_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    nullptr,
    nullptr,
    nullptr,
    Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo::NativeMaxClockFrequency___I4,
    Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo::NativeMinClockFrequency___I4,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::DisposeNative___VOID,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeTransfer___VOID__SystemReadOnlySpan_1__SystemSpan_1__BOOLEAN,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeInit___VOID,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeOpenDevice___I4,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Spi =
{
    "System.Device.Spi",
    0x686BF72E,
    method_lookup,
    { 100, 2, 0, 0 }
};

// clang-format on
