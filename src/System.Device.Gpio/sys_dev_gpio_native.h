//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#ifndef _SYS_DEV_GPIO_NATIVE_H_
#define _SYS_DEV_GPIO_NATIVE_H_

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack PinEventTypes
{
    PinEventTypes_None = 0,
    PinEventTypes_Rising = 1,
    PinEventTypes_Falling = 2,
} PinEventTypes;

// moved to src\PAL\Include\CPU_GPIO_decl.h for convenience
// typedef enum __nfpack PinMode
// {
//     PinMode_Input = 0,
//     PinMode_InputPullDown = 1,
//     PinMode_InputPullUp = 2,
//     PinMode_Output = 3,
//     PinMode_OutputOpenDrain = 4,
//     PinMode_OutputOpenDrainPullUp = 5,
//     PinMode_OutputOpenSource = 6,
//     PinMode_OutputOpenSourcePullDown = 7,
// } PinMode;

struct Library_sys_dev_gpio_native_System_Device_Gpio_PinValue
{
    static const int FIELD___value = 1;

    //--//
};

struct Library_sys_dev_gpio_native_System_Device_Gpio_GpioController
{
    static const int FIELD_STATIC__s_GpioPins = 0;
    static const int FIELD_STATIC___syncLock = 1;

    static const int FIELD___disposedValue = 1;
    static const int FIELD___NumberingScheme_k__BackingField = 2;

    NANOCLR_NATIVE_DECLARE(get_PinCount___I4);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(IsPinModeSupported___BOOLEAN__I4__SystemDeviceGpioPinMode);
    NANOCLR_NATIVE_DECLARE(IsPinOpen___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(SetPinMode___VOID__I4__SystemDeviceGpioPinMode);
    NANOCLR_NATIVE_DECLARE(NativeRead___U1__I4);
    NANOCLR_NATIVE_DECLARE(NativeWrite___VOID__I4__U1);

    //--//

    static void GetGpioPin(CLR_INT32 index, CLR_RT_StackFrame &stack, CLR_RT_HeapBlock *&gpioPin);
};

struct Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin
{
    static const int FIELD_STATIC__s_gpioPinEventManager = 2;

    static const int FIELD___syncLock = 1;
    static const int FIELD___pinNumber = 2;
    static const int FIELD___driveMode = 3;
    static const int FIELD___debounceTimeout = 4;
    static const int FIELD___callbacks = 5;
    static const int FIELD___lastOutputValue = 6;
    static const int FIELD___lastInputValue = 7;
    static const int FIELD___disposedValue = 8;

    NANOCLR_NATIVE_DECLARE(Read___SystemDeviceGpioPinValue);
    NANOCLR_NATIVE_DECLARE(Toggle___VOID);
    NANOCLR_NATIVE_DECLARE(DisposeNative___VOID);
    NANOCLR_NATIVE_DECLARE(NativeIsDriveModeSupported___BOOLEAN__SystemDeviceGpioPinMode);
    NANOCLR_NATIVE_DECLARE(NativeSetDriveMode___VOID__SystemDeviceGpioPinMode);
    NANOCLR_NATIVE_DECLARE(NativeSetDebounceTimeout___VOID);
    NANOCLR_NATIVE_DECLARE(WriteNative___VOID__SystemDeviceGpioPinValue);
    NANOCLR_NATIVE_DECLARE(NativeSetAlternateFunction___VOID__I4);
    NANOCLR_NATIVE_DECLARE(NativeInit___BOOLEAN__I4);

    //--//

    static HRESULT ExtractDebounceTimeSpanValue(CLR_RT_HeapBlock &timeSpanValue, CLR_UINT64 &value);
};

struct Library_sys_dev_gpio_native_System_Device_Gpio_GpioPinBundle
{
    static const int FIELD__PinNumber = 1;
    static const int FIELD__GpioPin = 2;
    static const int FIELD__GpioEvents = 3;
    static const int FIELD__GpioPinChange = 4;
    static const int FIELD__GpioEventsHappening = 5;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_System_Device_Gpio;

#endif //_SYS_DEV_GPIO_NATIVE_H_
