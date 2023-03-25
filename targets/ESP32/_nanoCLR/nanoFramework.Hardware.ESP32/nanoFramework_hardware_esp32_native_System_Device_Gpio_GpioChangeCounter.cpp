//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"
#include <Core.h>
#include "pcnt.h"

typedef Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCount GpioPulseCount;
typedef Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter GpioPulseCounter;
typedef Library_corlib_native_System_TimeSpan TimeSpan;

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)
// Map Gpio pin number to 1 of 8 ESP32 counters, -1 = not mapped
// Each pulse counter is a 16 bit signed value.
// The managed code requires a 64 bit counter so we accumulate the overflows in an interrupt when count gets to 0x7fff

static bool isrInstalled = false;
static int8_t gpioCounterMap[PCNT_UNIT_MAX];
static int64_t overflowCount[PCNT_UNIT_MAX];
static int8_t numberCounterUsed = 0;

#define MAX_COUNTER_VALUE 0x7fff

// Interrupt routine used to catch overflows so we can have a 64 bit counter
static void IRAM_ATTR pcnt_intr_handler(void *arg)
{
    int counterIndex = (int)arg;

    uint32_t status;
    pcnt_get_event_status((pcnt_unit_t)counterIndex, &status);

    if (status & PCNT_EVT_H_LIM)
    {
        overflowCount[counterIndex] += MAX_COUNTER_VALUE;
    }

    if (status & PCNT_EVT_L_LIM)
    {
        overflowCount[counterIndex] -= MAX_COUNTER_VALUE;
    }
}

// Find an unused counter, returns counter index or -1 if not found
static int FindFreeCounter(int gpioPin)
{
    int counterIndex;

    for (counterIndex = 0; counterIndex < 8; counterIndex++)
    {
        if (gpioCounterMap[counterIndex] == -1)
        {
            gpioCounterMap[counterIndex] = gpioPin;
            numberCounterUsed++;
            return counterIndex;
        }
    }

    return -1;
}

// Find the index of counter for a gpio pin, returns index or -1 if not found
static int FindCounterForGpio(int gpioPin)
{
    int counterIndex;

    for (counterIndex = 0; counterIndex < PCNT_UNIT_MAX; counterIndex++)
    {
        if (gpioCounterMap[counterIndex] == gpioPin)
        {
            return counterIndex;
        }
    }

    return -1;
}

// Initalise the ESP32 counter
// return true if ok
static bool InitialiseCounter(int counterIndex, int gpioNumA, int gpioNumB, bool countRising, bool countFalling)
{
    esp_err_t ec;

    // Prepare configuration for the PCNT unit */
    pcnt_config_t pcnt_config;

    // Set PCNT input signal and control GPIOs
    pcnt_config.pulse_gpio_num = gpioNumA;
    pcnt_config.ctrl_gpio_num = gpioNumB;

    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = (pcnt_unit_t)counterIndex;

    if (gpioNumB >= 0)
    {
        pcnt_config.pos_mode = PCNT_COUNT_DEC;
        pcnt_config.neg_mode = PCNT_COUNT_INC;
    }
    else
    {
        // What to do on the positive / negative edge of pulse input?
        pcnt_config.pos_mode = countRising ? PCNT_COUNT_INC : PCNT_COUNT_DIS;  // positive edge count ?
        pcnt_config.neg_mode = countFalling ? PCNT_COUNT_INC : PCNT_COUNT_DIS; // falling edge count ?
    }

    // What to do when control input is low or high?
    pcnt_config.lctrl_mode = gpioNumB < 0 ? PCNT_MODE_KEEP : PCNT_MODE_REVERSE; // Keep the primary counter mode if low
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;                                    // Keep the primary counter mode if high

    // Set the maximum and minimum limit values to watch
    pcnt_config.counter_h_lim = MAX_COUNTER_VALUE;
    pcnt_config.counter_l_lim = -MAX_COUNTER_VALUE;

    //* Initialize PCNT unit
    ec = pcnt_unit_config(&pcnt_config);

    if (gpioNumB >= 0)
    {
        pcnt_config.pulse_gpio_num = gpioNumB;
        pcnt_config.ctrl_gpio_num = gpioNumA;
        pcnt_config.channel = PCNT_CHANNEL_1;
        // Setup reverse behavior on second counter
        pcnt_config.pos_mode = PCNT_COUNT_INC;
        pcnt_config.neg_mode = PCNT_COUNT_DEC;
        ec |= pcnt_unit_config(&pcnt_config);
    }

    if ((gpioNumB < 0) && (gpioNumA < 0))
    {
        pcnt_config.pulse_gpio_num = gpioNumB;
        pcnt_config.ctrl_gpio_num = gpioNumA;
        pcnt_config.channel = PCNT_CHANNEL_1;
        pcnt_config.pos_mode = PCNT_COUNT_DIS;
        pcnt_config.neg_mode = PCNT_COUNT_DIS;
        ec |= pcnt_unit_config(&pcnt_config);
    }

    return ec == ESP_OK;
}

void PulseCountUninitialize()
{
    if (isrInstalled)
    {
        pcnt_isr_service_uninstall();
        isrInstalled = false;
        numberCounterUsed = 0;
    }
}

#endif

HRESULT Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter::NativeInit___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

        int16_t pinNumberA;
        int16_t pinNumberB;
        CLR_RT_HeapBlock *pThis = stack.This();

        FAULT_ON_NULL(pThis);

        if (numberCounterUsed == 0)
        {
            HAL_AddSoftRebootHandler(PulseCountUninitialize);

            // We need to initialize the array if it's the first one
            for (int i = 0; i < PCNT_UNIT_MAX; i++)
            {
                gpioCounterMap[i] = -1;
            }
        }

        pinNumberA = pThis[FIELD___pinNumberA].NumericByRefConst().s4;
        pinNumberB = pThis[FIELD___pinNumberB].NumericByRefConst().s4;
        if (pinNumberB < 0)
        {
            pinNumberB = PCNT_PIN_NOT_USED;
        }

        int index = FindFreeCounter(pinNumberA);
        if (index == -1)
        {
            // No free counters
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        overflowCount[index] = 0;
        // Reserve pin for Counter use
        CPU_GPIO_ReservePin(pinNumberA, true);
        if (pinNumberB >= 0)
        {
            CPU_GPIO_ReservePin(pinNumberB, true);
        }

#else
        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter::
    NativeRead___SystemDeviceGpioGpioPulseCount__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

        CLR_RT_TypeDef_Index GpioPulseCountTypeDef;
        CLR_RT_HeapBlock *GpioPulseCount;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int pinNumberA = pThis[FIELD___pinNumberA].NumericByRefConst().s4;

        int counterIndex = FindCounterForGpio(pinNumberA);

        if (counterIndex == -1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        bool resetAfterRead = (bool)stack.Arg1().NumericByRef().u1;

        int16_t counter;

        pcnt_get_counter_value((pcnt_unit_t)counterIndex, &counter);

        // relativeTime Read Time,  Number of micro seconds since boot
        int64_t relativeTime = esp_timer_get_time();

        // Combine to make a 64 bit value
        uint64_t totalCount = overflowCount[counterIndex] + (uint64_t)counter;

        if (resetAfterRead)
        {
            pcnt_counter_clear((pcnt_unit_t)counterIndex);
            overflowCount[counterIndex] = 0;
        }

        // push return value to stack
        CLR_RT_HeapBlock &top = stack.PushValue();

        // find <GpioGpioPulseCount> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef("GpioPulseCount", "System.Device.Gpio", GpioPulseCountTypeDef);

        // create an instance of <GpioGpioPulseCount> in the stack
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, GpioPulseCountTypeDef));

        // dereference the <GpioGpioPulseCount> object in order to reach its fields
        GpioPulseCount = top.Dereference();

        // set fields

        GpioPulseCount[GpioPulseCount::FIELD__Count].NumericByRef().s8 = totalCount;

        // relative time is a TimeSpan, so needs to be access through a pointer
        CLR_INT64 *val = TimeSpan::GetValuePtr(GpioPulseCount[GpioPulseCount::FIELD__RelativeTime]);

        // timespan in milliseconds, but...
        *val = (CLR_UINT64)relativeTime;
        // ... need to convert to ticks with this
        *val *= TIME_CONVERSION__TICKUNITS;

#else
        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter::NativeStart___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

        bool countRising = false;
        bool countFalling = false;

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if (pThis[GpioPulseCounter::FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int pinNumberA = pThis[FIELD___pinNumberA].NumericByRefConst().s4;
        int pinNumberB = pThis[FIELD___pinNumberB].NumericByRefConst().s4;

        int counterIndex = FindCounterForGpio(pinNumberA);
        if (counterIndex == -1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        GpioChangePolarity polarity = (GpioChangePolarity)pThis[FIELD___polarity].NumericByRefConst().s4;

        switch (polarity)
        {
            case GpioChangePolarity_Both:
                countRising = true;
                countFalling = true;
                break;

            case GpioChangePolarity_Rising:
                countRising = true;
                countFalling = false;
                break;

            case GpioChangePolarity_Falling:
                countRising = false;
                countFalling = true;
                break;
        }

        if (!InitialiseCounter(counterIndex, pinNumberA, pinNumberB, countRising, countFalling))
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        // Apply filter.
        uint16_t filter = pThis[GpioPulseCounter::FIELD___filter].NumericByRef().u2;
        pcnt_set_filter_value((pcnt_unit_t)counterIndex, filter);
        pcnt_filter_enable((pcnt_unit_t)counterIndex);

        pcnt_event_enable((pcnt_unit_t)counterIndex, PCNT_EVT_H_LIM);
        pcnt_event_enable((pcnt_unit_t)counterIndex, PCNT_EVT_L_LIM);

        pcnt_counter_pause((pcnt_unit_t)counterIndex);

        pcnt_counter_clear((pcnt_unit_t)counterIndex);
        overflowCount[counterIndex] = 0;

        // Register ISR handler and enable interrupts for PCNT unit */
        if (isrInstalled == false)
        {
            pcnt_isr_service_install(0);
            isrInstalled = true;
        }

        pcnt_isr_handler_add((pcnt_unit_t)counterIndex, pcnt_intr_handler, (void *)counterIndex);

        // enable interrupts for PCNT unit
        pcnt_intr_enable((pcnt_unit_t)counterIndex);

        pcnt_counter_resume((pcnt_unit_t)counterIndex);

#else
        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter::NativeStop___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if (pThis[GpioPulseCounter::FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int pinNumberA = pThis[FIELD___pinNumberA].NumericByRefConst().s4;

        int counterIndex = FindCounterForGpio(pinNumberA);
        if (counterIndex == -1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        pcnt_counter_pause((pcnt_unit_t)counterIndex);

#else
        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter::NativeDispose___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32S3)

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        int pinNumberA = pThis[FIELD___pinNumberA].NumericByRefConst().s4;
        int pinNumberB = pThis[FIELD___pinNumberB].NumericByRefConst().s4;

        int counterIndex = FindCounterForGpio(pinNumberA);
        if (counterIndex == -1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        numberCounterUsed--;
        pcnt_isr_handler_remove((pcnt_unit_t)counterIndex);
        if (numberCounterUsed == 0)
        {
            pcnt_isr_service_uninstall();
            isrInstalled = false;
        }

        // Disable counter, remove gpio pin
        InitialiseCounter(counterIndex, PCNT_PIN_NOT_USED, PCNT_PIN_NOT_USED, false, false);

        // Clear counter / gpio mapping
        gpioCounterMap[counterIndex] = -1;

        CPU_GPIO_ReservePin(pinNumberA, false);
        if (pinNumberB >= 0)
        {
            CPU_GPIO_ReservePin(pinNumberB, false);
        }

#else
        NANOCLR_SET_AND_LEAVE(stack.NotImplementedStub());
#endif
    }
    NANOCLR_NOCLEANUP();
}
