//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _WIN_DEV_GPIO_NATIVE_H_
#define _WIN_DEV_GPIO_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoCLR_Checks.h>
#include <nf_rt_events_native.h>

///////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioChangePolarity (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////

enum GpioChangePolarity
{
	Both = 0,
	Falling, 
	Rising
};


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

struct Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCount
{
    static const int FIELD__Count = 1;
    static const int FIELD__RelativeTime = 2;

    //--//

};

struct Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter
{
    static const int FIELD___pinNumber = 1;
    static const int FIELD___inputMode = 2;
    static const int FIELD___polarity = 3;
    static const int FIELD___countActive = 4;
    static const int FIELD___readTime = 5;
    static const int FIELD___syncLock = 6;
    static const int FIELD___disposedValue = 7;

    NANOCLR_NATIVE_DECLARE(NativeInit___VOID);
    NANOCLR_NATIVE_DECLARE(NativeRead___U8__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(NativeStart___VOID);
    NANOCLR_NATIVE_DECLARE(NativeStop___VOID);
    NANOCLR_NATIVE_DECLARE(NativeDispose___VOID);

    //--//

};

struct Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPinValueChangedEventArgs
{
    static const int FIELD___edge = 1;

    //--//

};

struct Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin
{
    static const int FIELD_STATIC__s_eventListener = 0;

    static const int FIELD___syncLock = 1;
    static const int FIELD___pinNumber = 2;
    static const int FIELD___driveMode = 3;
    static const int FIELD___debounceTimeout = 4;
    static const int FIELD___callbacks = 5;
    static const int FIELD___lastOutputValue = 6;
    static const int FIELD___lastInputValue = 7;
    static const int FIELD___disposedValue = 8;

    NANOCLR_NATIVE_DECLARE(Read___WindowsDevicesGpioGpioPinValue);
    NANOCLR_NATIVE_DECLARE(Toggle___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode);
    NANOCLR_NATIVE_DECLARE(NativeSetDriveMode___VOID__WindowsDevicesGpioGpioPinDriveMode);
    NANOCLR_NATIVE_DECLARE(NativeInit___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(NativeSetDebounceTimeout___VOID);
    NANOCLR_NATIVE_DECLARE(WriteNative___VOID__WindowsDevicesGpioGpioPinValue);
    NANOCLR_NATIVE_DECLARE(NativeSetAlternateFunction___VOID__I4);

    //--//

    static HRESULT ExtractDebounceTimeSpanValue( CLR_RT_HeapBlock& timeSpanValue, CLR_UINT64& value );
};

struct Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioController
{
    static const int FIELD_STATIC___syncLock = 1;
    static const int FIELD_STATIC__s_instance = 2;

    NANOCLR_NATIVE_DECLARE(get_PinCount___I4);

    //--//

};

struct Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPinEvent
{
    static const int FIELD__PinNumber = 1;
    static const int FIELD__Edge = 2;

    //--//

};

struct Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPinEventListener
{
    static const int FIELD___pinMap = 1;

    //--//

};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Gpio;

#endif  //_WIN_DEV_GPIO_NATIVE_TARGET_H_
