//
// Copyright (c) 2017 The nanoFramework project contributors
// See LICENSE file in the project root for full license information.
//


#ifndef _WINDOWS_DEVICES_GPIO_NATIVE_H_
#define _WINDOWS_DEVICES_GPIO_NATIVE_H_

#include <nanoCLR_Interop.h>
struct Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioChangeCount
{
    static const int FIELD__Count = 1;
    static const int FIELD__RelativeTime = 2;


    //--//

};

struct Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioChangeCounter
{
    static const int FIELD___Polarity = 1;


    //--//

};

struct Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioChangeReader
{
    static const int FIELD___Capacity = 1;
    static const int FIELD___IsOverflowed = 2;
    static const int FIELD___IsStarted = 3;
    static const int FIELD___Length = 4;
    static const int FIELD___Polarity = 5;


    //--//

};

struct Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioChangeRecord
{
    static const int FIELD__Edge = 1;
    static const int FIELD__RelativeTime = 2;


    //--//

};

struct Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioController
{
    static const int FIELD___PinCount = 1;


    //--//

};

struct Library_windows_devices_gpio_native_Windows_Devices_Gpio_GpioPin
{
    static const int FIELD___pinNumber = 1;
    static const int FIELD__disposedValue = 2;

    NANOCLR_NATIVE_DECLARE(get_DebounceTimeout___mscorlibSystemTimeSpan);
    NANOCLR_NATIVE_DECLARE(set_DebounceTimeout___VOID__mscorlibSystemTimeSpan);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);

    //--//

};



extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_Windows_Devices_Gpio;

#endif  //_WINDOWS_DEVICES_GPIO_NATIVE_H_
