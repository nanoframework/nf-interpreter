//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include <ch.h>
#include <hal.h>
#include <cmsis_os.h>

#include <targetPAL.h>
#include "win_dev_gpio_native.h"

///////////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinDriveMode (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////////

enum GpioPinDriveMode
{
    GpioPinDriveMode_Input = 0,
    GpioPinDriveMode_InputPullDown,
    GpioPinDriveMode_InputPullUp,
    GpioPinDriveMode_Output,
    GpioPinDriveMode_OutputOpenDrain,
    GpioPinDriveMode_OutputOpenDrainPullUp,
    GpioPinDriveMode_OutputOpenSource,
    GpioPinDriveMode_OutputOpenSourcePullDown,
};

///////////////////////////////////////////////////////////////////////////////////
// !!! KEEP IN SYNC WITH Windows.Devices.Gpio.GpioPinValue (in managed code) !!! //
///////////////////////////////////////////////////////////////////////////////////

enum GpioPinValue
{
    GpioPinValue_Low = 0,
    GpioPinValue_High,
};

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


void IsrProcedure( GPIO_PIN pin, bool pinState, void* context )
{
    // TODO check if we need this here 
    // ASSERT_IRQ_MUST_BE_OFF();
    PostManagedEvent( EVENT_GPIO, 0, pin, pinState );
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::get_DebounceTimeout___mscorlibSystemTimeSpan( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::set_DebounceTimeout___VOID__mscorlibSystemTimeSpan( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();

    NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());

    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Read___WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        signed int pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

        stack.SetResult_I4( palReadPad(GPIO_PORT(pinNumber), pinNumber % 16) );
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Write___VOID__WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        signed int pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        signed int driveMode = pThis[ FIELD___driveMode ].NumericByRefConst().s4;

        signed int state = stack.Arg1().NumericByRef().s4;

        // sanity check for drive mode to output so we don't mess up with writing to an input pin
        // TODO should consider the other output variantions too???
        if (driveMode == GpioPinDriveMode_Output)
        {
            if (state == 0)
            {
                palClearPad(GPIO_PORT(pinNumber), pinNumber % 16);
            }
            else 
            {
                palSetPad(GPIO_PORT(pinNumber), pinNumber % 16);
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        // set pin to input to save power
        // clear interrupts
        // releases the pin

    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

        signed int driveMode = stack.Arg1().NumericByRef().s4;
        
        // Return value to the managed application
        stack.SetResult_Boolean( driveMode < 5 ) ;
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDriveMode___VOID__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        signed int pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
        signed int driveMode = stack.Arg1().NumericByRef().s4;

        switch (driveMode)
        {
            case 0 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT);
                        break;
            case 1 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLDOWN);
                        break;
            case 2 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLUP);
                        break;
            case 3 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_OUTPUT_PUSHPULL);
                        break;
            case 4 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_OUTPUT_OPENDRAIN);
                        break;
            default :   palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLDOWN);
                        break;
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeInit___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

        signed int pinNumber = stack.Arg1().NumericByRef().s4;

        // TODO is probably a good idea keep track of the used pins, so we can check that here
        // TODO is probably a good idea to check if this pin exists

        // TODO initialize the pin INT

        // Return value to the managed application
        stack.SetResult_Boolean(true );
    }
    NANOCLR_NOCLEANUP();
}
