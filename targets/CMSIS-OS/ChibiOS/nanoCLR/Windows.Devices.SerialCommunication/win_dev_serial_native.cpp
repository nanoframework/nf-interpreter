//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#include "win_dev_serial_native.h"


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
    Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeDispose___VOID,
    Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeInit___VOID,
    Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::NativeWrite___VOID__SZARRAY_U1,
    NULL,
    Library_win_dev_serial_native_Windows_Devices_SerialCommunication_SerialDevice::GetDeviceSelector___STATIC__STRING,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_SerialCommunication =
{
    "Windows.Devices.SerialCommunication", 
    0x08A389F7,
    method_lookup
};
