//
// Copyright (c) 2019 The nanoFramework project contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "win_dev_gpio_native.h"
#include <core.h>

typedef Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCount GpioChangeCount;
typedef Library_corlib_native_System_TimeSpan TimeSpan;

// Map Gpio pin number to 1 of 8 ESP32 counters, -1 = not mapped
// Each pulse counter is a 16 bit signed value.
// The managed code requires a 64 bit counter so we accumulate the overflows in an interrupt when count gets to 0x7fff

static int8_t   IsrInstalled = 0;
static int8_t   GpioCounterMap[8] = { -1,-1,-1,-1,-1,-1,-1,-1 };
static uint64_t OverflowCount[8];

#define MAX_COUNTER_VALUE	0x7fff

extern void Remove_Gpio_Interrupt(gpio_num_t pinNumber);


// Interrupt routine used to catch overflows so we can have a 64 bit counter
static void IRAM_ATTR pcnt_intr_handler(void *arg)
{
	(void)arg;

	uint32_t intr_status = PCNT.int_st.val;

	for (int i = 0; i < PCNT_UNIT_MAX; i++)
	{
		if (intr_status & (BIT(i)))
		{

			uint32_t status = PCNT.status_unit[i].val;
			// Hit high limit MAX_COUNTER_VALUE
			if (status & PCNT_STATUS_H_LIM_M)
			{
				OverflowCount[i] += MAX_COUNTER_VALUE;
			}

			PCNT.int_clr.val = BIT(i);
		}
	}
}


// Find an unused counter, returns counter index or -1 if not found
static int  FindFreeCounter(int gpioPin)
{
	int counterIndex;

	for (counterIndex = 0; counterIndex < 8; counterIndex++)
	{
		if (GpioCounterMap[counterIndex] == -1)
		{
			GpioCounterMap[counterIndex] = gpioPin;
			return counterIndex;
		}
	}
	return -1;
}

// Find the index of counter for a gpio pin, returns index or -1 if not found
static int  FindCounterForGpio(int gpioPin)
{
	int counterIndex;

	for (counterIndex = 0; counterIndex < 8; counterIndex++)
	{
		if (GpioCounterMap[counterIndex] == gpioPin)  return counterIndex;
	}
	return -1;
}

// Initalise the ESP32 counter
// return true if ok
static bool InitialiseCounter(int counterIndex, int gpioNum, bool countRising, bool countFalling)
{
	esp_err_t ec;

	// Prepare configuration for the PCNT unit */
	pcnt_config_t pcnt_config;

	// Set PCNT input signal and control GPIOs
	pcnt_config.pulse_gpio_num = gpioNum;
	pcnt_config.ctrl_gpio_num = PCNT_PIN_NOT_USED;        // ignore

	pcnt_config.channel = PCNT_CHANNEL_0;
	pcnt_config.unit = (pcnt_unit_t)counterIndex;

	// What to do on the positive / negative edge of pulse input?
	pcnt_config.pos_mode = countRising ? PCNT_COUNT_INC : PCNT_COUNT_DIS;   // positive edge count ?
	pcnt_config.neg_mode = countFalling ? PCNT_COUNT_INC : PCNT_COUNT_DIS;   // falling edge count ?

	// What to do when control input is low or high?
	pcnt_config.lctrl_mode = PCNT_MODE_KEEP;    // Keep the primary counter mode if low
	pcnt_config.hctrl_mode = PCNT_MODE_KEEP;    // Keep the primary counter mode if high

	// Set the maximum and minimum limit values to watch
	pcnt_config.counter_h_lim = MAX_COUNTER_VALUE;
	pcnt_config.counter_l_lim = 0;

	//* Initialize PCNT unit
	ec = pcnt_unit_config(&pcnt_config);
	if (ec != ESP_OK) return false;

	return true;
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter::NativeInit___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		int16_t pinNumber = pThis[FIELD___pinNumber].NumericByRefConst().s4;

		bool InputMode = (bool)pThis[FIELD___inputMode].NumericByRefConst().u1;
		if (InputMode)
		{
			int index = FindFreeCounter(pinNumber);
			if (index == -1)
			{
				// No free counters
				NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
			}

			// Disable GPIO input pin as we use separate counter
			CPU_GPIO_DisablePin(pinNumber, GpioPinDriveMode::GpioPinDriveMode_Input, 0);

			// Reserve pin for Counter use
			CPU_GPIO_ReservePin(pinNumber, true);
		}
		else
		{
			// Output not currently supported
			NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
		}

	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter::NativeRead___WindowsDevicesGpioGpioChangeCount__BOOLEAN( CLR_RT_StackFrame& stack )
{
	NANOCLR_HEADER();
	{
		CLR_RT_TypeDef_Index gpioChangeCountTypeDef;
		CLR_RT_HeapBlock* gpioChangeCount;
		
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		// check if object has been disposed
		if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		int pinNumber = pThis[FIELD___pinNumber].NumericByRefConst().s4;

		int counterIndex = FindCounterForGpio(pinNumber);
		if (counterIndex == -1)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
		}

		bool resetAfterRead = (bool)stack.Arg0().NumericByRef().u1;

		int16_t counter;

		pcnt_get_counter_value((pcnt_unit_t)counterIndex, &counter);

		// relativeTime Read Time,  Number of micro seconds since boot
		int64_t relativeTime = esp_timer_get_time();

		// Combine to make a 64 bit value
		uint64_t totalCount = OverflowCount[counterIndex] + (uint64_t)counter;

		if (resetAfterRead)
		{
			pcnt_counter_clear((pcnt_unit_t)counterIndex);
			OverflowCount[counterIndex] = 0;
		}

		// push return value to stack
		CLR_RT_HeapBlock& top   = stack.PushValue();

		// find <GpioGpioChangeCount> type definition, don't bother checking the result as it exists for sure
		g_CLR_RT_TypeSystem.FindTypeDef( "GpioChangeCount", "Windows.Devices.Gpio", gpioChangeCountTypeDef );

		// create an instance of <GpioGpioChangeCount> in the stack
		NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, gpioChangeCountTypeDef));

		// dereference the <GpioGpioChangeCount> object in order to reach its fields
		gpioChangeCount = top.Dereference();

		// set fields

		gpioChangeCount[GpioChangeCount::FIELD__Count].NumericByRef().u8 = totalCount;

		// relative time is a TimeSpan, so needs to be access through a pointer
        CLR_INT64* val = TimeSpan::GetValuePtr( gpioChangeCount[GpioChangeCount::FIELD__RelativeTime] );

		// timespan in milliseconds, but...
        *val = (CLR_UINT64)relativeTime;
		// ... need to convert to ticks with this
        *val *= TIME_CONVERSION__TICKUNITS;

	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter::NativeStart___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();
	{
		bool countRising = false;
		bool countFalling = false;

		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		// check if object has been disposed
		if (pThis[Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter::FIELD___disposedValue].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		int pinNumber = pThis[FIELD___pinNumber].NumericByRefConst().s4;

		int counterIndex = FindCounterForGpio(pinNumber);
		if (counterIndex == -1)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
		}

		GpioChangePolarity polarity = (GpioChangePolarity)pThis[FIELD___polarity].NumericByRefConst().s4;

		switch (polarity)
		{
			case Both:  countRising = true; countFalling = true; break;
			case Rising:  countRising = true; break;
			case Falling: countFalling = true; break;
		}

		if (!InitialiseCounter(counterIndex, pinNumber, countRising, countFalling))
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
		}

		// Posible filter ( debounce ) future.
		//    pcnt_set_filter_value((pcnt_unit_t)counterIndex, 10);
		//    pcnt_filter_enable((pcnt_unit_t)counterIndex);

		pcnt_event_enable((pcnt_unit_t)counterIndex, PCNT_EVT_H_LIM);

		pcnt_counter_pause((pcnt_unit_t)counterIndex);

		pcnt_counter_clear((pcnt_unit_t)counterIndex);
		OverflowCount[counterIndex] = 0;

		// Register ISR handler and enable interrupts for PCNT unit */
		if (IsrInstalled == 0)
		{
			pcnt_isr_register(pcnt_intr_handler, NULL, 0, NULL);
			IsrInstalled = 1;
		}

		// enable interrupts for PCNT unit
		pcnt_intr_enable((pcnt_unit_t)counterIndex);

		pcnt_counter_resume((pcnt_unit_t)counterIndex);
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter::NativeStop___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		// check if object has been disposed
		if (pThis[Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter::FIELD___disposedValue].NumericByRef().u1 != 0)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
		}

		int pinNumber = pThis[FIELD___pinNumber].NumericByRefConst().s4;

		int counterIndex = FindCounterForGpio(pinNumber);
		if (counterIndex == -1)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
		}

		pcnt_counter_pause((pcnt_unit_t)counterIndex);
	}
	NANOCLR_NOCLEANUP();
}

HRESULT Library_win_dev_gpio_native_Windows_Devices_Gpio_GpioChangeCounter::NativeDispose___VOID(CLR_RT_StackFrame& stack)
{
	NANOCLR_HEADER();
	{
		CLR_RT_HeapBlock*  pThis = stack.This();  FAULT_ON_NULL(pThis);

		int pinNumber = pThis[FIELD___pinNumber].NumericByRefConst().s4;

		int counterIndex = FindCounterForGpio(pinNumber);
		if (counterIndex == -1)
		{
			NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
		}

		pcnt_intr_disable((pcnt_unit_t)counterIndex);

		// Disable counter, remove gpio pin
		InitialiseCounter(counterIndex, PCNT_PIN_NOT_USED, false, false);

		// Clear counter / gpio mapping
		GpioCounterMap[counterIndex] = -1;

		CPU_GPIO_ReservePin(pinNumber, false);
	}
	NANOCLR_NOCLEANUP();
}
