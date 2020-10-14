//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "sys_dev_gpio_native.h"

typedef Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin GpioPin;
typedef Library_sys_dev_gpio_native_System_Device_Gpio_GpioPinBundle GpioPinBundle;

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::get_PinCount___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // return value to the managed application
    stack.SetResult_I4(CPU_GPIO_GetPinCount());

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    (void)stack;

    NANOCLR_HEADER();

    CPU_GPIO_Uninitialize();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::
    IsPinModeSupported___BOOLEAN__I4__SystemDeviceGpioPinMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    GpioPinDriveMode driveMode;

    GPIO_PIN pinNumber = (GPIO_PIN)stack.Arg1().NumericByRef().s4;

    driveMode = (GpioPinDriveMode)stack.Arg2().NumericByRef().s4;

    // Return value to the managed call
    stack.SetResult_Boolean(CPU_GPIO_DriveModeSupported(pinNumber, driveMode));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::IsPinOpen___BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    GPIO_PIN pinNumber = (GPIO_PIN)stack.Arg1().NumericByRef().s4;

    // Return value to the managed call
    stack.SetResult_Boolean(CPU_GPIO_PinIsBusy(pinNumber));

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::SetPinMode___VOID__I4__SystemDeviceGpioPinMode(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    GPIO_PIN pinNumber;
    GpioPinDriveMode driveMode;
    bool validPin;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    pinNumber = (GPIO_PIN)stack.Arg1().NumericByRef().s4;
    driveMode = (GpioPinDriveMode)stack.Arg2().NumericByRef().s4;

    if (driveMode >= (int)GpioPinDriveMode_Output)
    {
        validPin = CPU_GPIO_EnableOutputPin(pinNumber, GpioPinValue_Low, (GpioPinDriveMode)driveMode);
    }
    else
    {
        validPin = CPU_GPIO_EnableInputPin(pinNumber, -1, NULL, NULL, GPIO_INT_EDGE_BOTH, (GpioPinDriveMode)driveMode);
    }

    if (!validPin)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::NativeRead___U1__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    GPIO_PIN pinNumber;
    CLR_RT_HeapBlock *gpioPin = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    pinNumber = (GPIO_PIN)stack.Arg1().NumericByRef().s4;

    // try to get GpioPin object
    GetGpioPin(pinNumber, stack, gpioPin);

    if (gpioPin == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // read pin and set result
    stack.SetResult_U1(CPU_GPIO_GetPinState(pinNumber));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::NativeWrite___VOID__I4__U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    GPIO_PIN pinNumber;
    GpioPinValue state;
    CLR_RT_HeapBlock *gpioPin = NULL;

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    pinNumber = (GPIO_PIN)stack.Arg1().NumericByRef().s4;
    state = (GpioPinValue)stack.Arg2().NumericByRef().u1;

    // try to get GpioPin object
    GetGpioPin(pinNumber, stack, gpioPin);

    if (gpioPin == NULL)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_OPERATION);
    }

    // write to pin
    CPU_GPIO_SetPinState(pinNumber, state);

    // store the output value in the field
    gpioPin[GpioPin::FIELD___lastOutputValue].NumericByRef().s4 = state;

    NANOCLR_NOCLEANUP();
}

void Library_sys_dev_gpio_native_System_Device_Gpio_GpioController::GetGpioPin(
    CLR_INT32 pinNumber,
    CLR_RT_StackFrame &stack,
    CLR_RT_HeapBlock *&gpioPin)
{
    CLR_IDX assemblyIdx;
    CLR_INT32 index = 0;
    CLR_RT_Assembly *thisAssembly = NULL;
    CLR_RT_HeapBlock *gpioPinBundle = NULL;
    CLR_RT_HeapBlock_ArrayList *gpioPins = NULL;

    // get "This" because we need to access the s_GpioPins field
    CLR_RT_HeapBlock *pThis = stack.This();

    // s_GpioPins it's a static field, need to access it through the assembly
    assemblyIdx = pThis->ObjectCls().Assembly();
    thisAssembly = g_CLR_RT_TypeSystem.m_assemblies[assemblyIdx - 1];

    // finally dereference s_GpioPins (as an ArrayList)
    gpioPins = (CLR_RT_HeapBlock_ArrayList *)thisAssembly->GetStaticField(FIELD_STATIC__s_GpioPins)->Dereference();

    do
    {
        // try to get item from the array list
        gpioPins->GetItem(index++, gpioPinBundle);

        if (gpioPinBundle == NULL)
        {
            // no more items in the array
            break;
        }

        // check if the pin number matches the one we are looking for
        if (gpioPinBundle[GpioPinBundle::FIELD__PinNumber].NumericByRef().s4 == pinNumber)
        {
            // matches, get the GpioPin
            gpioPin = gpioPinBundle[GpioPinBundle::FIELD__GpioPin].Dereference();

            // done here
            break;
        }

    } while (true);
}
