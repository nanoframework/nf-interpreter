//
// Copyright (c) 2017 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

//  ESP32 GPIO ( 40 physical GPIO pads)
//
//  GPIO6-11 used for PSI flash
//
//  GPIO34-39 Only input mode
//

#include <targetPAL.h>
#include "win_dev_gpio_native.h"
#include "nf_rt_events_native.h"

#include "Esp32_DeviceMapping.h"


///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


void Gpio_Interupt_ISR(GPIO_PIN pinNumber, bool pinState, void* param )
{
   NATIVE_INTERRUPT_START

   CLR_RT_HeapBlock*  pThis = (CLR_RT_HeapBlock * )param;
   if ( pThis == NULL )
   {
	   NATIVE_INTERRUPT_END
	   return;
   }
	
   // check if object has been disposed
   if( pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
   {
	   // object has been disposed, leave now
	   NATIVE_INTERRUPT_END
	   return;
   }

   // flag to determine if there are any callbacks registered in managed code
   bool callbacksRegistered = (pThis[  Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___callbacks ].Dereference() != NULL);
   if ( callbacksRegistered )
   {
		// if handle registed then post a managed event with the current pin reading
		PostManagedEvent( EVENT_GPIO, 0, (uint16_t)pinNumber, pinState?1:0 );
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

		stack.SetResult_I4( CPU_GPIO_GetPinState(pinNumber)?1:0 );
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
		if ((driveMode == GpioPinDriveMode_Output) ||
			(driveMode == GpioPinDriveMode_OutputOpenDrain) ||
			(driveMode == GpioPinDriveMode_OutputOpenDrainPullUp) ||
			(driveMode == GpioPinDriveMode_OutputOpenSource) ||
			(driveMode == GpioPinDriveMode_OutputOpenSourcePullDown))
		{
			// ESP32 GPIO API doesn't offer a 'toggle', so need to rely on the last output value field and toggle that one
			GpioPinValue newState = (GpioPinValue)(GpioPinValue_High ^ (GpioPinValue)pThis[ FIELD___lastOutputValue ].NumericByRef().s4);
			
			// ...write back to the GPIO...
//			CPU_GPIO_SetPinState(pinNumber, newState != 0 );
			gpio_set_level((gpio_num_t)pinNumber, newState);


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
		
		CPU_GPIO_DisablePin(pinNumber, GpioPinDriveMode_Input, GPIO_ALT_PRIMARY);
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
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);
		bool validPin;

		if(pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___disposedValue ].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		GPIO_PIN pinNumber = (GPIO_PIN)pThis[ FIELD___pinNumber ].NumericByRefConst().s4;
		GpioPinDriveMode driveMode = (GpioPinDriveMode)stack.Arg1().NumericByRef().s4;

		if (driveMode >= (int)GpioPinDriveMode_Output)
		{
			validPin = CPU_GPIO_EnableOutputPin(pinNumber, false, driveMode);
		}
		else
		{
			int64_t debounceTimeoutMilsec = (CLR_INT64_TEMP_CAST) pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___debounceTimeout ].NumericByRefConst().s8 / TIME_CONVERSION__TO_MILLISECONDS;
			
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
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		GPIO_PIN pinNumber = (GPIO_PIN)stack.Arg1().NumericByRef().s4;

		int64_t debounceTimeoutMilsec = (CLR_INT64_TEMP_CAST) pThis[ Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioPin::FIELD___debounceTimeout ].NumericByRefConst().s8 / TIME_CONVERSION__TO_MILLISECONDS;

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
			CPU_GPIO_SetPinState(pinNumber, (state != 0) );

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

		Esp32_SetMappedDevicePins( (uint8_t)pinNumber, alternateFunction );
	}
	NANOCLR_NOCLEANUP();
}
