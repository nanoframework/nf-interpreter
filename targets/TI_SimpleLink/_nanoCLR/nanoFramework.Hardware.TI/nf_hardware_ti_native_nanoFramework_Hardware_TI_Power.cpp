//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_hardware_ti_native.h"
#include <nanoCLR_Hardware.h>

// hack required to be able to config wakeup from deep sleep
// bug introduced in SDK 5.30.01.01.
// waiting for a proper fix in a future SDK
extern "C"
{
    PINCC26XX_HWAttrs PINCC26XX_hwAttrs = {.intPriority = ~0, .swiPriority = 0};

    const PIN_Config BoardGpioInitTable[] = {PIN_TERMINATE};

    void PIN_init_nano()
    {
        Power_init();

        if (PIN_init(BoardGpioInitTable) != PIN_SUCCESS)
        {
            // Error with PIN_init
            while (1)
                ;
        }
    }
}

// declare a shorter type to make code readable
typedef Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power__PinWakeupConfig PinWakeupConfig;

HRESULT Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::
    ConfigureWakeupFromGpioPin___STATIC__VOID__SZARRAY_nanoFrameworkHardwareTIPowerPinWakeupConfig(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    int32_t configCount;
    int32_t index = 0;
    PIN_Config *wakeupConfigurations = NULL;
    CLR_RT_HeapBlock *pinWakeupConfig = NULL;

    CLR_RT_HeapBlock_Array *configArray = stack.Arg0().DereferenceArray();
    FAULT_ON_NULL(configArray);

    // how many configurations are in the array
    configCount = configArray->m_numOfElements;

    if (configCount > 0)
    {
        // allocate config array
        // need an extra position to store the "terminator"
        wakeupConfigurations = (PIN_Config *)platform_malloc((configCount + 1) * sizeof(PIN_Config));

        // check allocation
        if (wakeupConfigurations != NULL)
        {
            // clear memory
            memset(wakeupConfigurations, 0, configCount * sizeof(PIN_Config));

            // build the array of configuration
            while (index < configCount)
            {
                // get a pointer to the configuration
                pinWakeupConfig = ((CLR_RT_HeapBlock *)configArray->GetElement(index))->Dereference();

                // get pin number
                wakeupConfigurations[index] = PIN_ID(pinWakeupConfig[PinWakeupConfig::FIELD___pin].NumericByRef().s4);

                // pin is always input
                wakeupConfigurations[index] |= PIN_INPUT_EN | PIN_NOPULL | PIN_IRQ_DIS;

                // grab and set pullup/down option
                wakeupConfigurations[index] |=
                    (Power_PinPullUpDown)pinWakeupConfig[PinWakeupConfig::FIELD___pinPullUpDown].NumericByRef().u4;

                // grab and set wakeup option
                wakeupConfigurations[index] |=
                    (Power_PinWakeupEdge)pinWakeupConfig[PinWakeupConfig::FIELD___wakeupConfig].NumericByRef().u4;

                // move to the next configuration element
                index++;
            }

            // add "terminator" to the array
            wakeupConfigurations[index] = PIN_TERMINATE;

            // configure DIO for wakeup from shutdown
            PINCC26XX_setWakeup(wakeupConfigurations);

            // done here, free memory
            platform_free(wakeupConfigurations);
        }
        else
        {
            // failed to allocate memory
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::
    get_SourceOfReset___STATIC__nanoFrameworkHardwareTIPowerResetSource(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    stack.SetResult_U4(WakeupReasonStore);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::NativeEnterShutdownMode___STATIC__VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    // set flags to stop debug...
    CLR_EE_DBG_SET(RebootPending);
    // ... reboot CLR so execution ends gracefully ...
    CLR_EE_REBOOT_CLR;
    // ... and set power level to OFF
    g_CLR_HW_Hardware.m_powerLevel = PowerLevel__Off;

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_hardware_ti_native_nanoFramework_Hardware_TI_Power::
    NativeEnterStandbyMode___STATIC__VOID__SystemTimeSpan(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT64 standbyDurationMilsec;

    // debounceTimeout field its a TimeSpan, which is a primitive type stored as an heap block, therefore needs to
    // be accessed indirectly
    CLR_INT64 *standbyDuration = Library_corlib_native_System_TimeSpan::GetValuePtr(stack.Arg0());
    FAULT_ON_NULL(standbyDuration);

    standbyDurationMilsec = *(CLR_UINT64 *)standbyDuration / TIME_CONVERSION__TO_SECONDS;

    ClockP_sleep(standbyDurationMilsec);

    // need to reset the CPU after waking from standby
    CPU_Reset();

    //////////////////////////////////////
    // THE EXECUTION NEVER REACHES HERE //
    //////////////////////////////////////

    NANOCLR_NOCLEANUP();
}
