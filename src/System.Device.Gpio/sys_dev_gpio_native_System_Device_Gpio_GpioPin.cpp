//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <targetPAL.h>
#include <corlib_native.h>
#include "sys_dev_gpio_native_target.h"
#include "nf_rt_events_native.h"
#include "sys_dev_gpio_native.h"

// just make it shorter and readable
typedef Library_sys_dev_gpio_native_System_Device_Gpio_PinValue PinValue;

// declared here as external
// the implementation will be moved here when Windows.Devices.Gpio is removed
extern void Gpio_Interupt_ISR(GPIO_PIN pinNumber, bool pinState, void *pArg);

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::Read___SystemDeviceGpioPinValue(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_TypeDef_Index pinValueTypeDef;
    CLR_RT_HeapBlock *hbObj;
    bool pinValue;

    CLR_RT_HeapBlock &top = stack.PushValue();

    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    NANOCLR_CHECK_HRESULT(Read(pThis, pinValue));

    // find <GpioPin> type definition, don't bother checking the result as it exists for sure
    g_CLR_RT_TypeSystem.FindTypeDef("PinValue", "System.Device.Gpio", pinValueTypeDef);

    // create an instance of <GpioPin>
    NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, pinValueTypeDef));

    // dereference the object in order to reach its fields
    hbObj = top.Dereference();

    hbObj[Library_sys_dev_gpio_native_System_Device_Gpio_PinValue::FIELD___value].NumericByRef().u1 = pinValue;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::Toggle___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        GPIO_PIN pinNumber = (GPIO_PIN)pThis[FIELD___pinNumber].NumericByRefConst().s4;
        GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[FIELD___pinMode].NumericByRefConst().s4;

        // sanity check for drive mode set to output so we don't mess up writing to an input pin
        if (driveMode >= GpioPinDriveMode_Output)
        {
            CPU_GPIO_TogglePinState(pinNumber);

            // fire event, only if there are callbacks registered
            if (pThis[FIELD___callbacks].Dereference() != NULL)
            {
                PostManagedEvent(EVENT_GPIO, 0, (uint16_t)pinNumber, (uint32_t)CPU_GPIO_GetPinState(pinNumber));
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::DisposeNative___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // set pin to input to save power
        // clear interrupts
        // releases the pin
        GPIO_PIN pinNumber = (GPIO_PIN)pThis[FIELD___pinNumber].NumericByRefConst().s4;

        CPU_GPIO_DisablePin(pinNumber, GpioPinDriveMode_Input, 0);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::
    NativeIsPinModeSupported___BOOLEAN__SystemDeviceGpioPinMode(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        GPIO_PIN pinNumber = (GPIO_PIN)pThis[FIELD___pinNumber].NumericByRefConst().s4;

        GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

        // Return value to the managed application
        stack.SetResult_Boolean(CPU_GPIO_DriveModeSupported(pinNumber, driveMode));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeSetPinMode___VOID__SystemDeviceGpioPinMode(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

        NANOCLR_CHECK_HRESULT(SetPinMode(pThis, driveMode));

        // protect this from GC so that the callback is where it's supposed to
        CLR_RT_ProtectFromGC gc(*pThis);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeSetDebounceTimeout___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_UINT64 debounceTimeoutMilsec;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        GPIO_PIN pinNumber = (GPIO_PIN)pThis[FIELD___pinNumber].NumericByRefConst().s4;

        NANOCLR_CHECK_HRESULT(ExtractDebounceTimeSpanValue(pThis[FIELD___debounceTimeout], debounceTimeoutMilsec));

        // developer note:
        // the following call will FAIL if the pin hasn't been previously setup as input
        // that's OK because the debounce timeout will be eventually set when the pin is configured
        CPU_GPIO_SetPinDebounce(pinNumber, (uint32_t)debounceTimeoutMilsec);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::WriteNative___VOID__SystemDeviceGpioPinValue(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    GpioPinValue state;

    CLR_RT_HeapBlock *pinValue;
    CLR_RT_HeapBlock *pThis = stack.This();
    FAULT_ON_NULL(pThis);

    // check if object has been disposed
    if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
    }

    // get PinValue object from parameter
    pinValue = stack.Arg1().Dereference();

    // access value field inside PinValue
    state = (GpioPinValue)pinValue[PinValue::FIELD___value].NumericByRef().u1;

    NANOCLR_CHECK_HRESULT(Write(pThis, state));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeSetAlternateFunction___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        // get pin number and take the port and pad references from that one
        int16_t pinNumber = pThis[FIELD___pinNumber].NumericByRefConst().s4;

        // get alternate function argument
        int32_t alternateFunction = stack.Arg1().NumericByRef().s4;

        CPU_GPIO_DisablePin(pinNumber, GpioPinDriveMode_Input, alternateFunction);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::NativeInit___BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        GPIO_PIN pinNumber = (GPIO_PIN)stack.Arg1().NumericByRef().s4;

        // Return value to the managed application
        stack.SetResult_Boolean(CPU_GPIO_ReservePin(pinNumber, true));
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::ExtractDebounceTimeSpanValue(
    CLR_RT_HeapBlock &timeSpanValue,
    CLR_UINT64 &value)
{
    NANOCLR_HEADER();
    {
        // debounceTimeout field its a TimeSpan, which is a primitive type stored as an heap block, therefore needs to
        // be accessed indirectly
        CLR_INT64 *debounceValue = Library_corlib_native_System_TimeSpan::GetValuePtr(timeSpanValue);
        FAULT_ON_NULL(debounceValue);

        value = (CLR_UINT64)(*debounceValue / TIME_CONVERSION__TO_MILLISECONDS);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::SetPinMode(
    CLR_RT_HeapBlock *gpioPin,
    GpioPinDriveMode pinMode)
{
    NANOCLR_HEADER();

    bool validPin;
    CLR_UINT64 debounceTimeoutMilsec;
    bool callbacksRegistered = false;

    GPIO_PIN pinNumber = (GPIO_PIN)gpioPin[FIELD___pinNumber].NumericByRefConst().s4;

    if (pinMode >= (int)GpioPinDriveMode_Output)
    {
        validPin = CPU_GPIO_EnableOutputPin(pinNumber, GpioPinValue_Low, pinMode);
    }
    else
    {
        NANOCLR_CHECK_HRESULT(ExtractDebounceTimeSpanValue(gpioPin[FIELD___debounceTimeout], debounceTimeoutMilsec));

        // flag to determine if there are any callbacks registered in managed code
        // this is use to determine if there is any need to setup and process INT handler
        callbacksRegistered = (gpioPin[FIELD___callbacks].Dereference() != NULL);

        validPin = CPU_GPIO_EnableInputPin(
            pinNumber,
            (uint32_t)debounceTimeoutMilsec,
            callbacksRegistered ? Gpio_Interupt_ISR : NULL,
            NULL,
            GPIO_INT_EDGE_BOTH,
            pinMode);
    }

    if (validPin)
    {
        // all good, store pin mode in managed field
        gpioPin[FIELD___pinMode].NumericByRef().s4 = pinMode;
    }
    else
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::Write(CLR_RT_HeapBlock *gpioPin, GpioPinValue pinValue)
{
    GPIO_PIN pinNumber = (GPIO_PIN)gpioPin[FIELD___pinNumber].NumericByRefConst().s4;
    GpioPinDriveMode driveMode = (GpioPinDriveMode)gpioPin[FIELD___pinMode].NumericByRefConst().s4;

    // sanity check for drive mode set to output so we don't mess up writing to an input pin
    if ((driveMode >= GpioPinDriveMode_Output))
    {
        CPU_GPIO_SetPinState(pinNumber, pinValue);

        // fire event if there are callbacks registered
        if (gpioPin[FIELD___callbacks].Dereference() != NULL)
        {
            PostManagedEvent(EVENT_GPIO, 0, (uint16_t)pinNumber, (uint32_t)pinValue);
        }
    }
    else
    {
        return CLR_E_INVALID_PARAMETER;
    }

    return S_OK;
}

HRESULT Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::Read(CLR_RT_HeapBlock *gpioPin, bool &pinValue)
{
    GPIO_PIN pinNumber = (GPIO_PIN)gpioPin[FIELD___pinNumber].NumericByRefConst().s4;

    pinValue = CPU_GPIO_GetPinState(pinNumber);

    return S_OK;
}
