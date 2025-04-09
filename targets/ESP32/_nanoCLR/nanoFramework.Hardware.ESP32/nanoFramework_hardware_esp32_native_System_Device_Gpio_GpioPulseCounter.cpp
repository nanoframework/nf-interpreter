//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"
#include <Core.h>

// PCNT device not available of the ESP32-C3
#if !defined(CONFIG_IDF_TARGET_ESP32C3)

#include "driver/pulse_cnt.h"

typedef Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCount GpioPulseCount;
typedef Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter GpioPulseCounter;
typedef Library_corlib_native_System_TimeSpan TimeSpan;

// Counter by target device
// ESP32 - 8, ESP32_S2 = 4, ESP32S3 = 4, ESP32_C3 = 0, ESP32_C6 = 4, , ESP32_H2 = 4
// As each target device type has a different number of counters
// Calculate number of counters in target
#define PCNT_NUM_UNIT (SOC_PCNT_GROUPS * SOC_PCNT_UNITS_PER_GROUP)

// Maximum count value from driver
#define MAX_COUNTER_VALUE 0x7fff

// Pulse counter information entry held in "counterEntries" array when active, inactive entries will be NULL.
struct PulseCounterEntry
{
    int8_t gpioPinA;
    int8_t gpioPinB;
    pcnt_unit_handle_t unitHandle;
    pcnt_channel_handle_t chanHandleA;
    pcnt_channel_handle_t chanHandleB;
    int64_t overflowCount;
};

static PulseCounterEntry *counterEntries[PCNT_NUM_UNIT];
static int8_t numberCounterUsed = 0;

// Callback routine used to catch overflows/underflows so we can have a 64 bit counter
static IRAM_ATTR bool pcnt_callback_handler(
    pcnt_unit_handle_t unit,
    const pcnt_watch_event_data_t *edata,
    void *user_ctx)
{
    PulseCounterEntry *entry = counterEntries[(int)user_ctx];
    if (entry != NULL)
    {
        if (edata->watch_point_value == MAX_COUNTER_VALUE)
        {
            entry->overflowCount += MAX_COUNTER_VALUE;
        }

        if (edata->watch_point_value == -MAX_COUNTER_VALUE)
        {
            entry->overflowCount -= MAX_COUNTER_VALUE;
        }
    }

    return false;
}

// Find & create an unused counter entry, returns index or -1 if not available
static int FindFreeCounter(int gpioPinA, int gpioPinB)
{
    int counterIndex;

    for (counterIndex = 0; counterIndex < PCNT_NUM_UNIT; counterIndex++)
    {
        if (counterEntries[counterIndex] == NULL)
        {
            PulseCounterEntry *counter = new PulseCounterEntry();
            counterEntries[counterIndex] = counter;
            counter->gpioPinA = gpioPinA;
            counter->gpioPinB = gpioPinB;
            counter->overflowCount = 0;

            numberCounterUsed++;
            return counterIndex;
        }
    }

    return -1;
}

// Find the pcnt_unit_handle_t  of counter for a GPIO pin, returns index or -1 if not found
static int FindCounterForGpio(int gpioPin)
{
    int counterIndex;

    for (counterIndex = 0; counterIndex < PCNT_NUM_UNIT; counterIndex++)
    {
        if (counterEntries[counterIndex] != NULL && counterEntries[counterIndex]->gpioPinA == gpioPin)
        {
            return counterIndex;
        }
    }

    return -1;
}

// Initialize the ESP32 counter
// Create unit & channels
// return true if OK
static pcnt_unit_handle_t InitialiseCounter(int counterIndex)
{
    esp_err_t ec;

    if (counterIndex >= SOC_PCNT_UNITS_PER_GROUP)
    {
        return NULL;
    }

    PulseCounterEntry *counter = counterEntries[counterIndex];

    // Prepare configuration for the PCNT unit */
    pcnt_unit_config_t unit_config = {};

    unit_config.low_limit = -MAX_COUNTER_VALUE;
    unit_config.high_limit = MAX_COUNTER_VALUE;

    counter->unitHandle = NULL;
    ec = pcnt_new_unit(&unit_config, &counter->unitHandle);
    if (ec != ESP_OK)
    {
        return NULL;
    }

    pcnt_chan_config_t chan_config_a = {};
    chan_config_a.edge_gpio_num = counter->gpioPinA;
    chan_config_a.level_gpio_num = counter->gpioPinB;

    counter->chanHandleA = NULL;
    counter->chanHandleB = NULL;

    ec |= pcnt_new_channel(counter->unitHandle, &chan_config_a, &counter->chanHandleA);

    // Add 2nd channel if required
    if (counter->gpioPinB >= 0)
    {
        pcnt_chan_config_t chan_config_b = {};
        chan_config_b.edge_gpio_num = counter->gpioPinB;
        chan_config_b.level_gpio_num = counter->gpioPinA;

        pcnt_new_channel(counter->unitHandle, &chan_config_b, &counter->chanHandleB);
    }

    pcnt_unit_add_watch_point(counter->unitHandle, MAX_COUNTER_VALUE);
    pcnt_unit_add_watch_point(counter->unitHandle, -MAX_COUNTER_VALUE);
    pcnt_unit_clear_count(counter->unitHandle);

    // Register callbacks
    pcnt_event_callbacks_t cbs = {};
    cbs.on_reach = pcnt_callback_handler;
    pcnt_unit_register_event_callbacks(counter->unitHandle, &cbs, (void *)counterIndex);

    return (ec == ESP_OK) ? counter->unitHandle : NULL;
}

static pcnt_unit_handle_t InitialiseEdgeAndActions(int counterIndex, GpioPulsePolarity polarity)
{
    esp_err_t ec = ESP_OK;
    bool countRising = false;
    bool countFalling = false;

    PulseCounterEntry *counter = counterEntries[counterIndex];

    switch (polarity)
    {
        case GpioPulsePolarity_Both:
            countRising = true;
            countFalling = true;
            break;

        case GpioPulsePolarity_Rising:
            countRising = true;
            countFalling = false;
            break;

        case GpioPulsePolarity_Falling:
            countRising = false;
            countFalling = true;
            break;
    }

    ec |= pcnt_channel_set_edge_action(
        counter->chanHandleA,
        countRising ? PCNT_CHANNEL_EDGE_ACTION_INCREASE : PCNT_CHANNEL_EDGE_ACTION_HOLD,
        countFalling ? PCNT_CHANNEL_EDGE_ACTION_INCREASE : PCNT_CHANNEL_EDGE_ACTION_HOLD);

    ec |= pcnt_channel_set_level_action(
        counter->chanHandleA,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP,
        PCNT_CHANNEL_LEVEL_ACTION_KEEP);

    if (counter->gpioPinB >= 0)
    {
        pcnt_channel_set_edge_action(
            counter->chanHandleB,
            PCNT_CHANNEL_EDGE_ACTION_INCREASE,
            PCNT_CHANNEL_EDGE_ACTION_DECREASE);
        pcnt_channel_set_level_action(
            counter->chanHandleB,
            PCNT_CHANNEL_LEVEL_ACTION_KEEP,
            PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
        pcnt_channel_set_level_action(
            counter->chanHandleA,
            PCNT_CHANNEL_LEVEL_ACTION_KEEP,
            PCNT_CHANNEL_LEVEL_ACTION_INVERSE);
    }

    return (ec == ESP_OK) ? counter->unitHandle : NULL;
}

static void DisposeCounter(int counterIndex)
{
    PulseCounterEntry *counter = counterEntries[counterIndex];
    if (counter != NULL)
    {
        pcnt_unit_disable(counter->unitHandle);

        pcnt_unit_remove_watch_point(counter->unitHandle, 0);
        pcnt_unit_remove_watch_point(counter->unitHandle, 1);

        pcnt_del_unit(counter->unitHandle);

        // delete channel A ?
        if (counter->chanHandleA != NULL)
        {
            pcnt_del_channel(counter->chanHandleA);
        }

        // delete channel B ?
        if (counter->chanHandleB != NULL)
        {
            pcnt_del_channel(counter->chanHandleB);
        }

        counterEntries[counterIndex] = NULL;
        delete counter;
    }
}

void PulseCountUninitialize()
{
    for (int counterIndex = 0; counterIndex < PCNT_NUM_UNIT; counterIndex++)
    {
        if (counterEntries[counterIndex] != NULL)
        {
            DisposeCounter(counterIndex);
        }
    }
}

#endif

HRESULT Library_nanoFramework_hardware_esp32_native_System_Device_Gpio_GpioPulseCounter::NativeInit___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
#if !defined(CONFIG_IDF_TARGET_ESP32C3)

        int16_t pinNumberA;
        int16_t pinNumberB;
        CLR_RT_HeapBlock *pThis = stack.This();

        FAULT_ON_NULL(pThis);

        if (numberCounterUsed == 0)
        {
            HAL_AddSoftRebootHandler(PulseCountUninitialize);

            // We need to initialize the array if it's the first one
            for (int i = 0; i < PCNT_NUM_UNIT; i++)
            {
                counterEntries[i] = NULL;
            }
        }

        pinNumberA = pThis[FIELD___pinNumberA].NumericByRefConst().s4;
        pinNumberB = pThis[FIELD___pinNumberB].NumericByRefConst().s4;
        if (pinNumberB < 0)
        {
            pinNumberB = -1;
        }

        int counterIndex = FindFreeCounter(pinNumberA, pinNumberB);
        if (counterIndex == -1)
        {
            // No free counters
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

        if (InitialiseCounter(counterIndex) == NULL)
        {
            DisposeCounter(counterIndex);
            NANOCLR_SET_AND_LEAVE(CLR_E_NOT_SUPPORTED);
        }

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
#if !defined(CONFIG_IDF_TARGET_ESP32C3)

        CLR_RT_TypeDef_Index gpioPulseCountTypeDef;
        CLR_RT_HeapBlock *gpioPulseCount;

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

        PulseCounterEntry *counter = counterEntries[counterIndex];

        bool resetAfterRead = (bool)stack.Arg1().NumericByRef().u1;

        int count;

        pcnt_unit_get_count(counter->unitHandle, &count);

        // relativeTime Read Time,  Number of micro seconds since boot
        int64_t relativeTime = esp_timer_get_time();

        // Combine to make a 64 bit value
        uint64_t totalCount = counter->overflowCount + (uint64_t)count;

        if (resetAfterRead)
        {
            pcnt_unit_clear_count(counter->unitHandle);
            counter->overflowCount = 0;
        }

        // push return value to stack
        CLR_RT_HeapBlock &top = stack.PushValue();

        // find <GpioGpioPulseCount> type definition, don't bother checking the result as it exists for sure
        g_CLR_RT_TypeSystem.FindTypeDef("GpioPulseCount", "System.Device.Gpio", gpioPulseCountTypeDef);

        // create an instance of <GpioGpioPulseCount> in the stack
        NANOCLR_CHECK_HRESULT(g_CLR_RT_ExecutionEngine.NewObjectFromIndex(top, gpioPulseCountTypeDef));

        // dereference the <GpioGpioPulseCount> object in order to reach its fields
        gpioPulseCount = top.Dereference();

        // set fields

        gpioPulseCount[GpioPulseCount::FIELD__Count].NumericByRef().s8 = totalCount;

        // relative time is a TimeSpan, so needs to be access through a pointer
        CLR_INT64 *val = TimeSpan::GetValuePtr(gpioPulseCount[GpioPulseCount::FIELD__RelativeTime]);

        // timespan in milliseconds, but...
        *val = (CLR_UINT64)relativeTime;
        // ... need to convert to ticks with this
        *val *= TIME_CONVERSION__TICKUNITS / 1000;

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
#if !defined(CONFIG_IDF_TARGET_ESP32C3)

        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        // check if object has been disposed
        if (pThis[GpioPulseCounter::FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        int counterIndex = FindCounterForGpio(pThis[FIELD___pinNumberA].NumericByRefConst().s4);
        if (counterIndex == -1)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }

        pcnt_unit_handle_t unitHandle = counterEntries[counterIndex]->unitHandle;

        // Between Stop and Start the Polarity and filter can change
        // must be disabled to change
        pcnt_unit_disable(unitHandle);

        GpioPulsePolarity polarity = (GpioPulsePolarity)pThis[FIELD___polarity].NumericByRefConst().s4;
        InitialiseEdgeAndActions(counterIndex, polarity);

        // Apply filter.
        uint16_t filter = pThis[GpioPulseCounter::FIELD___filter].NumericByRef().u2;

        pcnt_glitch_filter_config_t filterConfig;
        filterConfig.max_glitch_ns = filter;
        pcnt_unit_set_glitch_filter(unitHandle, &filterConfig);

        pcnt_unit_clear_count(unitHandle);
        counterEntries[counterIndex]->overflowCount = 0;

        pcnt_unit_enable(unitHandle);

        pcnt_unit_start(unitHandle);

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
#if !defined(CONFIG_IDF_TARGET_ESP32C3)

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

        pcnt_unit_stop(counterEntries[counterIndex]->unitHandle);

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
#if !defined(CONFIG_IDF_TARGET_ESP32C3)

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

        // Disable counter, remove GPIO pin
        DisposeCounter(counterIndex);

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
