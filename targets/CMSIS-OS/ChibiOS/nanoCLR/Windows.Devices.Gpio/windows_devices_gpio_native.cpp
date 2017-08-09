//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "windows_devices_gpio_native.h"


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
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioController::NativeOpenpin___BOOLEAN__I4,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioController::get_PinCount___I4,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::get_DebounceTimeout___mscorlibSystemTimeSpan,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::set_DebounceTimeout___VOID__mscorlibSystemTimeSpan,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::Read___WindowsDevicesGpioGpioPinValue,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::Write___VOID__WindowsDevicesGpioGpioPinValue,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::DisposeNative___VOID,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDriveMode___VOID__WindowsDevicesGpioGpioPinDriveMode,
    Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin::NativeInit___BOOLEAN__I4,
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
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Gpio =
{
    "Windows.Devices.Gpio", 
    0xE793D87B,
    method_lookup
};
