﻿//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


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
    NULL,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeChipSelectLineCount___I4__I4,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeMaxClockFrequency___I4__I4,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiBusInfo::NativeMinClockFrequency___I4__I4,
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
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiController::GetDeviceSelector___STATIC__STRING,
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
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::DisposeNative___VOID,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U1__SZARRAY_U1__BOOLEAN,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeTransfer___VOID__SZARRAY_U2__SZARRAY_U2__BOOLEAN,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeInit___VOID,
    Library_win_dev_spi_native_Windows_Devices_Spi_SpiDevice::NativeOpenDevice___I4__I4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Spi =
{
    "Windows.Devices.Spi", 
    0x1E6398C8,
    method_lookup,
    { 100, 1, 4, 0 }
};
