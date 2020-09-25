//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//


#include "stdafx.h"
#include "win_dev_gpio_native.h"

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

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

		//stack.SetResult_I4(palReadPad(GPIO_PORT(pinNumber), pinNumber % 16));
		stack.SetResult_I4(1);
	}
    NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Toggle___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();
	{
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::DisposeNative___VOID( CLR_RT_StackFrame& stack )
{
    NANOCLR_HEADER();
    {
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

        // switch (driveMode)
        // {
        //     case 0 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT);
        //                 break;
        //     case 1 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLDOWN);
        //                 break;
        //     case 2 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLUP);
        //                 break;
        //     case 3 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_OUTPUT_PUSHPULL);
        //                 break;
        //     case 4 :    palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_OUTPUT_OPENDRAIN);
        //                 break;
        //     default :   palSetPadMode(GPIO_PORT(pinNumber), pinNumber % 16, PAL_MODE_INPUT_PULLDOWN);
        //                 break;
        // }
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

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDebounceTimeout___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	// nothing to do here

	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::WriteNative___VOID__WindowsDevicesGpioGpioPinValue(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		if (pThis[Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		signed int pinNumber = pThis[FIELD___pinNumber].NumericByRefConst().s4;
		GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[FIELD___driveMode].NumericByRefConst().s4;

		GpioPinValue state = (GpioPinValue)stack.Arg1().NumericByRef().s4;


		// sanity check for drive mode set to output so we don't mess up writing to an input pin
		if ((driveMode == GpioPinDriveMode_Output) ||
			(driveMode == GpioPinDriveMode_OutputOpenDrain) ||
			(driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
			(driveMode == GpioPinDriveMode_OutputOpenSource) ||
			(driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
		{
			if (state == GpioPinValue_Low)
			{
				//palClearPad(GPIO_PORT(pinNumber), pinNumber % 16);
			}
			else
			{
				//palSetPad(GPIO_PORT(pinNumber), pinNumber % 16);
			}
		}
		else
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
		}

	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetAlternateFunction___VOID__I4(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();

	NANOCLR_NOCLEANUP();
}