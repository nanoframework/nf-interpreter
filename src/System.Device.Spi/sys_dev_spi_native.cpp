//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_spi_native.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo::NativeMaxClockFrequency___I4,
    Library_sys_dev_spi_native_System_Device_Spi_SpiBusInfo::NativeMinClockFrequency___I4,
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
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::DisposeNative___VOID,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeTransfer___VOID__SystemSpanByte__SystemSpanByte__BOOLEAN,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeInit___VOID,
    Library_sys_dev_spi_native_System_Device_Spi_SpiDevice::NativeOpenDevice___I4,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Spi =
{
    "System.Device.Spi",
    0x3F6E2A7E,
    method_lookup,
    { 100, 1, 1, 0 }
};

// clang-format on
