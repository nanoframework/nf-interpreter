//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include <targetPAL.h>
#include <corlib_native.h>
#include "win_dev_gpio_native_target.h"
#include "nf_rt_events_native.h"

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void Gpio_Interupt_ISR(GPIO_PIN pinNumber, bool pinState, void* param )
{
	NATIVE_INTERRUPT_START

	CLR_RT_HeapBlock *pThis = (CLR_RT_HeapBlock *)param;
	if (pThis != NULL)
	{
		// check if object has been disposed
		if (pThis[Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue].NumericByRef().u1 == 0)
		{
			// flag to determine if there are any callbacks registered in managed code
			bool callbacksRegistered = (pThis[Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___callbacks].Dereference() != NULL);
			if (callbacksRegistered)
			{
				// if handle registered then post a managed event with the current pin reading
				PostManagedEvent(EVENT_GPIO, 0, (uint16_t)pinNumber, (uint32_t)pinState);
			}
		}
	}

	NATIVE_INTERRUPT_END
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

		GPIO_PIN pinNumber = (GPIO_PIN)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

		stack.SetResult_I4( CPU_GPIO_GetPinState(pinNumber) );
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::Toggle___VOID( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);
		
		// check if object has been disposed
		if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		GPIO_PIN pinNumber = (GPIO_PIN)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
		GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[ FIELD___driveMode ].NumericByRefConst().s4;
		
		// sanity check for drive mode set to output so we don't mess up writing to an input pin
		if (driveMode >=  GpioPinDriveMode_Output) 
		{
			// Not all lower level API offer a 'toggle', so need to rely on the last output value field and toggle that one
			GpioPinValue newState = (GpioPinValue)(GpioPinValue_High ^ (GpioPinValue)pThis[ FIELD___lastOutputValue ].NumericByRef().s4);
			
			// ...write back to the GPIO...
			CPU_GPIO_SetPinState(pinNumber, newState );

			// ... and finally store it
			pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = newState;
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
		GPIO_PIN pinNumber = (GPIO_PIN)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
		
		CPU_GPIO_DisablePin(pinNumber, GpioPinDriveMode_Input, 0) ;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeIsDriveModeSupported___BOOLEAN__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock* pThis = stack.This();  FAULT_ON_NULL(pThis);

		GPIO_PIN pinNumber = (GPIO_PIN)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

		GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

		// Return value to the managed application
		stack.SetResult_Boolean(CPU_GPIO_DriveModeSupported(pinNumber, driveMode)) ;
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDriveMode___VOID__WindowsDevicesGpioGpioPinDriveMode( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		bool validPin;
		CLR_UINT64 debounceTimeoutMilsec;

		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		GPIO_PIN pinNumber = (GPIO_PIN)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
		GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

		if (driveMode >= (int)GpioPinDriveMode_Output)
		{
			validPin = CPU_GPIO_EnableOutputPin(pinNumber, GpioPinValue_Low, driveMode);
		}
		else
		{
			NANOCLR_CHECK_HRESULT( ExtractDebounceTimeSpanValue(pThis[ FIELD___debounceTimeout ], debounceTimeoutMilsec ) );

			validPin = CPU_GPIO_EnableInputPin(pinNumber, debounceTimeoutMilsec, Gpio_Interupt_ISR, (void*)pThis, GPIO_INT_EDGE_BOTH, driveMode);
		}

		if (!validPin)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
		}

		// protect this from GC so that the callback is where it's supposed to
		CLR_RT_ProtectFromGC         gc( *pThis );
					
				   
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeInit___BOOLEAN__I4( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		GPIO_PIN pinNumber = (GPIO_PIN)stack.Arg1().NumericByRef().s4;

		// Return value to the managed application
		stack.SetResult_Boolean(CPU_GPIO_ReservePin(pinNumber, true));
	}
	NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetDebounceTimeout___VOID( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		CLR_UINT64 debounceTimeoutMilsec;

		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		GPIO_PIN pinNumber = (GPIO_PIN)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

		NANOCLR_CHECK_HRESULT( ExtractDebounceTimeSpanValue(pThis[ FIELD___debounceTimeout ], debounceTimeoutMilsec ) );

		// developer note:
		// the following call will FAIL if the pin hasn't been previously setup as input 
		// that's OK because the debounce timeout will be eventually set when the pin is configured
		CPU_GPIO_SetPinDebounce( pinNumber, debounceTimeoutMilsec );
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::WriteNative___VOID__WindowsDevicesGpioGpioPinValue( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);
		
		// check if object has been disposed
		if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		GPIO_PIN pinNumber = (GPIO_PIN)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
		GpioPinDriveMode driveMode = (GpioPinDriveMode)pThis[ FIELD___driveMode ].NumericByRefConst().s4;

		GpioPinValue state = (GpioPinValue)stack.Arg1().NumericByRef().s4;
		
		// sanity check for drive mode set to output so we don't mess up writing to an input pin
		if ((driveMode >= GpioPinDriveMode_Output) )
		{
			CPU_GPIO_SetPinState(pinNumber, state );

			// store the output value in the field
			pThis[ FIELD___lastOutputValue ].NumericByRef().s4 = state;
		}
		else
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
		}
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::NativeSetAlternateFunction___VOID__I4( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		// check if object has been disposed
		if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		// get pin number and take the port and pad references from that one
		int16_t pinNumber = pThis[ FIELD___pinNumber ].NumericByRefConst().s4;

		// get alternate function argument
		int32_t alternateFunction = stack.Arg1().NumericByRef().s4;

		CPU_GPIO_DisablePin( pinNumber, GpioPinDriveMode_Input, alternateFunction);
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::ExtractDebounceTimeSpanValue( CLR_RT_HeapBlock& timeSpanValue, CLR_UINT64& value )
{
    NANOCLR_HEADER();
	{
		// debounceTimeout field its a TimeSpan, which is a primitive type stored as an heap block, therefore needs to be accessed indirectly
		CLR_INT64* debounceValue = Library_corlib_native_System_TimeSpan::GetValuePtr( timeSpanValue ); FAULT_ON_NULL(debounceValue);

		value = *(CLR_UINT64*)debounceValue / TIME_CONVERSION__TO_MILLISECONDS;
	}
	NANOCLR_NOCLEANUP();
}
