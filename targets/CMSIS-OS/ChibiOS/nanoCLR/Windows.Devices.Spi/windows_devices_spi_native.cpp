//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "windows_devices_spi_native.h"


static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_windows_devices_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MaxClockFrequency___I4,
    Library_windows_devices_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MinClockFrequency___I4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__STRING__SZARRAY_U1__SZARRAY_U1__BOOLEAN,
    Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__STRING__SZARRAY_U2__SZARRAY_U2__BOOLEAN,
    Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID__STRING__I4__I4__I4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_windows_devices_spi_native_Windows_Devices_Spi_SpiDevice::GetDeviceSelector___STATIC__STRING,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi =
{
    "Windows.Devices.Spi", 
    0x4BBBA80B,
    method_lookup
};
