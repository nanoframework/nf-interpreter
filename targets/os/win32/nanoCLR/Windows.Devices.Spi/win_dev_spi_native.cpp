//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "stdafx.h"
#include "win_dev_spi_native.h"


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
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MaxClockFrequency___I4,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::get_MinClockFrequency___I4,
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
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U1__SZARRAY_U1__BOOLEAN,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID,
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
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID,
    NULL,
    NULL,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::GetDeviceSelector___STATIC__STRING,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi =
{
    "Windows.Devices.Spi", 
    0x083DBB79,
    method_lookup,
    { 1, 1, 3, 0 }
};
